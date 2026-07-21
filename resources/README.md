# 资源汇总：书单 / 论文 / 课程 / 工具

## 📗 必读书单
| 书 | 方向 |
|----|------|
| 深入理解计算机系统 (CSAPP) | 系统基础 |
| Operating Systems: Three Easy Pieces (OSTEP, 免费) | 操作系统 |
| Fluent Python / Effective Python | Python |
| C++ Primer / Effective Modern C++ | C++ |
| Programming Massively Parallel Processors (PMPP) | CUDA/GPU |
| Kubernetes in Action | 云原生 |
| Designing Data-Intensive Applications (DDIA) | 分布式系统 |

## 📄 核心论文清单
**Transformer / 架构**
- Attention Is All You Need
- LLaMA / Mixtral (MoE)
- FlashAttention 1 & 2

**分布式训练**
- Megatron-LM (1/2/3)
- ZeRO / ZeRO-Offload / ZeRO-Infinity
- GPipe / PipeDream (1F1B)
- Reducing Activation Recomputation (Sequence Parallelism)

**推理 / 量化**
- Efficient Memory Management for LLM Serving (vLLM / PagedAttention)
- Orca (continuous batching)
- GPTQ / AWQ / SmoothQuant / LLM.int8() / QLoRA
- Speculative Decoding / Medusa / EAGLE

## 🎥 精选课程 / 视频
- Andrej Karpathy —《Neural Networks: Zero to Hero》(手写 GPT)
- GPU MODE (CUDA MODE) lectures — CUDA/Triton 实战
- CMU 15-213 CSAPP / MIT 6.S081 — 系统基础
- Stanford CS336 — Language Modeling from Scratch
- MIT 6.5940 (Han Lab) — TinyML & Efficient Deep Learning（量化/剪枝/蒸馏）

## 🛠️ 关键开源项目（读源码）
| 项目 | 学什么 |
|------|--------|
| [nanoGPT](https://github.com/karpathy/nanoGPT) | 训练全流程 |
| [PyTorch](https://github.com/pytorch/pytorch) | 框架内部机制 |
| [DeepSpeed](https://github.com/microsoft/DeepSpeed) | ZeRO |
| [Megatron-LM](https://github.com/NVIDIA/Megatron-LM) | TP/PP |
| [TorchTitan](https://github.com/pytorch/torchtitan) | 现代分布式训练范式 |
| [vLLM](https://github.com/vllm-project/vllm) | 推理引擎 |
| [Triton](https://github.com/triton-lang/triton) | 算子开发 |
| [FlashAttention](https://github.com/Dao-AILab/flash-attention) | 融合算子 |

## 🌐 社区 / 持续跟进
- GPU MODE Discord
- HuggingFace 博客
- 各框架 GitHub Issues / Discussions
- arXiv（cs.DC / cs.LG）、Papers with Code
- 公众号/博客：跟踪 vLLM、SGLang、DeepSpeed 更新

## 💡 学习方法建议
1. **复现优先**：读论文后尽量复现最小实验。
2. **读源码**：带着问题读，画调用/数据流图。
3. **做 benchmark**：任何优化都要有前后对比数据。
4. **写输出**：博客/笔记沉淀，倒逼理解。
5. **参与开源**：从修文档/小 bug 开始给主流项目提 PR。
