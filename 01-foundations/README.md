# 01 · 基础功底：Python / C++ / 操作系统 / 计算机网络

> 目标：打牢工程基础。AI Infra 的性能问题最终都会落到语言、系统、网络的底层机制上。

---

## 1. Python 精进（第一语言，必须熟练）

### 知识点
- 语言核心：数据模型、魔术方法、迭代器/生成器、装饰器、上下文管理器
- 并发：GIL、`threading` vs `multiprocessing` vs `asyncio`、`concurrent.futures`
- 性能：`cProfile`/`line_profiler`、`memory_profiler`、`numpy` 向量化
- C 扩展与绑定：`ctypes`、`cffi`、`pybind11`（连接 C++/CUDA 的关键）
- 打包与环境：`venv`/`conda`、`pip`、`pyproject.toml`、`uv`
- 工程化：类型注解 `typing`、`mypy`、`ruff`/`black`、`pytest`、`logging`

### 精选资源
- 📗《Fluent Python》(第2版) — 深入 Python 数据模型
- 📗《Effective Python》(90 条) — 工程最佳实践
- 🎥 CPython Internals（Anthony Shaw）— 理解解释器
- 🔗 [pybind11 官方文档](https://pybind11.readthedocs.io/)

### 动手项目
- 用 `pybind11` 把一段 C++ 函数封装成 Python 模块并 benchmark
- 用 `asyncio` 写一个并发爬虫/请求池

---

## 2. C++（性能关键路径 & 读框架源码必备）

### 知识点
- 现代 C++（C++11/14/17）：RAII、智能指针、移动语义、模板、STL
- 内存模型：栈/堆、对齐、缓存友好、`new/delete`、内存池
- 并发：`std::thread`、`atomic`、内存序、锁与无锁
- 构建：CMake、编译链接过程、静态/动态库
- 调试/性能：gdb、valgrind、perf、`-O2/-O3`、编译器优化

### 精选资源
- 📗《C++ Primer》(第5版) — 系统入门
- 📗《Effective Modern C++》— 现代特性
- 🔗 [cppreference.com](https://en.cppreference.com/)
- 🔗 [learncpp.com](https://www.learncpp.com/)

### 动手项目
- 用 CMake 组织一个多文件项目，写一个线程池
- 手写一个简单的内存池 / 对象池

> 📌 **专项计划**：如果你「C++ 学了很久但感觉模糊」，见 [C++「走出模糊」4–6 周学习计划](./cpp-4-6-week-plan.md) —— 每周任务 + 可运行的验证代码 + 过关自测。
>
> 📖 **精读教材**：
> - [从零实现 shared_ptr —— 17 个 C++ 知识点详解](./shared_ptr-deep-dive.md)
> - [移动语义 & unique_ptr —— 彻底搞懂「移动」到底移动了什么](./move-semantics-deep-dive.md)
>
> 配套可运行代码在 [`code/`](./code/)。

---

## 3. 操作系统（性能与资源管理的根基）

### 知识点
- 进程/线程/协程，调度，上下文切换
- 虚拟内存、分页、TLB、mmap、page cache
- 文件系统与 I/O：阻塞/非阻塞、`epoll`、零拷贝、`io_uring`
- 同步原语：互斥锁、信号量、条件变量、futex
- CPU 缓存层级、NUMA、亲和性（对多卡训练性能很重要）

### 精选资源
- 📗《Operating Systems: Three Easy Pieces》(OSTEP, 免费) — 首选
- 🎥 MIT 6.S081 / CMU 15-213 (CS:APP)
- 📗《深入理解计算机系统》(CSAPP)

### 动手项目
- 完成 CSAPP 的 Malloc Lab / Shell Lab
- 用 `epoll` 写一个简单的高并发 echo server

---

## 4. 计算机网络（分布式训练与服务的基础）

### 知识点
- TCP/IP、UDP、HTTP/2、gRPC、RPC 原理
- 高性能网络：RDMA、RoCE、InfiniBand（多机训练核心）
- 集合通信底层：为什么 AllReduce 依赖高带宽低延迟网络
- 网络拓扑：Fat-Tree、NVLink/NVSwitch、PCIe

### 精选资源
- 📗《Computer Networking: A Top-Down Approach》
- 📗《TCP/IP 详解 卷1》
- 🔗 NVIDIA NCCL / InfiniBand 文档

### 动手项目
- 用 socket 实现一个简易 RPC
- 抓包分析一次 gRPC 调用（Wireshark）

---

## ✅ 本模块自测
- [ ] 能解释 GIL 对 CPU/IO 密集任务的不同影响
- [ ] 能用 pybind11 打通 C++ ↔ Python
- [ ] 能讲清虚拟内存到物理内存的转换过程
- [ ] 能说明 RDMA 为什么比 TCP 快、为何多机训练需要它
