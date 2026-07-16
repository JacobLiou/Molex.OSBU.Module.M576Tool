# IL 验证 vs comm 日志综合审计 (2026-07-08)

来源：
- IL：`验证IL.png`（21 条采样路径，MAX-MIN 为 PM 挡位极差）
- 通信日志：`comm_2026-07-08.log`
- 扫频 CSV：`comm_2026-07-08_recal_sweeps.csv`
- Run Path 规则：`step = (ch1-1)*18 + ch2`；slot1=`RECAL 1 3 ...`，slot2=`RECAL 1 4 ...`

**判定阈值**：MAX-MIN > 0.2 dB 视为异常 IL。

---

## 1. 总结论

| 维度 | 结论 |
|------|------|
| 根因 | **主要是 mesa 路径上的上位机策略缺陷**（非单纯「mesa 认错峰」） |
| 旧版指纹 | slot2：`fine64 Flat` → `coarse200 fail` → `stitch k1/k2 PND` → **`mesa bypass` → `dual_knee` → `skip fineRefine` → 直接 success** |
| 连带问题 | ① merge DAC 用 `col0+t×step` 在 301 点 merge 网格上算错（写出 -463 等）；② cross Y `power=0` 无门控仍落 LUT |
| 相关性 | **16 次** `mesa bypass`；**15 条** IL>0.2 路径中 **10 条**命中 mesa（slot2 占 8 条） |
| IL≤0.2 | 6 条路径均为 4 次扫频、`peak_ok=1`，**无 mesa bypass** |
| 已修复（不改算法前提下验证） | dual_knee → **fineRefine**；DAC 用 `DacAtMergedSampleIndex`；Y merge 门控；CrossPeakTest comm 回放见下 |

**综合判断**：高 IL 与 **slot2 mesa 直成功 + 错误 Y DAC + 无效 cross Y** 强相关；mesa 形态检测本身可触发，但致命的是**跳 fineRefine 与 DAC/交叉写回**。修复后应用 comm 回放验证：slot2 在 **fineRefine 或 Failed** 停下，**不再**写出 -463 一类 legacy DAC。

---

## 2. IL>0.2 全表（15 条）— 日志定位

| MAX-MIN | step | MPO in→out | slot1 log | slot2 log | mesa? | 旧版 slot 问题 |
|--------:|-----:|------------|-----------|-----------|:-----:|----------------|
| 3.5917 | 494 | MPO42-2→MPO90-2 | L9718 fine64 Ok | **L21128–21158** mesa, Y=-463 | slot2 | cross Y=0 |
| 2.9899 | 551 | MPO46-11→MPO94-11 | L10827 fine64 Ok | **L22281–22311** mesa, Y=-495 | slot2 | cross Y=0 |
| 2.3530 | 241 | MPO21-1→MPO69-1 | L4737 fine64 Ok | **L16116–16146** mesa, Y=-427 | slot2 | cross Y=0 |
| 2.2313 | 444 | MPO37-12→MPO85-12 | L8722 fine64 Ok | **L20146–20176** mesa, Y=-455 | slot2 | cross Y=0 |
| 2.2015 | 458 | MPO39-2→MPO87-2 | L9002–9032 | L20426 slot1 mesa Y=-420 | slot1 | slot1 cross Y=0 |
| 1.9543 | 547 | MPO46-7→MPO94-7 | **L10731–10761** mesa Y=-433 | L22193 | slot1 | slot1 cross Y=0 |
| 1.6695 | 570 | MPO48-6→MPO96-6 | L11206 | L22656 多 att PND | 否 | slot1 四轮 Y fail |
| 1.1819 | 332 | MPO28-8→MPO76-8 | L6514–6592 mesa | **L17938–17968** mesa Y=-350 | 双端 | 双端 cross Y=0 |
| 1.0814 | 529 | MPO45-1→MPO93-1 | L10389 Ok | **L21819–21849** mesa Y=-389 | slot2 | cross Y=0 |
| 0.9973 | 187 | MPO16-7→MPO64-7 | L3673–3688 mesa | **L15058–15769** mesa | 双端 | 双端 |
| 0.7323 | 61 | MPO06-1→MPO54-1 | L1231 多 att fail | **L12524–12539** mesa | slot2 | slot2 四轮 Y fail 后 mesa |
| 0.6051 | 223 | MPO19-7→MPO67-7 | L4395 多 att | L15754 | 否 | slot1 多 att PND |
| 0.5321 | 457 | MPO39-1→MPO87-1 | L457 多 att fail | L20426 邻步 | 否 | slot1 四轮 fail |
| 0.5017 | 259 | MPO22-7→MPO70-7 | ~L5093 mesa | L259 slot2 | slot1 | slot1 mesa |
| 0.3002 | 335 | MPO28-11→MPO76-11 | L335 | — | 否 | 4 sweep 全 Ok |

`Step N/576 (slot X) RECAL 1 -> OK` 为 Run Path 步进标记；mesa 细节见同段后 30–40 行内 `stitch_k2 symmetric trio mesa bypass`。

---

## 3. Top-4 高 IL — slot2 旧版指令链（step 494 示例）

| 次序 | 日志行 | 指令 / 结果 |
|-----:|--------|-------------|
| route | 21128 | `RECAL 1 4 28 8 8 60` Step 494 slot2 |
| att1 | 21129–32 | `RECAL 3 0 9999 9999 64` → fine64 **Flat** |
| att2 | 21133–36 | `RECAL 3 0 9999 9999 200` → coarse200 **PND** |
| att3 | 21137–40 | `RECAL 3 0 9999 -483 200` → stitch k1 **StrictInc** skip merge |
| att4 | 21141–44 | `RECAL 3 0 9999 317 200` → stitch k2 **mesa bypass** |
| 旧成功 | 21143–44 | `dual_knee t*=165.5` → **success (skip fineRefine)** |
| 错 DAC | 21148–49 | `RECAL 3 1 9999 **-463** 64`（应为 merge 中心约 **-21**） |
| cross | 21154–55 | Y 全局最大 **功率=0**，拟合点无 |

其余三步（551/241/444）模式相同：`legacyWrongDac` 分别为 **-495 / -427 / -455**，真实 merge 中心约 **-53 / +13 / -12**。

---

## 4. IL≤0.2 正常路径（6 条）

| MAX-MIN | step | MPO | 特征 |
|--------:|-----:|-----|------|
| 0.2707 | 85 | MPO08-1→MPO56-1 | 4 sweep，全 peak_ok |
| 0.2545 | 88 | MPO08-4→MPO56-4 | 同上 |
| 0.2404 | 92 | MPO08-8→MPO56-8 | 同上 |
| 0.2094 | 90 | MPO08-6→MPO56-6 | 同上 |
| 0.1885 | 96 | MPO08-12→MPO56-12 | 同上 |
| 0.1696 | 87 | MPO08-3→MPO56-3 | 同上 |

**无** `mesa bypass` / `skip fineRefine` 记录。

---

## 5. CrossPeakTest comm 回放用例（修复后验证）

实现：`CrossPeakTest/main.cpp` → `RunComm20260708Slot2IlMesaReplayTests()`  
数据：`comm_2026-07-08_recal_sweeps.csv`（优先仓库根目录完整文件）

| step | IL | legacyWrongDac | 断言 |
|-----:|---:|----------------:|------|
| 494 | 3.59 | -463 | phase≠Succeeded；FineRefine 时 pendingFineBase 远离 -463 且≈merge DAC；flat fineRefine→Failed |
| 551 | 2.99 | -495 | 同上 |
| 241 | 2.35 | -427 | 同上 |
| 444 | 2.23 | -455 | 同上 |
| 529 | 1.08 | -389 | 同上 |
| 332 | 1.18 | -350 | 同上 |

回放使用 CSV 记录的 `peak_ok`/`code`（与 07-08 实机会话一致），feed 四轮 slot2 Y 扫频后经 pipeline：

- **不得** `Succeeded`（旧 mesa 直成功）
- **必须** `FineRefine` 或 `Failed`
- `FineRefine`：`pendingFineBase` 对齐 `DacAtMergedSampleIndex(mergeCol0, t*, 4)`
- 追加 flat fine64 → `Failed` / `fineRefine`

构建与运行：

```text
msbuild M576Calibrator\CrossPeakTest\CrossPeakTest.vcxproj /p:Configuration=Release /p:Platform=Win32
M576Calibrator\CrossPeakTest\Release\CrossPeakTest.exe
```

---

## 6. 与 mesa 算法的关系（边界说明）

- **mesa 检测**（`IsMergedMesaProfile` / `dual_knee`）：在 plateau+单调拼接场景仍会触发——这是预期形态分类。
- **旧版危害**：把 dual_knee 当 **terminal success**，并用 **错误 DAC 公式** 与 **无 Y 有效性** 的 cross 写 LUT。
- **修复后**：dual_knee → **fineRefine**（或 Y 门控失败 → **Failed**）；DAC 与 cross 分离校验；**算法检测逻辑未删**，仅收敛落盘路径。

固件扫频/判峰能力增强后，上位机 mesa 兜底可再收敛；在 FW 未改前保留 stitch/mesa 重试路径（见 `INVARIANTS.md` INV-10）。
