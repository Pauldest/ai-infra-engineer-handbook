# 代码示例

配套讲解见上一级目录的 markdown 文档。

| 文件 | 主题 | 讲解文档 |
|------|------|---------|
| `my_shared_ptr.cpp` | 从零实现 shared_ptr/weak_ptr（引用计数、RAII、atomic、拷贝/移动语义） | [../shared_ptr-deep-dive.md](../shared_ptr-deep-dive.md) |
| `cyclic_reference.cpp` | 循环引用泄漏 vs weak_ptr 打破环 | 同上 |

## 编译运行

```bash
clang++ -std=c++17 -O2 -Wall -pthread my_shared_ptr.cpp -o my_shared_ptr && ./my_shared_ptr
clang++ -std=c++17 -O2 -Wall cyclic_reference.cpp -o cyclic && ./cyclic
```

（macOS/Linux 用 `clang++` 或 `g++` 均可。）
