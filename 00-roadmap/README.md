# 00 · 总路线图与岗位画像

## 一、AI Infra 工程师画像

AI Infra（AI Infrastructure / 大模型系统）工程师处在 **算法 ↔ 硬件** 的中间层，核心价值是让训练与推理 **更快、更省、更稳**。

```
算法研究员 ── 需要算力/框架 ──▶  【AI Infra 工程师】 ──▶ GPU 集群 / 硬件
                                   |
              训练平台 · 并行策略 · 算子 · 推理服务 · 调度平台
```

### 常见细分方向
| 方向 | 关注点 | 关键技能 |
|------|--------|----------|
| 训练框架/加速 | 分布式训练、并行策略、显存/通信优化 | Megatron、DeepSpeed、NCCL、CUDA |
| 推理引擎 | 低延迟高吞吐、量化、KV Cache | vLLM、TensorRT-LLM、SGLang |
| 算子/编译 | Kernel 开发、图优化、编译器 | CUDA、Triton、TVM、MLIR |
| 平台/调度 | 集群管理、资源调度、MLOps | K8s、Slurm、Ray、监控 |
| 数据基础设施 | 大规模数据 pipeline、存储 | Spark、Parquet、对象存储 |

---

## 二、推荐学习顺序（依赖关系）

```
01 基础功底 (Python/C++/OS/网络)
        │
        ▼
02 PyTorch + Transformer + 训练全流程
        │
        ├──────────────┬───────────────┐
        ▼              ▼               ▼
03 分布式训练      04 CUDA/Triton   05 推理与量化
 (FSDP→DS→Megatron) (算子/显存)      (vLLM+量化)
        │              │               │
        └──────────────┴───────────────┘
                       ▼
              06 K8s / 集群调度 (工程落地)
```

- **02 是核心枢纽**，务必扎实。
- 03/04/05 可根据兴趣并行推进，但都以 02 为前置。
- 04（CUDA/Triton）是分水岭，决定你能否深入优化。
- 06 贯穿始终，可边学边补。

---

## 三、时间规划参考（每周 10–15h）

| 周次 | 内容 |
|------|------|
| W1–6 | 基础功底 + Python 精进，OS/网络补齐 |
| W7–12 | PyTorch 深入 + 手写 Transformer + 训练 GPT |
| W13–18 | FSDP/DeepSpeed 多卡训练，读 ZeRO 论文 |
| W19–22 | Megatron TP/PP，通信优化 |
| W23–30 | CUDA 基础 + Triton 融合算子 + profiling |
| W31–35 | vLLM 部署 + 量化实战 |
| W36–39 | K8s + GPU 调度 + 监控 |

---

## 四、面试考察重点（自检）
- 分布式并行：DP/TP/PP/SP/EP 原理与通信量分析
- 显存构成：参数/梯度/优化器状态/激活值，如何降显存
- 混合精度 & 数值稳定性（fp16/bf16/fp8、loss scaling）
- 通信原语：AllReduce/AllGather/ReduceScatter 与 Ring/Tree 算法
- 推理优化：KV Cache、PagedAttention、continuous batching、投机解码
- 量化：对称/非对称、per-tensor/per-channel、GPTQ/AWQ/SmoothQuant
- 系统：GPU 架构、SM/warp、内存层级、Roofline 模型

---

## 五、心态建议
- **动手 > 读书**：每个概念都要能跑出代码验证。
- **读源码**：PyTorch / vLLM / Megatron 源码是最好的教材。
- **读论文 + 复现**：从复现小实验开始建立直觉。
- 建个人博客/笔记，输出倒逼输入。
