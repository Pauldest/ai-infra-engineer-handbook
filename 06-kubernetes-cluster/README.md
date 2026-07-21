# 06 · Kubernetes 与集群调度：工程落地能力

> 目标：补齐云原生与集群工程能力，能在 K8s 上用 GPU 调度跑起训练/推理任务，理解大规模集群的资源管理与运维。

---

## 1. 容器基础（前置）

### 知识点
- Docker：镜像分层、Dockerfile、多阶段构建、镜像优化
- 容器原理：namespace、cgroups（资源隔离的底层）
- **GPU 容器**：NVIDIA Container Toolkit、`--gpus`、CUDA 镜像
- 镜像仓库（registry）、私有仓库

### 资源
- 🔗 Docker 官方文档、[NVIDIA Container Toolkit](https://docs.nvidia.com/datacenter/cloud-native/)

---

## 2. Kubernetes 核心

### 知识点
- 核心对象：Pod、Deployment、Service、ConfigMap、Secret、Namespace、PVC
- 调度：scheduler、亲和性/反亲和性、taint/toleration、资源 request/limit
- **GPU 调度**：NVIDIA device plugin、`nvidia.com/gpu` 资源、MIG、GPU 共享（time-slicing/MPS）
- 存储：PV/PVC、StorageClass、共享存储（NFS/CephFS）对接训练数据
- 网络：CNI、Service、Ingress
- Operator 模式、CRD、Helm

### 精选资源
- 🔗 [Kubernetes 官方文档](https://kubernetes.io/docs/)
- 📗《Kubernetes in Action》
- 🎥 CNCF / KubeAcademy 教程

### 动手项目 ⭐
- 本地起 kind/minikube 集群，部署一个带 GPU 的推理 Pod
- 用 Helm 部署 vLLM 服务

---

## 3. 分布式训练调度（AI 场景核心）

### 知识点
- **Kubeflow Training Operator**：PyTorchJob、支持多机多卡
- **Volcano** / **YuniKorn**：批调度、gang scheduling（要么全调度要么不调度，训练必需）
- **Ray** / KubeRay：分布式计算与训练
- **Slurm**：传统 HPC 调度器（很多训练集群仍在用），sbatch、srun
- 队列管理、优先级、抢占、公平共享
- 弹性训练、故障恢复

### 精选资源
- 🔗 [Kubeflow](https://www.kubeflow.org/)、[Volcano](https://volcano.sh/)、[Ray](https://docs.ray.io/)
- 🔗 Slurm 官方文档

### 动手项目 ⭐
- 用 Kubeflow PyTorchJob 提交一个多卡 DDP 训练
- 配置 Volcano gang scheduling 跑分布式任务

---

## 4. 可观测性与运维（MLOps）

### 知识点
- **监控**：Prometheus + Grafana、DCGM Exporter（GPU 指标：利用率/显存/温度）
- **日志**：Loki / EFK（Elasticsearch+Fluentd+Kibana）
- **告警**：Alertmanager
- 分布式追踪、性能剖析
- **MLOps 工具链**：实验跟踪（MLflow/W&B）、模型注册、CI/CD、流水线（Argo Workflows / Kubeflow Pipelines）
- 成本优化：GPU 利用率提升、Spot 实例、资源超卖

### 动手项目 ⭐
- 部署 Prometheus+Grafana+DCGM，做一个 GPU 集群监控面板
- 用 Argo Workflows 编排一条训练→评估→部署流水线

---

## 5. 基础设施即代码（IaC，了解）
- Terraform、Ansible
- 云平台 GPU 实例（AWS/GCP/Azure）
- 网络（RDMA/InfiniBand 在云上的配置）

---

## ✅ 本模块自测
- [ ] 解释 cgroups/namespace 如何实现容器隔离
- [ ] 在 K8s 上申请并使用 GPU 资源
- [ ] 说明为什么分布式训练需要 gang scheduling
- [ ] 用 Kubeflow/Volcano 提交一个多机训练任务
- [ ] 搭建 GPU 集群监控并解读关键指标
