# Week 1 精读：从零实现 `shared_ptr` —— 17 个 C++ 知识点详解

> 通过手写一个引用计数智能指针，一次性吃透 C++ 的封装、RAII、模板、拷贝/移动语义、原子操作等核心概念。
>
> 配套可运行代码：[`code/my_shared_ptr.cpp`](./code/my_shared_ptr.cpp)、[`code/cyclic_reference.cpp`](./code/cyclic_reference.cpp)

## 如何运行

```bash
cd code
clang++ -std=c++17 -O2 -Wall -pthread my_shared_ptr.cpp -o my_shared_ptr && ./my_shared_ptr
clang++ -std=c++17 -O2 -Wall cyclic_reference.cpp -o cyclic && ./cyclic
```
（macOS/Linux 用 `clang++` 或 `g++` 均可，命令相同。）

---

## 被讲解的核心代码

```cpp
struct ControlBlock {
    std::atomic<long> strong{1};   // 强引用计数（有几个 shared_ptr）
    std::atomic<long> weak{0};     // 弱引用计数（有几个 weak_ptr）
};

template <typename T>
class shared_ptr {
    T* ptr;                 // 指向对象
    ControlBlock* ctrl;     // 指向共享的控制块
public:
    explicit shared_ptr(T* p) : ptr(p), ctrl(new ControlBlock{}) {}

    shared_ptr(const shared_ptr& o) : ptr(o.ptr), ctrl(o.ctrl) {
        if (ctrl) ++ctrl->strong;
    }

    ~shared_ptr() {
        if (ctrl && --ctrl->strong == 0) {
            delete ptr;
            if (ctrl->weak == 0) delete ctrl;
        }
    }

    long use_count() const { return ctrl ? ctrl->strong.load() : 0; }
};
```

---

## 📋 知识点清单

| # | 知识点 | 出现位置 |
|---|--------|---------|
| 1 | `struct` vs `class`（默认访问权限） | `struct ControlBlock` / `class shared_ptr` |
| 2 | `std::atomic<T>` 原子类型 | `std::atomic<long> strong` |
| 3 | 类内成员默认初始化 `{1}` | `strong{1}`、`weak{0}` |
| 4 | 类模板 `template<typename T>` | `template <typename T>` |
| 5 | 裸指针成员 | `T* ptr; ControlBlock* ctrl;` |
| 6 | `explicit` 关键字 | `explicit shared_ptr(T* p)` |
| 7 | 成员初始化列表 `: ptr(p), ctrl(...)` | 构造函数 |
| 8 | `new ControlBlock{}` 值初始化 | 构造函数 |
| 9 | 拷贝构造函数 & `const T&` | `shared_ptr(const shared_ptr& o)` |
| 10 | `++ctrl->strong`（运算符优先级 + 原子自增） | 拷贝构造 |
| 11 | 析构函数 `~shared_ptr()` | 析构 |
| 12 | 前置 `--` 返回值 + 原子递减 | `--ctrl->strong == 0` |
| 13 | `delete` 释放堆内存 | 析构 |
| 14 | `const` 成员函数 | `use_count() const` |
| 15 | `.load()` 读原子值 | `ctrl->strong.load()` |
| 16 | 三目运算符 `? :` | `use_count` |
| 17 | `->` 指针成员访问 | 各处 |

---

## 1️⃣ `struct` vs `class`

唯一区别：**默认访问权限**。`struct` 默认 `public`，`class` 默认 `private`。

- `ControlBlock` 用 `struct`：纯数据聚合体，成员要被智能指针直接访问，默认 public 最方便。
- `shared_ptr` 用 `class`：`ptr`/`ctrl` 默认 private，外部不能乱碰内部指针 —— 这是**封装**。

> 惯例：纯数据用 `struct`，有行为/需封装的用 `class`。

## 2️⃣ `std::atomic<long>` 原子类型 ⭐核心

`atomic<long>` 保证操作**不可分割**。引用计数会被多个线程同时改，普通 `++count` 底层是「读→加1→写回」三步，多线程交错会**丢失更新**，导致计数错误（提前 delete 或泄漏）。`atomic` 让 `++` 成为一条不可打断的原子操作，多线程下计数永远正确。这是标准库 `shared_ptr` 引用计数线程安全的根源。

> 代价：原子操作比普通操作慢（涉及 CPU 缓存一致性），这也是 shared_ptr 比 unique_ptr 有开销的原因之一。

## 3️⃣ 类内成员默认初始化 `{1}`

C++11 起可在类定义里直接给成员写默认值。任何 `ControlBlock` 一创建，`strong` 就自动是 1。

- **为什么初值 1**？控制块被创建时正好有第一个 shared_ptr 持有它，强引用从 1 起步。
- **为什么用 `{}` 不用 `=`**？`atomic` 不可拷贝，`strong = 1` 是拷贝语义会报错，必须用花括号**直接构造**。花括号初始化更通用、还能防窄化转换。

## 4️⃣ 类模板 `template<typename T>`

`T` 是占位符。用 `shared_ptr<int>` 时编译器**在编译期生成**一份 `T=int` 的代码。这就是泛型：一套代码适配所有类型。`ControlBlock` 不含 `T`，因为计数逻辑与被管理类型无关。

## 5️⃣ 裸指针成员

`shared_ptr` 内部就是**两个裸指针**（`sizeof ≈ 16` 字节）。所有拷贝出来的副本，它们的 `ctrl` 都指向**同一个**控制块 —— 这是「共享计数」的关键。

## 6️⃣ `explicit` 关键字

禁止隐式转换。没有它，裸指针可能被「悄悄」转成 shared_ptr，导致一个裸指针被两个 shared_ptr 各自接管 → double free。

> 经验法则：单参数构造函数，默认都加 `explicit`。

## 7️⃣ 成员初始化列表 `: ptr(p), ctrl(...)`

冒号后面这部分在**构造函数体 `{}` 执行之前**直接初始化成员。

- `: ptr(p)` 是**初始化**（一诞生就是正确值）；在 `{}` 里 `ptr = p` 是先默认构造再赋值，两步、低效。
- 对 `const` 成员、引用成员、无默认构造的成员，**只能**用初始化列表。

> ⚠️ 成员**初始化顺序取决于声明顺序**，不是初始化列表的书写顺序。

## 8️⃣ `new ControlBlock{}` 值初始化

`new` 在**堆**上分配（因为要跨作用域被多个 shared_ptr 共享），`{}` 是值初始化，确保初始化、避免未定义值。

## 9️⃣ 拷贝构造函数 & `const shared_ptr&`

当 `shared_ptr<int> b = a;` 时调用。

- `&` 传引用：避免拷贝参数本身（否则无限递归调拷贝构造）。
- `const`：承诺不修改源对象。
- 函数体：让 b 的两个指针指向和 a 相同的对象/控制块（**浅拷贝指针**），然后 `++strong`（多一个人共享）。
- `if (ctrl)`：防止源是空指针时解引用崩溃。

## 🔟 `++ctrl->strong`（优先级 + 原子自增）

`->` 优先级高于 `++`，所以等价于 `++(ctrl->strong)` —— 先找到 `strong` 再自增，**不是** `(++ctrl)->strong`。因 `strong` 是 atomic，此 `++` 是原子自增。

## 1️⃣1️⃣ 析构函数 `~shared_ptr()`

对象离开作用域时**自动调用**（RAII 核心）。每个副本销毁时把强引用 -1，并检查自己是不是最后一个。

## 1️⃣2️⃣ 前置 `--` 的返回值 + 原子递减 ⭐最精妙

```cpp
if (ctrl && --ctrl->strong == 0)
```
- **前置 `--`**：先减 1，**返回减之后的新值**；判断是否为 0 → 是则「我是最后一个」，释放对象。
- **为什么不用后置 `strong--`**？后置返回旧值，这里需要「减完后的值」，必须前置。
- **原子安全**：两个线程同时释放最后两个引用时，原子递减保证只有一个线程拿到返回值 0 → 只有一个执行 delete，不会 double free。
- `ctrl &&`：**短路求值**，ctrl 为空时右边不执行，避免空指针解引用。

## 1️⃣3️⃣ `delete` 释放堆内存（分两层）

```cpp
if (--ctrl->strong == 0) {   // 强引用归零 → 没人用对象
    delete ptr;               // 先释放对象
    if (ctrl->weak == 0)      // 且没有 weak_ptr 还引用控制块
        delete ctrl;          // 才释放控制块
}
```
为什么分两层？`weak_ptr` 不持有对象但持有控制块（靠它判断对象是否还活着）。所以**对象**在 strong=0 时释放，**控制块**要等 strong 和 weak 都为 0 才释放。

## 1️⃣4️⃣ `const` 成员函数

`use_count() const` 承诺不修改成员：编译器帮你检查，且 `const` 对象也能调用它。

## 1️⃣5️⃣ `.load()` 读原子值

读 `atomic` 用 `.load()`（原子读），保证读到完整一致的值，不会读到「改了一半」的中间状态；还可指定内存序。

## 1️⃣6️⃣ 三目运算符 `? :`

`ctrl ? ctrl->strong.load() : 0` —— 条件真取前者假取后者，这里做空指针保护。

## 1️⃣7️⃣ `->` 指针成员访问

`ctrl->strong` 等价 `(*ctrl).strong`：先解引用指针再取成员。

---

## 🧪 动手练习（务必自己做）

1. **跑一遍**两个示例，对照输出理解每一步计数变化。
2. **改坏它验证理解**：把 `ControlBlock` 的 `atomic<long>` 改成普通 `long`，重跑多线程那段，观察 `count` 不再等于 1（数据竞争）。
3. **改坏循环引用**：把 `cyclic_reference.cpp` 里 GOOD 版本的 `weak_ptr prev` 改成 `shared_ptr`，观察析构不再打印（泄漏）。
4. **扩展**：给 `SharedPtr` 加一个自定义删除器（deleter）支持。

> 完成后回到 [cpp-4-6-week-plan.md](./cpp-4-6-week-plan.md) 的 Week 1 过关自测，检查是否都能讲清。
