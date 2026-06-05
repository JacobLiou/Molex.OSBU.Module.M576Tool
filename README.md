# M576 波长自校准上位机

Windows 产线定标工具：通过 **439F 控制板单串口**，在四级级联光开关 + 功率计/PD 拓扑上，为 M576 在 **1310 nm** 下自动完成多 MEMS 通道 DAC 定标（约 **1286 步路径**）、二维寻峰、定标表读回与 Z4671 兼容 BIN 的备份/生成/烧录及重复性验证。

> 物理拓扑与业务背景见 [`claude.md`](claude.md)、[`需求Spec PRD.md`](需求Spec PRD.md) 及 [`物理拓扑图.png`](物理拓扑图.png)（若仓库内提供）。

---

## 业务主流程

产线出货前定标的标准顺序（勿打乱）：

| 步骤 | 操作 | 说明 |
|------|------|------|
| 1 | **Read Bin / ReadFlashBk** | 经 `trans` 从 2×MCS、2×1×64 读 Flash，备份旧 BIN |
| 2 | **Calibrate / Run path** | 按路径 CSV 逐步 RECAL 扫频、寻峰、写会话定标结果 |
| 3 | **Write Bin** | 将会话结果合并到各子模块 BIN 正确 DAC 位置 |
| 4 | **Burn Flash** | 经 `trans` 将新 BIN 烧录回各从属设备 |

```mermaid
flowchart LR
  read[ReadFlashBk 备份]
  cal[Run path 校准]
  write[Write BIN]
  burn[Burn Flash]
  read --> cal --> write --> burn
```

**产线操作员**请参阅 [M576Calibrator/Docs/USER_MANUAL_CN.md](M576Calibrator/Docs/USER_MANUAL_CN.md)（傻瓜式步骤，无需了解协议细节）。

---

## 仓库结构

```
├── M576Calibrator/          # 主工程（Visual Studio 解决方案）
│   ├── M576CalibratorApp/   # MFC 上位机 UI 与业务编排
│   ├── Z4671Core/           # 串口、Z4671 二进制协议、LUT/MEMS bin 读写（静态库）
│   ├── CrossPeakTest/       # 寻峰 / 重试策略单元测试（控制台）
│   ├── Docs/                # 开发文档、不变量、LUT 索引、用户手册
│   └── output/              # 默认路径 CSV、comm 日志、BIN 输出目录
├── dataAnalysis/            # 日志后处理（PyInstaller 打包 exe）
├── tools/                   # comm 日志提取、路径 CSV 辅助脚本
├── Z4671/ / 1X64 Switch/    # 历史 Z4671 / 1×64 参考工程（非主构建链）
├── claude.md                # 项目级 AI / 架构约束（贡献者必读）
└── 需求Spec PRD.md           # 产品需求与物理拓扑说明
```

### 核心模块（M576CalibratorApp）

| 模块 | 职责 |
|------|------|
| `CRecalSession` | ASCII `RECAL` 扫频与会话 |
| `PeakFinder2D` / `Peak1DSweepRecenter` | 1D/2D 寻峰、Flat/单调失败重试（expand range + base 平移） |
| `PathCsvDriver` | 1286 步路径 CSV 驱动 |
| `McsFwTransport` / `Switch1x64FwTransport` | MCS / 1×64 Flash 读烧（`trans` + Z4671 二进制） |
| `LutMerge1310` | 1310 低温槽 DAC 合并写 BIN |

架构详图见 [M576Calibrator/Docs/DEVELOPMENT_AND_CODE_GUIDE.md](M576Calibrator/Docs/DEVELOPMENT_AND_CODE_GUIDE.md)。

---

## 构建与运行

### 环境

- **OS**：Windows 10/11
- **IDE**：Visual Studio 2019/2022（工程含 v140 配置；MFC 动态库）
- **平台**：Win32（x86）为主程序目标
- **硬件**：PC 单 COM 口连接 **439F**，默认 **115200 8N1**

### 编译主程序

1. 打开 [`M576Calibrator/M576Calibrator.sln`](M576Calibrator/M576Calibrator.sln)
2. 配置 **Release | Win32**（或 Debug）
3. 生成 **M576CalibratorApp**（依赖 **Z4671Core**）
4. 运行生成的 exe；路径 CSV 与输出默认在 `M576Calibrator/output/`

### 运行寻峰单元测试

```text
M576Calibrator\CrossPeakTest\Release\CrossPeakTest.exe
```

Release | Win32 生成后可直接运行，含寻峰/重试自测。

### 打包日志提取工具

```bat
cd dataAnalysis
build_extract_comm_logs.bat
```

产出：

- `dataAnalysis/dist/ExtractCommLogs.exe`
- 同步复制到 `M576Calibrator/output/ExtractCommLogs.exe`

将 exe 与 `comm_YYYY-MM-DD.log` 放在同一目录，双击即可生成 `comm_*_recal_sweeps.csv` 与 `comm_*_peak_summary.csv`。

---

## 关键参数（界面可配）

| 参数 | 典型值 | 说明 |
|------|--------|------|
| DAC range (offset) | 64 | RECAL 3/5 扫频半宽；重试时可 expand（×2，**上限 200**） |
| DAC step | 4 | 扫点步长 |
| delay ms | 固件相关 | 影响 RECAL 读超时估算 |
| PM / PD Mode | RECAL 1 / 2 | 功率计 vs 板载 PD 定标路径 |

寻峰重试策略（Flat expand、单调先 expand 再平移等）见 [`.cursor/rules/m576-peak-recal.mdc`](.cursor/rules/m576-peak-recal.mdc) 与 [INVARIANTS.md INV-10..19](M576Calibrator/Docs/INVARIANTS.md)。

---

## 文档索引

| 文档 | 读者 | 内容 |
|------|------|------|
| [USER_MANUAL_CN.md](M576Calibrator/Docs/USER_MANUAL_CN.md) | 产线操作员 | 备份→校准→写 BIN→烧录 |
| [M576Calibrator使用说明.md](M576Calibrator/M576Calibrator使用说明.md) | 现场工程师 | 界面字段、RECAL、透传说明 |
| [DEVELOPMENT_AND_CODE_GUIDE.md](M576Calibrator/Docs/DEVELOPMENT_AND_CODE_GUIDE.md) | 开发维护 | 模块架构、类索引 |
| [INVARIANTS.md](M576Calibrator/Docs/INVARIANTS.md) | 开发 / AI | **不可违反**的 INV-xx 条文 |
| [LUT_INDEXING.md](M576Calibrator/Docs/LUT_INDEXING.md) | 开发 | SN / sw / ch / MCS 块号对照 |
| [需求Spec PRD.md](需求Spec PRD.md) | 产品 / 固件 | 需求、拓扑、1286 步路径 |
| [claude.md](claude.md) | AI / 架构 | 系统约束、禁止事项、DoD |

---

## 开发原则（摘要）

完整条文见 [`claude.md`](claude.md)。核心要求：

- **可测试、可模拟、可回放**：CrossPeakTest、comm 日志 CSV、ExtractCommLogs
- **通信有 ID、返回码、超时**；禁止仅靠打印判断结果
- **UI 线程不阻塞**；串口 / Flash IO 异步，异常可追踪
- **改 Merge / RECAL / Read 前**先读 [INVARIANTS.md](M576Calibrator/Docs/INVARIANTS.md) 与 [LUT_INDEXING.md](M576Calibrator/Docs/LUT_INDEXING.md)
- **1310 只写低温槽**；SN33/34、CH19–32 等保留规则见 INV-01..06

Cursor 规则目录：`.cursor/rules/`（如 `m576-peak-recal.mdc`、`m576-lut-bin.mdc`）。

---

## AI 贡献者

在生成或修改代码前，请先阅读：

1. [`claude.md`](claude.md) — 系统边界、禁止事项、成功标准（DoD）
2. [INVARIANTS.md](M576Calibrator/Docs/INVARIANTS.md) — 不变量自检
3. 相关 `.cursor/rules/*.mdc` — 子域约束（寻峰、LUT、透传等）

明确：**输入 / 输出 / 通信对象 / 失败处理 / 可测试性** 后再动手。

---

## 许可证与遗留工程

- **Z4671/**、**1X64 Switch/** 为历史参考，与当前 M576 主链路并行存在；新功能应落在 `M576Calibrator/`。
- BIN 布局与 `Z4767StructDefine.h` 保持与 Z4671 产线工具兼容。

