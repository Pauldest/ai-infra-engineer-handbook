// =============================================================================
//  Week 1 精读教材：从零实现 shared_ptr / weak_ptr —— 吃透 C++ 核心知识点
// =============================================================================
//
//  编译运行：
//    clang++ -std=c++17 -O2 -Wall -pthread my_shared_ptr.cpp -o my_shared_ptr && ./my_shared_ptr
//    （或 g++，命令相同）
//
//  这份文件的目标不是替代 std::shared_ptr，而是让你通过“自己造一遍”
//  彻底理解引用计数、RAII、原子操作、拷贝/移动语义、weak_ptr 等核心概念。
//
//  配套讲解见文末「17 个知识点速查」注释。
// =============================================================================

#include <atomic>     // std::atomic —— 原子类型，保证多线程下计数正确
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>

namespace demo {

// -----------------------------------------------------------------------------
// 控制块 (Control Block)：被所有共享同一对象的智能指针“共用”的一小块堆内存。
//   - 用 struct：纯数据聚合，成员默认 public，方便智能指针直接访问。
//   - strong：还有几个 shared_ptr 在用这个对象。归 0 → 释放【对象】。
//   - weak  ：还有几个 weak_ptr 在观察。strong 和 weak 都归 0 → 释放【控制块】。
//   - 两个计数都是 atomic：多线程拷贝/析构智能指针时，计数增减必须不可分割。
// -----------------------------------------------------------------------------
struct ControlBlock {
    std::atomic<long> strong{1};   // 类内默认初始化：控制块诞生时已有 1 个 shared_ptr
    std::atomic<long> weak{0};
};

// 前置声明，供 SharedPtr 声明友元 / weak_ptr 使用
template <typename T> class WeakPtr;

// =============================================================================
//  SharedPtr —— 共享所有权智能指针（引用计数）
// =============================================================================
template <typename T>
class SharedPtr {
    T*            ptr  = nullptr;   // 指向被管理对象
    ControlBlock* ctrl = nullptr;   // 指向共享控制块（所有副本指向同一个）

    friend class WeakPtr<T>;        // weak_ptr 需要访问私有成员来构造 shared_ptr

    // 私有：供 weak_ptr.lock() 使用的构造（已知对象存活时“提升”成 shared_ptr）
    SharedPtr(T* p, ControlBlock* c) : ptr(p), ctrl(c) {}

public:
    // --- 默认构造：空指针 ---
    SharedPtr() = default;

    // --- 从裸指针构造：接管所有权，新建控制块（strong 从 1 起步）---
    //     explicit：禁止裸指针隐式转成 SharedPtr，避免“悄悄接管”的 bug。
    explicit SharedPtr(T* p) : ptr(p), ctrl(p ? new ControlBlock{} : nullptr) {}

    // --- 拷贝构造：多一个人共享 → strong + 1 ---
    //     参数用 const&：传引用避免拷贝参数自身（否则无限递归），const 表示不改源。
    SharedPtr(const SharedPtr& o) : ptr(o.ptr), ctrl(o.ctrl) {
        if (ctrl) ++ctrl->strong;            // 原子自增；++ 优先级低于 ->，即 ++(ctrl->strong)
    }

    // --- 拷贝赋值：先接管 o，再释放旧的 ---
    SharedPtr& operator=(const SharedPtr& o) {
        if (this != &o) {                    // 防自赋值
            release();                       // 放掉当前持有的
            ptr = o.ptr; ctrl = o.ctrl;
            if (ctrl) ++ctrl->strong;
        }
        return *this;
    }

    // --- 移动构造：偷走 o 的指针，把 o 置空（不改变引用计数：只是换了持有者）---
    SharedPtr(SharedPtr&& o) noexcept : ptr(o.ptr), ctrl(o.ctrl) {
        o.ptr = nullptr; o.ctrl = nullptr;
    }

    // --- 移动赋值 ---
    SharedPtr& operator=(SharedPtr&& o) noexcept {
        if (this != &o) {
            release();
            ptr = o.ptr; ctrl = o.ctrl;
            o.ptr = nullptr; o.ctrl = nullptr;
        }
        return *this;
    }

    // --- 析构：strong - 1；归 0 释放对象；若 weak 也为 0 再释放控制块 ---
    ~SharedPtr() { release(); }

    // --- 常用接口 ---
    T& operator*()  const { return *ptr; }   // 解引用
    T* operator->() const { return ptr; }    // 成员访问
    T* get()        const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }

    // const 成员函数：承诺不修改对象，const 对象也能调用
    long use_count() const { return ctrl ? ctrl->strong.load() : 0; }

private:
    // 释放逻辑抽出来复用（析构 / 赋值都要用）
    void release() {
        if (!ctrl) return;
        if (--ctrl->strong == 0) {           // 原子递减，返回“减完后的新值”，故用前置 --
            delete ptr;                      // 最后一个持有者：释放对象
            if (ctrl->weak == 0)             // 没有 weak_ptr 观察了：连控制块一起释放
                delete ctrl;
        }
        ptr = nullptr; ctrl = nullptr;
    }
};

// =============================================================================
//  WeakPtr —— 弱引用：观察对象但不延长其生命周期，用于打破循环引用
// =============================================================================
template <typename T>
class WeakPtr {
    T*            ptr  = nullptr;
    ControlBlock* ctrl = nullptr;

public:
    WeakPtr() = default;

    // 从 shared_ptr 构造：只增加 weak 计数，不动 strong（关键！不延长对象寿命）
    WeakPtr(const SharedPtr<T>& s) : ptr(s.ptr), ctrl(s.ctrl) {
        if (ctrl) ++ctrl->weak;
    }

    WeakPtr(const WeakPtr& o) : ptr(o.ptr), ctrl(o.ctrl) {
        if (ctrl) ++ctrl->weak;
    }

    ~WeakPtr() { release(); }

    // 对象是否已被销毁
    bool expired() const { return !ctrl || ctrl->strong.load() == 0; }

    // lock：若对象仍存活，则“提升”成一个有效 shared_ptr（strong+1）；否则返回空
    SharedPtr<T> lock() const {
        if (ctrl && ctrl->strong.load() > 0) {
            ++ctrl->strong;                       // 抢一个强引用
            return SharedPtr<T>(ptr, ctrl);       // 用私有构造直接返回
        }
        return SharedPtr<T>();                    // 空
    }

private:
    void release() {
        if (!ctrl) return;
        if (--ctrl->weak == 0 && ctrl->strong == 0)
            delete ctrl;                          // 强、弱都归 0，控制块可回收
        ptr = nullptr; ctrl = nullptr;
    }
};

} // namespace demo

// =============================================================================
//  验证 main：亲眼观察引用计数、自动释放、移动、循环引用与 weak_ptr
// =============================================================================
using demo::SharedPtr;
using demo::WeakPtr;

struct Widget {
    int id;
    Widget(int i) : id(i) { std::cout << "  [构造] Widget#" << id << "\n"; }
    ~Widget()             { std::cout << "  [析构] Widget#" << id << "\n"; }
};

int main() {
    std::cout << "=== 1. 引用计数基本行为 ===\n";
    {
        SharedPtr<Widget> a(new Widget(1));
        std::cout << "a 创建后 count = " << a.use_count() << "\n";   // 1
        {
            SharedPtr<Widget> b = a;                                // 拷贝 → +1
            std::cout << "b = a 后 count = " << a.use_count() << "\n"; // 2
        }                                                           // b 析构 → -1
        std::cout << "b 离开作用域后 count = " << a.use_count() << "\n"; // 1
    }   // a 析构 → 0 → 释放 Widget#1
    std::cout << "（a 离开作用域，Widget#1 被自动释放）\n\n";

    std::cout << "=== 2. 移动语义（不改变计数，只换持有者）===\n";
    {
        SharedPtr<Widget> a(new Widget(2));
        std::cout << "移动前 count = " << a.use_count() << "\n";     // 1
        SharedPtr<Widget> c = std::move(a);                         // 偷走，a 变空
        std::cout << "移动后 c.count = " << c.use_count()
                  << ", a 是否为空 = " << (a ? "否" : "是") << "\n"; // 1, 是
    }
    std::cout << "\n";

    std::cout << "=== 3. weak_ptr：观察但不延长寿命 ===\n";
    {
        WeakPtr<Widget> w;
        {
            SharedPtr<Widget> s(new Widget(3));
            w = WeakPtr<Widget>(s);
            std::cout << "对象存活时 w.expired() = " << w.expired() << "\n"; // 0
            if (auto locked = w.lock())                                     // 提升成功
                std::cout << "  lock 成功，拿到 Widget#" << locked->id
                          << ", count = " << locked.use_count() << "\n";     // 2
        }   // s 析构 → Widget#3 释放（weak 不阻止）
        std::cout << "对象销毁后 w.expired() = " << w.expired() << "\n";    // 1
        std::cout << "  再 lock() → " << (w.lock() ? "非空" : "空") << "\n"; // 空
    }
    std::cout << "\n";

    std::cout << "=== 4. 多线程下引用计数安全（atomic）===\n";
    {
        SharedPtr<Widget> base(new Widget(4));
        std::vector<std::thread> ts;
        for (int i = 0; i < 8; ++i) {
            ts.emplace_back([base] {                 // 每个线程拷贝一份（各 +1/-1）
                for (int k = 0; k < 10000; ++k) {
                    SharedPtr<Widget> local = base;  // 拷贝 +1
                }                                    // 析构 -1
            });
        }
        for (auto& t : ts) t.join();
        std::cout << "8 线程各拷贝 1 万次后，count 应回到 1 → 实际 = "
                  << base.use_count() << "\n";
        assert(base.use_count() == 1);   // 若用普通 int 会因数据竞争导致此断言失败
        std::cout << "  ✅ atomic 保证了计数正确\n";
    }

    std::cout << "\n全部演示结束。\n";
    return 0;
}

// =============================================================================
//  📚 17 个知识点速查（对照 handbook 讲解）
// -----------------------------------------------------------------------------
//   1. struct vs class       —— 默认访问权限：struct=public, class=private
//   2. std::atomic<T>        —— 原子类型，多线程计数不可分割，避免丢失更新
//   3. 类内成员默认初始化 {1} —— C++11 default member initializer
//   4. template<typename T>  —— 类模板，编译期按类型生成代码（泛型）
//   5. 裸指针成员            —— shared_ptr 内部 = 对象指针 + 控制块指针
//   6. explicit              —— 禁止单参构造的隐式转换，防裸指针被“悄悄接管”
//   7. 成员初始化列表 : a(x)  —— 构造体执行前“初始化”成员（比赋值高效/必要）
//   8. new ControlBlock{}    —— 堆分配 + 值初始化（{} 花括号初始化）
//   9. 拷贝构造 const T&      —— 传引用防递归，const 不改源；浅拷贝指针 + 计数+1
//  10. ++ctrl->strong        —— 优先级：等价 ++(ctrl->strong)；atomic 原子自增
//  11. 析构函数 ~T()          —— 离开作用域自动调用（RAII 核心）
//  12. --strong == 0         —— 前置递减返回“新值”；atomic 保证只有一个线程见到 0
//  13. delete                —— 释放堆内存；对象与控制块分两层释放（因 weak_ptr）
//  14. const 成员函数        —— 承诺不改成员，const 对象也可调用
//  15. .load()               —— 原子读取当前值，避免读到中间状态
//  16. 三目运算符 ?:          —— ctrl ? ... : 0，空指针保护
//  17. -> 指针成员访问        —— ctrl->x 等价 (*ctrl).x
// =============================================================================
