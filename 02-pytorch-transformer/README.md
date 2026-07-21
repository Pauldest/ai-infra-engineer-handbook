# 02 · PyTorch 与 Transformer：精通框架，理解训练全流程

> 目标：从"会调 API"到"理解内部机制"，能从零实现并训练一个 GPT，讲清训练全流程。这是 AI Infra 的核心枢纽模块。

---

## 1. PyTorch 深入

### 知识点
- **Tensor 与自动微分**：`autograd` 机制、计算图（动态图）、`backward()` 原理、`grad_fn`
- **nn.Module**：参数管理、`state_dict`、`buffer`、hook
- **数据管道**：`Dataset`/`DataLoader`、`num_workers`、`pin_memory`、采样器
- **优化器与调度器**：SGD/Adam/AdamW 原理，学习率 warmup/cosine
- **混合精度**：`torch.cuda.amp`、`autocast`、`GradScaler`、fp16 vs bf16
- **性能**：`torch.compile`（TorchDynamo/Inductor）、CUDA graphs、`torch.profiler`
- **内部机制**：dispatcher、ATen、CUDA stream、内存缓存分配器（caching allocator）
- **保存/加载/checkpoint**：`torch.save`、`gradient checkpointing`

### 精选资源
- 🔗 [PyTorch 官方教程](https://pytorch.org/tutorials/) — 首选
- 🎥 Andrej Karpathy《Neural Networks: Zero to Hero》— 必看
- 🔗 [PyTorch internals (ezyang 博客)](http://blog.ezyang.com/2019/05/pytorch-internals/)
- 📗《Deep Learning with PyTorch》

### 动手项目
- 用纯 `autograd` 实现一个两层 MLP 的反向传播（不用 `nn`）
- 用 `torch.profiler` 找出训练瓶颈并优化

---

## 2. Transformer 架构（吃透原理）

### 知识点
- **Attention**：Scaled Dot-Product、Multi-Head、causal mask
- **位置编码**：绝对位置、RoPE、ALiBi
- **结构组件**：LayerNorm/RMSNorm、残差连接、FFN、GLU/SwiGLU
- **现代 LLM 架构**：GPT / LLaMA / Mixtral(MoE) 的差异
- **KV Cache**：推理时的关键（承接模块 05）
- **注意力优化**：FlashAttention 原理（IO 感知、分块 softmax）
- **Tokenizer**：BPE、SentencePiece

### 精选资源
- 📄 论文《Attention Is All You Need》
- 🔗 [The Illustrated Transformer](https://jalammar.github.io/illustrated-transformer/)
- 🔗 [The Annotated Transformer (Harvard)](https://nlp.seas.harvard.edu/annotated-transformer/)
- 📄 FlashAttention / FlashAttention-2 论文

### 动手项目 ⭐
- **nanoGPT 复现**：跟着 Karpathy 从零写一个 GPT 并训练（[nanoGPT](https://github.com/karpathy/nanoGPT)）
- 自己实现 Multi-Head Attention + RoPE

---

## 3. 训练全流程（端到端串起来）

### 知识点
- 完整链路：数据加载 → 前向 → loss → 反向 → 梯度裁剪 → 优化器 step → 调度器
- **显存构成**：参数 + 梯度 + 优化器状态 + 激活值（为模块 03 的 ZeRO 打基础）
- 梯度累积、梯度裁剪、梯度检查点（用时间换显存）
- 训练稳定性：loss spike、NaN 排查、初始化、梯度范数监控
- 实验管理：`wandb` / `tensorboard`、seed、可复现性
- 微调范式：全参微调 vs LoRA/QLoRA（PEFT）

### 精选资源
- 🔗 HuggingFace [Transformers](https://huggingface.co/docs/transformers) & [PEFT](https://huggingface.co/docs/peft) 文档
- 📄 LoRA 论文
- 🔗 [HuggingFace Trainer 源码](https://github.com/huggingface/transformers)

### 动手项目 ⭐
- 用 HF Transformers 微调一个小模型（如 GPT-2 / Qwen-0.5B）
- 手动实现梯度累积 + 混合精度训练循环，对比显存占用

---

## ✅ 本模块自测
- [ ] 不看资料手写 Scaled Dot-Product Attention
- [ ] 讲清一次训练迭代中显存如何变化、峰值在哪
- [ ] 解释 fp16 与 bf16 区别及各自适用场景
- [ ] 说明 gradient checkpointing 如何用计算换显存
- [ ] 讲清 KV Cache 的作用与显存代价
