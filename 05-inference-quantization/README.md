# 05 · 推理与量化：vLLM 部署 + 模型压缩

> 目标：掌握一个主流推理框架（vLLM），理解推理优化核心技术，能对模型做量化并评估精度损失。

---

## 1. LLM 推理基础

### 知识点
- **两阶段**：Prefill（并行处理 prompt，compute-bound）vs Decode（逐 token 生成，memory-bound）
- **KV Cache**：原理、显存占用计算、为何是推理瓶颈
- 采样策略：greedy、top-k、top-p、temperature、beam search
- 评价指标：**TTFT**（首 token 延迟）、**TPOT/ITL**（token 间延迟）、**吞吐（tokens/s）**、并发
- 显存 vs 吞吐 vs 延迟的权衡

### 精选资源
- 🔗 [HuggingFace: How to generate](https://huggingface.co/blog/how-to-generate)
- 📄 相关 survey：Efficient LLM Inference

---

## 2. vLLM（推荐主攻）

### 知识点
- **PagedAttention**：像 OS 分页一样管理 KV Cache，减少碎片 ⭐核心
- **Continuous batching**（连续批处理 / in-flight batching）：动态拼 batch，提高利用率
- **Prefix caching**：共享前缀 KV 复用
- **Chunked prefill**、调度器原理
- 部署：OpenAI 兼容 API server、张量并行、量化支持
- 分布式推理：TP、PP、多机
- **投机解码**（speculative decoding）、Medusa、EAGLE

### 精选资源
- 🔗 [vLLM 官方文档](https://docs.vllm.ai/) + [GitHub](https://github.com/vllm-project/vllm)
- 📄 vLLM (PagedAttention) 论文
- 🔗 阅读 vLLM 源码：scheduler、block manager、attention backend

### 动手项目 ⭐
- 用 vLLM 起一个 OpenAI 兼容服务并压测吞吐/延迟
- 开启/关闭 continuous batching、prefix caching，对比性能
- 用张量并行部署一个较大模型

---

## 3. 其他推理引擎（了解对比）
- **TensorRT-LLM**：NVIDIA 官方，极致性能，编译式
- **SGLang**：RadixAttention，结构化生成强
- **llama.cpp / GGUF**：CPU/边缘/量化推理
- **TGI**（HuggingFace）、**LMDeploy**
- 对比维度：性能、易用性、量化支持、生态

---

## 4. 量化（Quantization）核心技能

### 知识点
- **基础**：对称/非对称、per-tensor / per-channel / per-group、校准（calibration）
- **PTQ（训练后量化）**：
  - **GPTQ**：基于二阶信息逐层量化，INT4
  - **AWQ**：激活感知权重量化，保护重要通道
  - **SmoothQuant**：平滑激活异常值，W8A8
  - **INT8 / FP8 / INT4** 权重量化
- **QAT（量化感知训练）**、**QLoRA**（4bit + LoRA 微调）
- KV Cache 量化
- 精度评估：PPL、下游任务准确率对比

### 精选资源
- 📄 论文：GPTQ、AWQ、SmoothQuant、LLM.int8()、QLoRA
- 🔗 [AutoGPTQ](https://github.com/AutoGPTQ/AutoGPTQ)、[AutoAWQ](https://github.com/casper-hansen/AutoAWQ)、[llm-awq](https://github.com/mit-han-lab/llm-awq)

### 动手项目 ⭐
- 用 GPTQ/AWQ 把一个模型量化到 INT4，评估 PPL 与显存/速度变化
- 在 vLLM 中加载量化模型部署，对比 FP16 吞吐

---

## 5. 其他压缩技术（了解）
- **蒸馏**（Knowledge Distillation）
- **剪枝**（Pruning）：结构化/非结构化、2:4 稀疏
- MoE 稀疏化

---

## ✅ 本模块自测
- [ ] 解释 prefill 与 decode 的计算特性差异
- [ ] 计算给定模型/序列长度下的 KV Cache 显存
- [ ] 讲清 PagedAttention 解决了什么问题
- [ ] 说明 continuous batching 为何提升吞吐
- [ ] 对比 GPTQ 与 AWQ 的核心思想
- [ ] 部署一个量化模型并报告精度/性能权衡
