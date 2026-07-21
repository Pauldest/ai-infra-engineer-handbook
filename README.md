# AI Infra 工程师学习手册 🚀

> 面向大模型时代的 AI 基础设施（AI Infrastructure）工程师，一套从基础到进阶的体系化自学教材。

AI Infra 工程师负责搭建和优化支撑大模型**训练**与**推理**的底层系统，让算法团队能高效、稳定、低成本地训练和部署模型。本仓库按 6 大方向整理学习路线、核心知识点、动手项目与精选资源。

---

## 📚 学习路线总览

| 阶段 | 模块 | 目标 | 建议周期 |
|------|------|------|----------|
| 1️⃣ | [基础功底](./01-foundations/) | Python/C++ + 操作系统 + 计算机网络 | 4–8 周 |
| 2️⃣ | [PyTorch 与 Transformer](./02-pytorch-transformer/) | 精通 PyTorch，理解训练全流程 | 4–6 周 |
| 3️⃣ | [分布式训练](./03-distributed-training/) | FSDP / DeepSpeed → Megatron-LM | 4–6 周 |
| 4️⃣ | [CUDA / Triton](./04-cuda-triton/) | 算子开发与显存优化 | 4–8 周 |
| 5️⃣ | [推理与量化](./05-inference-quantization/) | vLLM 部署 + 模型量化 | 3–5 周 |
| 6️⃣ | [K8s 与集群调度](./06-kubernetes-cluster/) | 云原生工程能力 | 3–4 周 |

> 📄 完整路线图与画像见 [00-roadmap](./00-roadmap/)。
> 📖 通用书单/论文/课程见 [resources](./resources/)。

---

## 🎯 岗位职责速览

**训练侧**：搭建千卡/万卡分布式训练平台；实现并优化并行策略（DP/TP/PP/SP/MoE）；训练加速（显存、混合精度、算子融合、通信优化）；故障容错与断点续训。

**推理侧**：低延迟高吞吐的推理服务；推理框架落地（vLLM / TensorRT-LLM / SGLang）；KV Cache、连续批处理；量化/蒸馏/剪枝。

**平台侧**：GPU 集群调度（K8s / Slurm）；数据流水线、存储、监控告警、MLOps、成本与性能优化。

---

## 🗺️ 如何使用本手册

1. 按模块顺序推进，每个模块的 `README.md` 内含：知识点大纲 → 精选资源 → 动手项目 → 自测清单。
2. **重实践**：每个阶段都配了 mini-project，务必亲手写代码。
3. 遇到卡点回到 [01-foundations](./01-foundations/) 补基础。
4. 学完打勾（可 fork 本仓库当作个人进度看板）。

---

## ✅ 能力自检（学完全部后应能）

- [ ] 用 PyTorch 从零实现并训练一个 GPT，讲清前向/反向/优化器全流程
- [ ] 用 FSDP/DeepSpeed 多卡训练，并解释 ZeRO 三个阶段
- [ ] 用 Megatron 跑通 TP+PP，画出通信拓扑
- [ ] 用 Triton/CUDA 写一个融合算子并 profiling 出加速比
- [ ] 用 vLLM 部署模型，解释 PagedAttention 与 continuous batching
- [ ] 对模型做 INT8/INT4 量化并评估精度损失
- [ ] 在 K8s 上用 GPU 调度跑起训练/推理任务

---

## 🤝 贡献

欢迎 PR 补充资源、修正错误、增加习题与项目。

## 📜 License

MIT
