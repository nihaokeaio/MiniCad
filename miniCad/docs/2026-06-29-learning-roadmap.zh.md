# 2026-06-29 miniCad 学习路线图

本文用于记录 miniCad 后续一段时间的学习推进计划。目标不是一次性把所有模块做完整，而是保持主线清晰：每个阶段都完成一个可验证的闭环，然后再进入下一阶段。

当前 miniCad 已经具备一套 CAD runtime 雏形：

```text
Document / Element / Property
Command / UndoRedo
Scene / ElementMesh
Picking / BVH
Selection
TransformTool / Gizmo
Preview / ViewState
OCCT temporary presentation
```

接下来需要避免两个极端：

1. 一直深挖某个模块，导致主线停滞。
2. 过早重写渲染系统，导致现有 CAD 交互架构被打散。

因此后续路线采用：

```text
交互收尾
  -> Renderer MVP
  -> CAD 交互补全
  -> Scene / Dependency / Dirty System
  -> Persistence / Engineering
```

## 阶段 0：当前状态复盘

当前已经完成或基本完成的模块：

- 基础 `Element` 系统。
- `PropertySet` / `PropertyValue`。
- `Document` 数据层。
- `CommandManager` 与基础 undo / redo。
- `Scene` 运行时空间缓存。
- `ElementMesh`，包含 point / segment / triangle。
- object-level 和 primitive-level picking。
- BVH 初步接入。
- `PickTarget` 拆分为 `ElementPickTarget` 和 `GizmoPickTarget`。
- `SelectionManager` 基础选择能力。
- `TransformElementTool`。
- Gizmo axis / plane / ring / center。
- Gizmo 的 scene picking。
- Gizmo 的 retained-mode 显示优化。
- 拖动中 preview，鼠标释放后 command commit。

当前设计边界应该继续保持：

```text
Document owns persistent CAD data.
Scene owns runtime spatial state.
Picker owns spatial query.
SelectionManager owns selected targets.
Tool owns interaction session.
ViewStateAdaptor owns temporary view state.
Renderer / ViewAdaptor only display state.
```

## 阶段 1：Transform Gizmo 收尾

这是当前最接近完成的一块。建议先收尾，不继续无限扩展。

### 目标

让 Gizmo 达到“可用且设计边界清晰”的程度。

### 任务

1. 屏幕恒定大小 Gizmo
   - 当前 Gizmo 是世界空间大小。
   - 下一步根据 camera / projection / pivot depth 计算 scale。
   - 保持 `local geometry + worldTransform` 的结构。

2. Gizmo hover / active visual feedback
   - 当前已有 `hovered` / `active` 状态。
   - 需要让显示层根据状态改变颜色、线宽或透明度。

3. Gizmo display geometry 与 pick geometry 对齐
   - 显示多大，pick proxy 应该对应多大。
   - 后续屏幕空间缩放不能只改视觉，不改 picking。

4. 旋转和缩放的交互做基本可用
   - 不追求 Blender 级别完整。
   - 先保证轴向 rotate / scale 能稳定工作。

5. ESC / cancel 行为
   - 拖动中取消应恢复 preview 前状态。
   - 不提交 command。

### 暂不深入

- 数值输入。
- snapping。
- local / world transform space 完整切换。
- 高级 pivot mode。
- overlay depth policy 的最终实现。

这些放到后续阶段。

### 阶段完成标准

```text
选中对象
  -> Gizmo 显示在对象中心
  -> 屏幕大小基本稳定
  -> 点击不同 handle 进入不同约束
  -> 拖动时对象和 Gizmo preview 正确
  -> 释放鼠标提交 command
  -> undo / redo 正常
```

## 阶段 2：Renderer MVP

这一阶段不是重写整个显示系统，而是做一个最小 renderer spike，用来验证我们自己的图形管线。

### 为什么此时需要碰 Renderer

Gizmo 的几个问题已经属于渲染结构问题：

- 屏幕空间大小。
- overlay layer。
- depth policy。
- immediate layer。
- 只重绘辅助对象。

继续完全依赖 OCCT AIS 会绕开这些核心知识。因此需要做一个小型 renderer MVP，但不要马上替换 OCCT。

### 目标

建立一个最小自研 OpenGL 渲染管线，验证以下概念：

```text
Camera
MVP
Viewport
Depth test
Mesh rendering
Overlay / helper layer
Immediate drawing
```

### 任务

1. Renderer 基础框架
   - 初始化 OpenGL context。
   - 建立 shader / buffer / draw call 的最小封装。

2. Camera 与 MVP
   - 明确 world / view / clip / NDC / screen 的转换。
   - 写基础 camera controller 或复用当前 view 信息做验证。

3. Mesh 渲染
   - 从 `ElementMesh` 生成 VBO / IBO。
   - 先渲染 box / cylinder。

4. Grid / Axis
   - 用自己的 renderer 画网格和坐标轴。
   - 逐步减少 reference overlay 对 AIS 的依赖。

5. Helper / Overlay Layer
   - 尝试把 Gizmo 或简单线框放到 helper layer。
   - 验证 depth on / off 的效果。

### 暂不深入

- PBR。
- 材质系统。
- 复杂光照。
- 大规模 renderer 架构。
- 完全替换 OCCT viewer。

这一阶段只验证“我们能画，并且知道怎么控制层级”。

### 阶段完成标准

```text
能用自研 renderer 显示：
  - grid
  - axis
  - box mesh
  - cylinder mesh
  - 简单 helper line / gizmo proxy

并且能解释：
  - MVP 怎么走
  - depth test 怎么影响 gizmo
  - overlay layer 为什么有用
```

## 阶段 3：CAD 交互补全

Renderer MVP 完成后，回到 CAD 交互主线。此时我们已经知道渲染层的边界，不需要继续被 OCCT 的显示方式牵着走。

### 目标

补齐 CAD 软件最核心的交互能力。

### 任务

1. Sub-object selection
   - 点、边、面选择。
   - `SelectionManager` 支持 selection level。
   - `PickTarget` 可以表达 object / point / edge / face。

2. Selection filter
   - object mode。
   - vertex / edge / face mode。
   - helper object 是否可选。

3. Box selection / multi selection
   - 框选。
   - shift add / ctrl remove。
   - selection priority。

4. Snapping
   - grid snap。
   - endpoint / midpoint / center snap。
   - axis snap。
   - 后续可扩展到 intersection snap。

5. Transform system 增强
   - local / world space。
   - pivot mode。
   - numeric input。
   - transform constraint 状态机。

### 暂不深入

- 完整参数化约束求解器。
- 高级 sketch constraint solver。
- 工业级 selection priority。

这一阶段的重点是把 CAD 操作体验补齐，而不是一次做完整建模系统。

### 阶段完成标准

```text
用户可以：
  - 选择对象、点、边、面
  - 多选
  - 框选
  - 使用 snap 创建或移动对象
  - 使用 transform gizmo 做基本编辑
```

## 阶段 4：Sketch / Curve 基础

CAD 系统不能只靠 box / cylinder。后续建模能力需要 curve 和 sketch 基础。

### 目标

建立最小 sketch / curve 数据结构和交互。

### 任务

1. Line / Polyline
   - 创建线段。
   - 编辑端点。
   - 线段拾取。

2. Circle / Arc
   - 圆。
   - 圆弧。
   - 基础显示和拾取。

3. Sketch plane
   - 在某个 plane 上创建 sketch。
   - 2D 坐标和 3D world transform 之间转换。

4. Sketch element mesh
   - curve discretization。
   - segment pick proxy。

5. 简单建模入口
   - 先不做完整约束。
   - 可以尝试 extrude sketch profile。

### 暂不深入

- 完整 sketch solver。
- constraint graph。
- offset / trim / fillet 等复杂草图命令。

## 阶段 5：Scene Update / Dirty / Dependency

当对象变多、编辑关系变复杂后，需要更明确的更新系统。

### 目标

把“对象变化后哪些东西需要更新”表达清楚。

### 任务

1. Dirty flags
   - geometry dirty。
   - transform dirty。
   - mesh dirty。
   - bounds dirty。
   - render dirty。

2. Scene update queue
   - Document change 不立即散落调用多个系统。
   - 可以先进入 update queue。
   - frame end 或 command end 统一处理。

3. ElementMesh cache
   - 同类型或同参数对象是否能复用 mesh。
   - transform 独立于 mesh。

4. Dependency graph 初步设计
   - 父子 transform。
   - derived geometry。
   - sketch -> feature。

### 暂不深入

- 完整工业级 dependency graph。
- 增量拓扑命名。
- 历史树编辑。

这一阶段先解决“更新路径清晰”和“避免无意义重建”。

## 阶段 6：Persistence / Project Format

当基础数据和交互稳定后，需要保存和加载。

### 目标

让 miniCad 的 `Document` 能落盘。

### 任务

1. Document serialization
   - element type。
   - element id。
   - property set。
   - local transform。

2. Project file
   - 可以先用 JSON。
   - 后续再考虑 binary 或自定义格式。

3. Versioning
   - 文件版本号。
   - 简单 migration。

4. Undo / redo 与加载关系
   - 打开文件后 command stack 清空。
   - 保存点标记。

### 暂不深入

- STEP 完整读写。
- 工业级文件兼容。
- 大模型 streaming。

## 阶段 7：工程化与体验补强

这是让项目更像软件，而不是实验代码的一步。

### 任务

1. UI panel
   - object tree。
   - property inspector。
   - tool options。

2. Command history panel
   - 显示 undo stack。
   - 调试 command 行为。

3. Logging / Debug draw
   - pick ray debug。
   - BVH debug。
   - bounds debug。

4. Tests
   - geometry intersection tests。
   - command tests。
   - property tests。
   - scene update tests。

5. Performance profiling
   - picking time。
   - mesh build time。
   - render draw call count。

## 推荐推进节奏

后续每次学习尽量按这个节奏推进：

```text
1. 先复习上次的边界和遗留问题
2. 明确今天只解决一个主问题
3. 先做最小可验证实现
4. 再做一轮小整理
5. 写一篇阶段记录
```

避免出现：

```text
今天做 Gizmo
  -> 顺手重写 Renderer
  -> 顺手改 Scene
  -> 顺手设计 DependencyGraph
  -> 最后主问题没验证
```

每个阶段都应该有一个明确的完成标准。达到标准后，先停下来总结，再进入下一阶段。

## 当前最推荐的短期路线

结合目前项目状态，最推荐的短期顺序是：

```text
1. Transform Gizmo 收尾
   - 屏幕恒定大小
   - hover / active feedback
   - ESC cancel

2. Renderer MVP
   - grid / axis / simple mesh
   - MVP / depth / overlay

3. 回到 CAD 交互
   - sub-object selection
   - snapping
   - box selection

4. Sketch / Curve
   - line / circle / arc
   - sketch plane

5. Dirty / Dependency
   - update queue
   - mesh cache
```

这个顺序的理由是：

- Gizmo 已经接近闭环，应该先收住。
- Renderer 问题已经开始影响 Gizmo，不宜完全回避。
- 但 Renderer 只做 MVP，不直接替换整个显示系统。
- Renderer MVP 后再回到 CAD 交互，能更清楚地区分 interaction、scene、render 的职责。

## 主线判断标准

后续如果不确定下一步做什么，可以用这几个问题判断：

1. 这个功能是否能形成一个可验证闭环？
2. 它是否强化了当前架构边界？
3. 它是否服务于 miniCad 的长期目标？
4. 它是否会导致某个模块过早复杂化？
5. 做完后是否能写清楚一篇阶段总结？

如果答案不清楚，就先缩小任务范围。

## 总结

miniCad 后续不应该只沿着单一模块一直深入，而应该保持“CAD runtime 主线”和“图形学学习主线”交替推进。

当前推荐策略是：

```text
用 CAD 交互提出问题
用渲染 / 几何 / picking 知识解决问题
再把解决方案收束回 miniCad 架构中
```

这样既不会把项目做成单纯的 OCCT wrapper，也不会为了学习 renderer 而丢掉 CAD 软件的主线。
