# 03 · 分布式训练：FSDP / DeepSpeed → Megatron-LM

> 目标：理解各种并行策略与通信原语，能用 FSDP/DeepSpeed 多卡训练，进而用 Megatron 跑通 TP+PP。

---

## 0. 前置：集合通信（Collective Communication）

必须先吃透这些原语，它们是一切分布式训练的基石：
- **AllReduce**（DP 梯度同步核心）、**AllGather**、**ReduceScatter**、**Broadcast**、**All2All**（MoE 核心）
- Ring vs Tree AllReduce 算法，通信量分析
- **NCCL**：NVIDIA 集合通信库，`NCCL_DEBUG`、拓扑感知
- 带宽/延迟、NVLink vs PCIe vs InfiniBand

🔗 资源：NCCL 官方文档、[PyTorch Distributed 文档](https://pytorch.org/tutorials/beginner/dist_overview.html)

---

## 1. 并行策略全景（核心理论）

| 并行方式 | 切分对象 | 通信 | 适用 |
|----------|----------|------|------|
| **数据并行 DP/DDP** | batch | 梯度 AllReduce | 模型放得下单卡 |
| **张量并行 TP** | 层内权重矩阵 | 层内 AllReduce | 单层太大 |
| **流水线并行 PP** | 层间（按 stage） | 激活 P2P 传递 | 层数多 |
| **序列并行 SP** | 序列维度 | 配合 TP | 长序列省激活 |
| **专家并行 EP** | MoE 专家 | All2All | MoE 模型 |
| **ZeRO/FSDP** | 参数/梯度/优化器状态分片 | AllGather+ReduceScatter | 通用省显存 |

- **3D 并行** = DP × TP × PP 组合，万卡训练标配
- 通信量与显存的权衡分析是面试高频

📄 必读论文：Megatron-LM、ZeRO、GPipe / PipeDream

---

## 2. PyTorch DDP & FSDP（从这里入门）

### 知识点
- **DDP**：原理、`DistributedSampler`、`torchrun`、梯度桶（bucket）、通信与计算重叠
- **FSDP**（Fully Sharded Data Parallel）：ZeRO-3 的 PyTorch 原生实现
  - 分片策略 `FULL_SHARD`/`SHARD_GRAD_OP`、`auto_wrap_policy`
  - CPU offload、混合精度、activation checkpointing
- FSDP2（新版）新特性

### 动手项目 ⭐
- 用 DDP 把模块 02 的 GPT 训练改成多卡
- 用 FSDP 训练更大的模型，观察显存下降

🔗 资源：[FSDP 官方教程](https://pytorch.org/tutorials/intermediate/FSDP_tutorial.html)

---

## 3. DeepSpeed（ZeRO 生态）

### 知识点
- **ZeRO 三阶段**：Stage1(优化器状态) / Stage2(+梯度) / Stage3(+参数) 分片
- **ZeRO-Offload / ZeRO-Infinity**：offload 到 CPU/NVMe
- DeepSpeed 配置文件、`deepspeed` 启动器
- 混合精度、梯度累积、activation checkpointing 集成
- DeepSpeed-Inference / MoE 支持

### 动手项目 ⭐
- 用 DeepSpeed ZeRO-2/3 训练，切换 stage 对比显存与吞吐
- 配置 ZeRO-Offload 在有限显存下训练更大模型

🔗 资源：[DeepSpeed 官方文档与示例](https://www.deepspeed.ai/)、📄 ZeRO 论文

---

## 4. Megatron-LM（进阶：TP + PP）

### 知识点
- **张量并行 TP** 的具体实现：列并行/行并行 Linear，`f`/`g` 通信算子
- **流水线并行 PP**：1F1B 调度、micro-batch、bubble 分析、interleaved schedule
- 序列并行 SP 与 TP 的配合
- 分布式 optimizer、激活重计算
- Megatron-Core 模块化设计
- 结合 DeepSpeed / Megatron-DeepSpeed

### 动手项目 ⭐
- 用 Megatron-LM 跑通 GPT 预训练，配置 TP=2, PP=2
- 画出你的 3D 并行通信拓扑图，估算通信量

🔗 资源：[Megatron-LM GitHub](https://github.com/NVIDIA/Megatron-LM)、📄 Megatron-LM 系列论文（1/2/3）

---

## 5. 进阶主题
- **通信优化**：计算通信重叠、梯度压缩、`bucket` 调优
- **容错训练**：checkpoint 保存/恢复、弹性训练（elastic）、故障自愈
- 大规模训练框架：Megatron-Core、TorchTitan、ColossalAI
- MoE 训练：专家并行、负载均衡、All2All 优化

---

## ✅ 本模块自测
- [ ] 画出 AllReduce 的 Ring 算法并算通信量
- [ ] 讲清 ZeRO 三个阶段分别分片什么、通信代价
- [ ] 解释 TP 中列并行/行并行何时需要 AllReduce
- [ ] 说明 PP 的 bubble 从何而来、1F1B 如何缓解
- [ ] 给定模型规模，设计一套 DP/TP/PP 组合方案
