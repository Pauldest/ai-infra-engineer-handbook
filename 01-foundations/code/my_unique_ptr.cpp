// =============================================================================
//  Week 2 精读教材：从零实现 unique_ptr —— 吃透移动语义与独占所有权
// =============================================================================
//
//  编译运行：
//    clang++ -std=c++17 -O2 -Wall my_unique_ptr.cpp -o my_unique_ptr && ./my_unique_ptr
//
//  unique_ptr 是理解「移动语义」最好的载体：
//    - 它【禁止拷贝】（独占），只能【移动】（转移所有权）。
//    - 内部只有一个裸指针 → 零开销，sizeof == sizeof(裸指针)。
//
//  配套讲解见文末「知识点速查」及同级目录 move-semantics-deep-dive.md
// =============================================================================

#include <iostream>
#include <utility>      // std::move, std::exchange
#include <vector>
#include <cassert>

namespace demo {

template <typename T>
class UniquePtr {
    T* ptr = nullptr;   // 内部就一个裸指针 —— 零开销的“安全外壳”

public:
    // --- 构造 ---
    UniquePtr() = default;
    explicit UniquePtr(T* p) : ptr(p) {}

    // --- 🔴 禁止拷贝：独占所有权，不允许两个 UniquePtr 指向同一对象 ---
    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // --- 🟢 允许移动：偷走 o 的指针，把 o 置空（转移所有权）---
    //     noexcept 很重要：容器（如 vector）扩容时只有在移动保证不抛异常时
    //     才会选择“移动”而非“拷贝”，否则为了异常安全会退回到拷贝。
    UniquePtr(UniquePtr&& o) noexcept : ptr(o.ptr) {
        o.ptr = nullptr;              // 源置空：这样源析构时不会 delete 同一块内存
    }

    UniquePtr& operator=(UniquePtr&& o) noexcept {
        if (this != &o) {             // 防自赋值（p = std::move(p)）
            delete ptr;               // 先释放自己原来持有的
            ptr = o.ptr;              // 接管 o 的资源
            o.ptr = nullptr;          // 源置空
        }
        return *this;
    }

    // --- 析构：RAII 核心，delete 自动释放 ---
    ~UniquePtr() { delete ptr; }

    // --- 接口 ---
    T& operator*()  const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get()        const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }

    // release：放弃所有权，返回裸指针（调用者负责释放）
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    // reset：释放当前对象，接管新的（或置空）
    void reset(T* p = nullptr) {
        delete ptr;                   // 先释放旧的
        ptr = p;
    }
};

// 类似 std::make_unique：完美转发参数，避免裸 new 暴露在外
template <typename T, typename... Args>
UniquePtr<T> makeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace demo

// =============================================================================
//  辅助类：打印拷贝 vs 移动，亲眼区分二者
// =============================================================================
struct Buffer {
    std::vector<int> data;
    Buffer(std::size_t n) : data(n) {
        std::cout << "  [构造] Buffer(" << n << " 个元素)\n";
    }
    Buffer(const Buffer& o) : data(o.data) {
        std::cout << "  [拷贝构造] 复制了 " << data.size() << " 个元素（昂贵）\n";
    }
    Buffer(Buffer&& o) noexcept : data(std::move(o.data)) {
        std::cout << "  [移动构造] 偷走底层数组（O(1)），源变空\n";
    }
};

using demo::UniquePtr;
using demo::makeUnique;

struct Widget {
    int id;
    Widget(int i) : id(i) { std::cout << "  [构造] Widget#" << id << "\n"; }
    ~Widget()             { std::cout << "  [析构] Widget#" << id << "\n"; }
};

// 演示：函数返回 UniquePtr（自动移动，无需手动 std::move 返回局部变量）
UniquePtr<Widget> makeWidget(int id) {
    return makeUnique<Widget>(id);
}

int main() {
    std::cout << "=== 1. 拷贝 vs 移动：性能差异从何而来 ===\n";
    {
        Buffer a(1000);
        std::cout << "--- b = a （拷贝）---\n";
        Buffer b = a;                    // 拷贝构造：复制 1000 个元素
        std::cout << "--- c = std::move(a) （移动）---\n";
        Buffer c = std::move(a);         // 移动构造：只偷指针 O(1)
        std::cout << "移动后 a.data.size() = " << a.data.size()
                  << "（源被掏空）\n";
    }
    std::cout << "\n";

    std::cout << "=== 2. UniquePtr：独占所有权，只能移动不能拷贝 ===\n";
    {
        UniquePtr<Widget> p = makeUnique<Widget>(1);
        std::cout << "p 是否持有对象: " << (p ? "是" : "否") << "\n";

        // UniquePtr<Widget> q = p;      // ❌ 编译错误：拷贝被 = delete
        UniquePtr<Widget> q = std::move(p);   // ✅ 移动：所有权 p → q
        std::cout << "move 后 p: " << (p ? "有" : "空")
                  << ", q: " << (q ? "有" : "空") << "\n";
    }   // q 析构 → Widget#1 释放
    std::cout << "\n";

    std::cout << "=== 3. 从函数返回（RVO/移动，无需手写 std::move）===\n";
    {
        UniquePtr<Widget> w = makeWidget(2);
        std::cout << "拿到 Widget#" << w->id << "\n";
    }
    std::cout << "\n";

    std::cout << "=== 4. release / reset ===\n";
    {
        UniquePtr<Widget> p = makeUnique<Widget>(3);
        Widget* raw = p.release();       // 放弃所有权，p 变空
        std::cout << "release 后 p: " << (p ? "有" : "空")
                  << "，裸指针 raw->id = " << raw->id << "\n";
        delete raw;                      // 现在要自己释放

        p.reset(new Widget(4));          // 接管新对象
        std::cout << "reset 后 p->id = " << p->id << "\n";
        p.reset();                       // 主动释放 Widget#4
        std::cout << "reset() 后 p: " << (p ? "有" : "空") << "\n";
    }
    std::cout << "\n";

    std::cout << "=== 5. 放进容器：只能移动进去 ===\n";
    {
        std::vector<UniquePtr<Widget>> vec;
        vec.push_back(makeUnique<Widget>(5));       // 临时对象 → 移动
        UniquePtr<Widget> local = makeUnique<Widget>(6);
        vec.push_back(std::move(local));            // 具名对象需显式 move
        std::cout << "vec 里有 " << vec.size() << " 个 Widget\n";
    }   // vec 析构 → 依次释放
    std::cout << "\n全部演示结束。\n";
    return 0;
}

// =============================================================================
//  📚 知识点速查
// -----------------------------------------------------------------------------
//  1. 左值 / 右值           —— 有名字、可取地址 = 左值；临时值 = 右值
//  2. 右值引用 T&&          —— 绑定右值（临时对象），是移动语义的语法基础
//  3. std::move             —— 不移动任何东西！只是把左值“强转”成右值引用
//  4. 移动构造 / 移动赋值    —— 偷资源（指针）而非复制，O(1)；源置空
//  5. noexcept 与移动        —— 容器扩容仅在移动 noexcept 时才用移动（异常安全）
//  6. = delete              —— 显式禁用拷贝，强制独占语义
//  7. 独占 vs 共享           —— unique_ptr 零开销独占；shared_ptr 引用计数共享
//  8. RVO / NRVO            —— 返回局部对象时编译器直接构造在返回位置，连移动都省
//  9. 完美转发 forward       —— makeUnique 用 forward 保持实参的左/右值类别
// 10. 防自赋值 this != &o    —— p = std::move(p) 时避免先 delete 再用空指针
// =============================================================================
