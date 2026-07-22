# C++「走出模糊」4–6 周学习计划

> 面向：语法学过但理解模糊、想达到 AI Infra「读框架源码 + 写算子绑定」要求（L2 及格线 → L3 有竞争力）的人。
>
> **核心方法**：拒绝被动通读。每个概念都要 ① 写最小验证程序看到现象 ② 用自己的话讲清原理。**看到 = 记住,讲清 = 掌握。**

---

## 使用说明
- 每周 8–12 小时,共 6 周(基础好可压缩到 4 周,跳过已掌握部分)。
- 每个概念配了「验证代码」:亲手写、编译、运行、观察输出。
- 每周末做「过关自测」,答不清就回去补。
- 编译命令统一:`g++ -std=c++17 -O2 -Wall xxx.cpp -o xxx && ./xxx`

---

## 📅 Week 1 — RAII 与智能指针(最高优先级)

**为什么先学**:这是现代 C++ 的灵魂,也是读框架源码时出现频率最高的东西。

### 核心概念
- RAII:资源获取即初始化,用对象生命周期管理资源(内存/文件/锁)
- `unique_ptr`:独占所有权,不可拷贝只可移动,零开销
- `shared_ptr`:引用计数共享,`use_count()`,线程安全的计数
- `weak_ptr`:打破循环引用
- 什么时候用裸指针(不拥有所有权时)

### 验证代码 ①:亲眼看到 RAII 自动释放
```cpp
#include <iostream>
#include <memory>

struct Resource {
    Resource()  { std::cout << "  [构造] 获取资源\n"; }
    ~Resource() { std::cout << "  [析构] 释放资源\n"; }
    void use()  { std::cout << "  使用资源\n"; }
};

int main() {
    std::cout << "进入作用域\n";
    {
        auto r = std::make_unique<Resource>();
        r->use();
    } // 离开作用域,unique_ptr 自动析构 —— 无需手动 delete
    std::cout << "离开作用域(资源已自动释放)\n";
}
```
👀 **观察**:析构在离开 `{}` 时自动发生。这就是 RAII —— 不会忘记释放、异常安全。

### 验证代码 ②:shared_ptr 引用计数
```cpp
#include <iostream>
#include <memory>

int main() {
    auto a = std::make_shared<int>(42);
    std::cout << "count=" << a.use_count() << "\n";   // 1
    {
        auto b = a;                                    // 拷贝 -> 计数+1
        std::cout << "count=" << a.use_count() << "\n"; // 2
    }                                                  // b 析构 -> 计数-1
    std::cout << "count=" << a.use_count() << "\n";    // 1
}
```

### 验证代码 ③:循环引用与 weak_ptr(重要陷阱)
```cpp
#include <iostream>
#include <memory>
struct Node {
    std::shared_ptr<Node> next;
    // std::weak_ptr<Node> prev;  // 试试改成 shared_ptr 会怎样?
    std::weak_ptr<Node> prev;
    ~Node() { std::cout << "Node 析构\n"; }
};
int main() {
    auto a = std::make_shared<Node>();
    auto b = std::make_shared<Node>();
    a->next = b;
    b->prev = a;   // 若 prev 是 shared_ptr,两者互相引用 -> 永不析构(内存泄漏!)
}
```
👀 **实验**:把 `prev` 改成 `shared_ptr`,你会发现"Node 析构"不再打印 —— 这就是循环引用泄漏,weak_ptr 用来打破它。

### ✅ 过关自测
- [ ] `unique_ptr` 和 `shared_ptr` 底层各如何管理生命周期?
- [ ] 为什么 `unique_ptr` 不能拷贝只能移动?
- [ ] 循环引用为什么泄漏?weak_ptr 如何解决?
- [ ] 什么场景该用裸指针?

---

## 📅 Week 2 — 移动语义与右值引用

**为什么重要**:高性能代码的关键(避免不必要的拷贝),也是很多人最模糊的地方。

### 核心概念
- 左值 vs 右值,右值引用 `T&&`
- 拷贝构造 vs 移动构造:移动是"偷"资源而非复制
- `std::move`:它**不移动任何东西**,只是把左值转成右值引用(一个类型转换)
- 移动赋值、`noexcept` 与移动、返回值优化(RVO/NRVO)

### 验证代码:亲眼区分拷贝与移动
```cpp
#include <iostream>
#include <vector>
#include <utility>

struct Buffer {
    std::vector<int> data;
    Buffer(size_t n) : data(n) { std::cout << "构造 " << n << "\n"; }
    Buffer(const Buffer& o) : data(o.data)            { std::cout << "拷贝构造(复制 " << data.size() << " 个元素)\n"; }
    Buffer(Buffer&& o) noexcept : data(std::move(o.data)) { std::cout << "移动构造(偷取,原对象变空)\n"; }
};

int main() {
    Buffer a(1000);
    std::cout << "--- b = a (拷贝) ---\n";
    Buffer b = a;                 // 触发拷贝构造:真的复制 1000 个元素
    std::cout << "--- c = move(a) (移动) ---\n";
    Buffer c = std::move(a);      // 触发移动构造:只偷指针,O(1)
    std::cout << "move 后 a.data.size() = " << a.data.size() << "\n"; // 0
}
```
👀 **观察**:拷贝要复制 1000 个元素,移动只是"偷走"底层指针(O(1)),移动后源对象变空。理解这个,你就懂了为什么 `std::vector` 扩容、函数返回大对象时移动能大幅提速。

### ✅ 过关自测
- [ ] `std::move` 到底做了什么?(它不搬数据!)
- [ ] 移动构造和拷贝构造的性能差异来自哪?
- [ ] 为什么移动构造最好标 `noexcept`?
- [ ] 什么是 RVO?

---

## 📅 Week 3 — 模板与泛型编程(读源码必备)

**为什么重要**:PyTorch/STL 大量用模板,读源码绕不开。

### 核心概念
- 函数模板、类模板、模板参数推导
- 编译期实例化(模板是"代码生成器")
- `auto`、`decltype`、尾置返回类型
- 可变参数模板(variadic template,`...`)入门
- 特化与偏特化(读懂即可)
- C++17/20:`if constexpr`、concepts(了解)

### 验证代码:模板实例化 + 编译期
```cpp
#include <iostream>
#include <type_traits>

template <typename T>
T add(T a, T b) {
    // 编译期分支:整型与浮点走不同逻辑
    if constexpr (std::is_integral_v<T>)
        std::cout << "[整型版本] ";
    else
        std::cout << "[浮点版本] ";
    return a + b;
}

int main() {
    std::cout << add(1, 2) << "\n";       // 编译器生成 add<int>
    std::cout << add(1.5, 2.5) << "\n";   // 编译器生成 add<double>
}
```
👀 **理解**:模板不是运行时多态,而是**编译期为每种类型生成一份代码**。这也是模板报错难读的原因。

### 建议
- 读一小段 STL 源码,如 `std::vector` 的 `push_back`/扩容逻辑。
- 不用一次学透元编程(TMP),能"读懂"框架模板即可。

### ✅ 过关自测
- [ ] 模板在编译期还是运行期实例化?对二进制大小有何影响?
- [ ] 模板 vs 虚函数多态,各自开销在哪?
- [ ] `if constexpr` 和普通 `if` 区别?

---

## 📅 Week 4 — 内存模型与性能直觉

**为什么重要**:AI Infra 的价值就是性能,必须建立"数据在内存里如何流动"的直觉。

### 核心概念
- 栈 vs 堆:分配速度、生命周期、大小限制
- 对象内存布局:成员排列、对齐(alignment)、padding
- 缓存层级:L1/L2/L3/主存的速度差(数量级!)
- **缓存局部性**:为什么连续访问快、随机访问慢
- `new/delete` 开销、内存池思想

### 验证代码:缓存局部性(行优先 vs 列优先)
```cpp
#include <iostream>
#include <vector>
#include <chrono>
using namespace std::chrono;

int main() {
    const int N = 4000;
    std::vector<std::vector<int>> m(N, std::vector<int>(N, 1));
    long long sum = 0;

    auto t1 = high_resolution_clock::now();
    for (int i = 0; i < N; ++i)          // 行优先:内存连续,缓存友好
        for (int j = 0; j < N; ++j) sum += m[i][j];
    auto t2 = high_resolution_clock::now();
    for (int j = 0; j < N; ++j)          // 列优先:跳跃访问,频繁 cache miss
        for (int i = 0; i < N; ++i) sum += m[i][j];
    auto t3 = high_resolution_clock::now();

    std::cout << "行优先: " << duration_cast<milliseconds>(t2-t1).count() << " ms\n";
    std::cout << "列优先: " << duration_cast<milliseconds>(t3-t2).count() << " ms\n";
}
```
👀 **观察**:同样的计算量,列优先常慢好几倍。这就是缓存局部性 —— 理解它,你才懂为什么算子优化要关注访存模式(直接关联模块 04 CUDA)。

### ✅ 过关自测
- [ ] 栈和堆分配的区别与开销?
- [ ] 为什么行优先遍历比列优先快?(cache line)
- [ ] 什么是内存对齐?为什么需要?

---

## 📅 Week 5 — 并发编程

**为什么重要**:推理引擎、数据 pipeline 都涉及多线程。

### 核心概念
- `std::thread`、`join`/`detach`
- 数据竞争(data race)与 `std::mutex`、`lock_guard`
- `std::atomic`、原子操作 vs 锁
- 内存序(memory order)入门:relaxed/acquire/release(理解概念即可)
- 条件变量 `condition_variable`、线程池思想

### 验证代码:数据竞争 → 原子修复
```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

int main() {
    // int counter = 0;                 // 试试非原子:结果会小于 400000(数据竞争)
    std::atomic<int> counter{0};        // 原子:结果稳定正确
    auto work = [&]{ for (int i = 0; i < 100000; ++i) ++counter; };
    std::vector<std::thread> ts;
    for (int i = 0; i < 4; ++i) ts.emplace_back(work);
    for (auto& t : ts) t.join();
    std::cout << "counter = " << counter << " (期望 400000)\n";
}
```
👀 **实验**:把 `counter` 改成普通 `int`,多跑几次,结果会小于 400000 且每次不同 —— 这就是数据竞争。原子操作或加锁可修复。

### ✅ 过关自测
- [ ] 什么是数据竞争?如何避免?
- [ ] `atomic` 和 `mutex` 分别适合什么场景?
- [ ] 为什么需要内存序?relaxed 和 seq_cst 区别(概念)?

---

## 📅 Week 6 — 工程化 + 连接 Python(收尾,最实用)

**为什么重要**:AI Infra 的 C++ 最终要**服务 Python 框架**,这一周把能力落地。

### 核心概念
- **CMake**:组织多文件项目、链接库、`target_link_libraries`
- 编译链接过程:预处理 → 编译 → 汇编 → 链接;静态库 vs 动态库
- 调试与性能:`gdb`、`valgrind`(内存泄漏)、`perf`(热点)
- **pybind11**:把 C++ 函数/类暴露给 Python(连接算子与框架的关键)

### 动手项目 ⭐(本计划的终极验证)
用 pybind11 把一个 C++ 函数封装成 Python 模块:

```cpp
// example.cpp
#include <pybind11/pybind11.h>
int add(int a, int b) { return a + b; }
PYBIND11_MODULE(example, m) {
    m.def("add", &add, "A function that adds two numbers");
}
```
```bash
pip install pybind11
c++ -O3 -Wall -shared -std=c++17 -fPIC \
    $(python3 -m pybind11 --includes) \
    example.cpp -o example$(python3-config --extension-suffix)
python3 -c "import example; print(example.add(3, 4))"   # 输出 7
```
👀 **意义**:你刚刚打通了 C++ ↔ Python 的桥 —— 这正是自定义 CUDA 算子集成进 PyTorch 的原理。**做到这一步,你的 C++ 已达到 AI Infra 的实用要求。**

### ✅ 过关自测
- [ ] 静态库和动态库的区别?
- [ ] 用 CMake 组织一个多文件项目并成功编译
- [ ] 独立完成一次 pybind11 封装并在 Python 调用

---

## 🎓 结业标准

完成后你应该能:
- [ ] 无障碍读懂框架里的现代 C++ 代码(智能指针/模板/移动)
- [ ] 清楚讲出前面 8 道自测题(不再模糊)
- [ ] 用 pybind11 打通 C++ 与 Python
- [ ] 对性能有直觉(缓存局部性、拷贝 vs 移动、栈 vs 堆)

达到以上 = **AI Infra 的 C++ 要求已达标(扎实 L2,摸到 L3)**。之后无需刷更多语法,而是在 CUDA/推理引擎的**真实场景**中继续深化。

## 📚 配套资源
- 📗《Effective Modern C++》— 配合每周主题精读对应条款
- 🔗 learncpp.com — 查漏补缺,原理清晰
- 🔗 cppreference.com — 权威 API 参考
- 🔗 [pybind11 文档](https://pybind11.readthedocs.io/)
- 🎥 搜 "C++ object model / move semantics" 类可视化讲解

## ⚠️ 三个避坑提醒
1. **别再"通读教程"**:你已经模糊了很久,问题是缺输出不是缺输入。每个概念必须写代码验证。
2. **别追求学全**:模板元编程、无锁等 L4 内容现在不用碰,达标即可推进到 CUDA。
3. **带真实目标学**:每周末尝试读一小段 PyTorch/框架的 C++ 源码,用它检验本周所学。
