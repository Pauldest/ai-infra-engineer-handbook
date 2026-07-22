# Week 2 精读：移动语义 & `unique_ptr` —— 彻底搞懂「移动」到底移动了什么

> 移动语义是很多人最模糊的地方。本文用 `unique_ptr` 作为载体，把左值/右值、右值引用、`std::move`、移动构造、`noexcept`、RVO 一次讲透。
>
> 配套可运行代码：[`code/my_unique_ptr.cpp`](./code/my_unique_ptr.cpp)

## 如何运行

```bash
cd code
clang++ -std=c++17 -O2 -Wall my_unique_ptr.cpp -o my_unique_ptr && ./my_unique_ptr
```

---

## 一、先破除最大的误解：`std::move` 不移动任何东西！

这是理解移动语义的**第一道坎**。很多人以为 `std::move(x)` 会把 x 的数据搬走 —— **错**。

> **`std::move` 只是一个类型转换**：它把一个左值「强制转换」成右值引用，**告诉编译器「这个对象我不要了，你可以来偷它」**。真正搬数据的是**移动构造函数 / 移动赋值**。

```cpp
Buffer c = std::move(a);
//         ↑ 只是把 a 标记成"可被偷"
//  ↑ 真正干活的是 Buffer 的【移动构造函数】
```

如果一个类**没有**定义移动构造函数,`std::move` 之后依然会走**拷贝**——因为没人来"偷"。所以 `std::move` 本身**不保证**任何性能提升,它只是"开了个许可"。

---

## 二、左值 vs 右值(移动语义的地基)

| 概念 | 通俗理解 | 例子 |
|------|---------|------|
| **左值 (lvalue)** | 有名字、能取地址、能持续存在 | `int x; x; a.data;` |
| **右值 (rvalue)** | 临时的、马上要消失的、不能取地址 | `42`、`x + 1`、`func()` 的返回值、`std::move(x)` |

**核心直觉**:右值是「即将死亡」的临时对象。既然它马上要销毁,我们就可以放心地「偷」它的资源,而不必费力复制。这就是移动语义的全部动机。

---

## 三、右值引用 `T&&`(移动语义的语法)

```cpp
Buffer(Buffer&& o) noexcept;   //  Buffer&& 就是右值引用
```

- `T&`(左值引用)只能绑定左值
- `T&&`(右值引用)专门绑定右值(临时对象)

编译器靠这个区分**该调拷贝还是该调移动**:
```cpp
Buffer b = a;              // a 是左值 → 匹配 Buffer(const Buffer&)  拷贝
Buffer c = std::move(a);   // move(a) 是右值 → 匹配 Buffer(Buffer&&) 移动
Buffer d = Buffer(100);    // Buffer(100) 是右值 → 移动(或被 RVO 优化掉)
```

---

## 四、移动构造:偷资源而非复制 ⭐核心

对比两个构造函数,一眼看出性能差异来源:

```cpp
// 拷贝构造:深复制底层数组 —— O(n),昂贵
Buffer(const Buffer& o) : data(o.data) { }

// 移动构造:偷走底层数组的指针,源置空 —— O(1),廉价
Buffer(Buffer&& o) noexcept : data(std::move(o.data)) { }
```

运行示例的输出直观证明:
```
--- b = a （拷贝）---
  [拷贝构造] 复制了 1000 个元素（昂贵）
--- c = std::move(a) （移动）---
  [移动构造] 偷走底层数组（O(1)），源变空
移动后 a.data.size() = 0（源被掏空）
```

**移动的本质**:把源对象内部的指针「过户」给新对象,然后把源的指针置空。数据一个字节都没动,只是换了个「主人」。这也是为什么移动后源对象变成「空壳」(size=0)——它的资源被偷走了。

> ⚠️ 移动后源对象处于「有效但未指定」状态:你不能假设它的值,但可以安全地销毁或重新赋值它。

---

## 五、`unique_ptr` 如何用移动语义实现「独占」

```cpp
template <typename T>
class UniquePtr {
    T* ptr = nullptr;                        // 内部就一个裸指针（零开销）
public:
    // 🔴 禁止拷贝 → 保证"独占"（不能两个指针管同一对象）
    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 🟢 只允许移动 → 转移所有权
    UniquePtr(UniquePtr&& o) noexcept : ptr(o.ptr) { o.ptr = nullptr; }

    ~UniquePtr() { delete ptr; }             // RAII
};
```

**设计精髓**:
- **拷贝 `= delete`**:从语言层面禁止「两个 unique_ptr 指向同一对象」,避免 double free。
- **移动保留**:所有权可以「过户」——移动后源置空,只有一个 owner。
- 于是 `unique_ptr` 天然表达「独占所有权」这个语义。

```cpp
UniquePtr<Widget> q = p;              // ❌ 编译错误:拷贝被删除
UniquePtr<Widget> q = std::move(p);   // ✅ 所有权 p → q,p 变空
```

---

## 六、`noexcept` 为什么对移动这么重要

```cpp
UniquePtr(UniquePtr&& o) noexcept : ...   // 注意这个 noexcept
```

`std::vector` 扩容时要把旧元素搬到新内存。它面临选择:用移动还是拷贝?

- 如果移动构造是 `noexcept`(保证不抛异常)→ vector **放心用移动**(快)
- 如果移动**可能抛异常** → vector 为了**异常安全**(搬到一半失败能回滚)→ **退回用拷贝**(慢!)

所以:**移动构造/赋值几乎总是应该标 `noexcept`**,否则你写了移动却享受不到性能。这是一个极易被忽略、但影响巨大的细节。

---

## 七、RVO / NRVO:比移动更强的优化

```cpp
UniquePtr<Widget> makeWidget(int id) {
    return makeUnique<Widget>(id);       // 不需要写 return std::move(...)
}
UniquePtr<Widget> w = makeWidget(2);
```

编译器的**返回值优化(RVO)**会直接把返回对象**构造在调用方的位置**,连移动都省了(零拷贝零移动)。

> 💡 常见误区:有人以为返回局部对象要写 `return std::move(local)`。**不要这样做!** 这反而会**阻止** NRVO 优化。直接 `return local;` 让编译器做最优决策。

---

## 八、完整知识点速查

| # | 知识点 | 要点 |
|---|--------|------|
| 1 | 左值 / 右值 | 有名字可取地址=左值;临时值=右值 |
| 2 | 右值引用 `T&&` | 绑定右值,是移动语义的语法基础 |
| 3 | `std::move` | **不移动!** 只是把左值转成右值引用(许可"来偷我") |
| 4 | 移动构造/赋值 | 偷指针而非复制,O(1);源置空 |
| 5 | `noexcept` 与移动 | 容器扩容仅在移动 noexcept 时才用移动 |
| 6 | `= delete` | 显式禁用拷贝,强制独占语义 |
| 7 | 独占 vs 共享 | unique_ptr 零开销独占;shared_ptr 引用计数共享 |
| 8 | RVO / NRVO | 返回局部对象时直接构造在返回位置,连移动都省 |
| 9 | 完美转发 `std::forward` | makeUnique 用它保持实参的左/右值类别 |
| 10 | 防自赋值 `this != &o` | `p = std::move(p)` 时避免先 delete 再用空指针 |

---

## 🧪 动手练习

1. **跑一遍**,重点看第 1 节拷贝(1000 元素) vs 移动(O(1))的输出差异。
2. **删掉移动构造**:把 `UniquePtr` 的移动构造/赋值注释掉,编译 `UniquePtr<Widget> q = std::move(p);` → 观察报错(没有移动就无法转移)。
3. **验证 noexcept 的作用**:给 `Buffer` 的移动构造去掉 `noexcept`,放进 `std::vector<Buffer>` 并 `push_back` 触发扩容,观察它退回调用拷贝构造。
4. **验证别写 `return std::move`**:在 `makeWidget` 里改成 `return std::move(...)`,对比是否多了一次移动(阻止了 RVO)。

> 完成后回到 [cpp-4-6-week-plan.md](./cpp-4-6-week-plan.md) 的 Week 2 过关自测。

---

## 与 shared_ptr 的联系

- `unique_ptr`:**只移动**(独占),内部 1 个指针,零开销 → 见本文
- `shared_ptr`:**可拷贝**(共享,拷贝时引用计数+1),内部 2 个指针 + 控制块 → 见 [shared_ptr-deep-dive.md](./shared_ptr-deep-dive.md)

两者对比是理解「所有权模型」的关键:**能用 unique_ptr 就别用 shared_ptr**(更省、更快、语义更清晰)。
