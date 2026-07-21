# 04 · CUDA / Triton：算子开发与显存优化

> 目标：从"用算子"到"写算子"。理解 GPU 体系结构，能用 Triton/CUDA 写融合算子并 profiling 出加速比。这是 AI Infra 深入优化的分水岭。

---

## 1. GPU 体系结构（先建立硬件心智模型）

### 知识点
- **执行模型**：Grid → Block → Warp → Thread；SM（流多处理器）、SIMT
- **内存层级**：寄存器 → shared memory → L2 → global memory（HBM），各级带宽/延迟
- **关键概念**：warp divergence、bank conflict、coalesced access（合并访存）、occupancy
- **性能分析框架**：Roofline 模型（compute-bound vs memory-bound）
- Tensor Core、FP16/BF16/FP8/INT8 计算
- CUDA stream、异步执行、事件同步

### 精选资源
- 🔗 [NVIDIA CUDA C++ Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- 📗《Programming Massively Parallel Processors》(PMPP, 第4版) — 经典教材
- 🎥 [CUDA MODE / GPU MODE 讲座系列](https://github.com/gpu-mode/lectures) — 强烈推荐

---

## 2. CUDA 编程

### 知识点
- Kernel 编写、`<<<grid, block>>>`、线程索引计算
- shared memory 使用、`__syncthreads()`、tiling 技巧
- 归约（reduction）、前缀和（scan）等并行原语模式
- 经典案例：**手写矩阵乘（GEMM）**并逐步优化（naive → tiled → 寄存器分块）
- 原子操作、warp-level 原语（`__shfl_*`）
- 编译与调试：`nvcc`、`cuda-gdb`、`compute-sanitizer`
- **性能分析**：Nsight Compute (ncu)、Nsight Systems (nsys)

### 动手项目 ⭐
- 手写向量加、矩阵转置（优化 coalescing）
- 手写 GEMM，从 naive 一步步优化，对比 cuBLAS 差距
- 手写 softmax / layernorm kernel

---

## 3. Triton（现代 GPU 算子开发首选）

### 知识点
- Triton 编程模型：block-level 编程，自动处理线程/访存细节
- `@triton.jit`、program_id、block pointer、mask
- **写融合算子**：fused softmax、layernorm、dropout+bias
- **手写 FlashAttention**（Triton 版，理解 IO 感知优化）
- autotuning、`triton.Config`
- Triton 与 `torch.compile`（Inductor 后端生成 Triton）

### 精选资源
- 🔗 [Triton 官方教程](https://triton-lang.org/main/getting-started/tutorials/index.html) — 从 vector add 到 FlashAttention
- 🎥 GPU MODE 的 Triton 专题
- 🔗 [Unsloth / FlashAttention Triton 实现参考]

### 动手项目 ⭐
- 跟着 Triton 官方教程实现 fused softmax 并 benchmark vs PyTorch
- 用 Triton 实现一个融合的 LayerNorm，集成进 PyTorch 模型

---

## 4. 显存优化专题（AI Infra 核心技能）

### 知识点
- 显存构成分析：权重/梯度/优化器状态/激活值/临时 buffer/碎片
- **算子融合**（fusion）减少中间张量与访存
- **激活重计算**（activation/gradient checkpointing）
- **FlashAttention**：省去 N×N 注意力矩阵的显存
- PyTorch caching allocator、内存碎片、`expandable_segments`
- offload（CPU/NVMe）、量化存储
- 用 `torch.cuda.memory_summary()` / memory snapshot 分析

### 动手项目 ⭐
- 用 memory snapshot 可视化训练显存，定位峰值
- 对比开启/关闭 gradient checkpointing 的显存与速度

---

## 5. 进阶：编译器与图优化
- `torch.compile`（Dynamo + Inductor）工作原理
- 算子融合、图捕获、CUDA graphs
- 了解 TVM / XLA / MLIR / CUTLASS（进阶方向）

---

## ✅ 本模块自测
- [ ] 解释 coalesced access 与 bank conflict，如何避免
- [ ] 用 Roofline 判断一个算子是 compute-bound 还是 memory-bound
- [ ] 手写一个 tiled GEMM 并说明 shared memory 的作用
- [ ] 用 Triton 写一个融合算子并给出加速比
- [ ] 讲清 FlashAttention 为何能同时省显存又加速
