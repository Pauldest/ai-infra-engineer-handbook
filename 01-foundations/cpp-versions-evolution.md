# C++ 版本演进 × 现代特性全景图

> 把 RAII、智能指针、移动语义、atomic 等散落的知识点,按「哪个版本引入」串成一条时间线。抓住这条线,现代 C++ 的整体脉络就清晰了。

---

## 一、总览时间线

```
C++98/03 ──────► C++11 ──────► C++14 ──► C++17 ──► C++20 ──────► C++23
(classic)      (现代C++起点)   (完善)    (完善)   (又一大跃迁)
   │               │
   │               └── 智能指针 / 移动语义 / atomic / thread / lambda / auto ...
   │
   └── RAII 地基 / 构造析构 / 模板 / STL / 异常
```

> 💡 常说「C++11 之前和之后是两种语言」——因为现代 C++ 的核心特性几乎全在 C++11 引入。

---

## 二、各版本核心特性

### C++98 / C++03（classic C++，1998/2003）
奠定语言地基,**但完全没有多线程概念**(标准假设单线程)。
- ✅ 类、封装、继承、多态、虚函数
- ✅ **模板**、泛型、STL(vector/map/algorithm)
- ✅ 异常处理
- ✅ **RAII 的地基**:构造函数/析构函数、确定性析构、栈展开
- ✅ `std::auto_ptr`(有缺陷的早期智能指针,C++17 已移除)
- ❌ 没有:多线程、移动语义、lambda、auto、智能指针(现代版)

### C++11（2011）🎉 现代 C++ 的起点，最重要的一版
| 特性 | 说明 | 本仓库对应讲解 |
|------|------|--------------|
| **移动语义 / 右值引用 `T&&`** | 偷资源而非复制,性能革命 | [move-semantics-deep-dive.md](./move-semantics-deep-dive.md) |
| **`unique_ptr` / `shared_ptr` / `weak_ptr`** | 现代智能指针,自动内存管理 | [shared_ptr-deep-dive.md](./shared_ptr-deep-dive.md) |
| **`std::atomic` + 内存模型** | 首个标准多线程内存模型、`memory_order` | [README](./README.md#4-计算机网络分布式训练与服务的基础) 及 handbook |
| **`std::thread` / `mutex` / `condition_variable`** | 标准多线程库 | — |
| **lambda 表达式** | `[](){}` 匿名函数 | — |
| **`auto` / `decltype`** | 类型推导 | — |
| **范围 for** | `for (auto x : v)` | — |
| **`nullptr`** | 取代 `NULL` | — |
| **列表初始化 `{}`** | 统一初始化语法 | — |
| **类内成员默认初始化** | `int x{0};` 写在类定义里 | shared_ptr 精读 |
| **`= default` / `= delete`** | 显式控制特殊成员函数 | unique_ptr 精读 |
| **可变参数模板** | `template<typename... Args>` | — |
| **`constexpr`** | 编译期常量表达式 | — |
| **强枚举 `enum class`** | 作用域枚举 | — |

### C++14（2014）— 补丁与完善
- **`make_unique`**(C++11 漏掉了,这版补上)
- 泛型 lambda:`[](auto x){}`
- 函数返回类型推导 `auto f() { ... }`
- 变量模板、`std::shared_lock`

### C++17（2017）— 实用性增强
- **结构化绑定**:`auto [a, b] = pair;`
- **`std::optional` / `variant` / `any`**(都基于 RAII)
- **`if constexpr`**(编译期分支,写模板神器)
- **`std::string_view`**(零拷贝字符串视图)
- **`scoped_lock`**(多锁 RAII)、`is_always_lock_free`
- 折叠表达式、文件系统库 `<filesystem>`
- 移除 `std::auto_ptr`

### C++20（2020）🚀 又一次大跃迁
- **Concepts**:给模板加约束,报错更友好
- **Ranges**:`views::filter | transform` 管道式操作
- **Coroutines**:`co_await`/`co_yield` 协程
- **Modules**:取代头文件的模块系统
- **`std::atomic` 增强**:`atomic_ref`、`atomic<shared_ptr>`、`wait/notify`、浮点特化
- `std::span`、三路比较 `<=>`(太空船运算符)、`std::format`

### C++23（2023）— 持续打磨
- `std::expected`(错误处理)、`std::mdspan`(多维视图)
- `std::print`、ranges 增强、栈跟踪 `<stacktrace>`

---

## 三、按主题看「哪个版本引入」

### 资源管理 / 智能指针
| 特性 | 版本 |
|------|------|
| RAII 地基(构造/析构/确定性析构) | C++98 |
| `auto_ptr`(已废弃) | C++98 |
| `unique_ptr` / `shared_ptr` / `weak_ptr` | **C++11** |
| `make_shared` | C++11 |
| `make_unique` | C++14 |
| `atomic<shared_ptr>` | C++20 |

### 移动语义
| 特性 | 版本 |
|------|------|
| 右值引用 `T&&` / 移动构造 / `std::move` / `std::forward` | **C++11** |
| 保证的拷贝消除(guaranteed copy elision) | C++17 |

### 并发 / 原子
| 特性 | 版本 |
|------|------|
| 语言层多线程(此前**完全没有**) | **C++11** |
| `std::atomic` / 内存模型 / `memory_order` | **C++11** |
| `std::thread` / `mutex` / `condition_variable` / `future` | **C++11** |
| `shared_mutex`(读写锁) | C++17 |
| `atomic_ref` / `atomic::wait/notify` / `jthread` / 协程 | C++20 |

---

## 四、给学习者的建议

1. **默认用 C++17**:`clang++ -std=c++17`。它稳定、特性够用,是当前工程主流。
2. **重点吃透 C++11**:智能指针、移动语义、atomic、lambda、auto —— 这些是「现代 C++」的地基,面试与源码高频。
3. **C++20 按需学**:Concepts/Ranges/Coroutines 很强大,但先把 C++11/14/17 打牢再上。
4. **记住这条主线**:
   > RAII(C++98 地基)→ C++11 用移动语义 + 智能指针 + atomic 把它武装成「现代 C++」→ C++17 增强实用性 → C++20 再跃迁。

## 一句话总结
> **C++98 打地基(RAII/模板/STL,但没有多线程);C++11 是分水岭,一口气引入智能指针、移动语义、atomic、多线程、lambda、auto,开启「现代 C++」;之后每三年一版持续增强。你最近学的 RAII、智能指针、移动语义、atomic —— 除 RAII 地基外,几乎全是 C++11 的产物。**
