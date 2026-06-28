# 2026-06-28 Gizmo 鼠标交互学习记录

本文记录今天围绕 `Gizmo`、`TransformElementTool` 和鼠标交互链路的学习与设计过程。它不是最终设计规范，而是一次阶段性复盘：我们从“能拖动物体”开始，逐步把 move / rotate / scale、轴向约束、平面约束、gizmo picking、view preview 和性能优化串起来。

## 今日主题

今天的核心主题是：

```text
如何把一个 CAD/建模软件里的 Transform Gizmo 接入到自己的 interaction system 中。
```

这里的关键不是画出一个坐标轴，而是让它真正参与交互：

- 鼠标能命中 Gizmo handle。
- 命中的 handle 能决定 transform mode 和 constraint。
- 拖动时能实时 preview。
- 松开鼠标后能提交 command。
- Gizmo 自身能跟随对象变化。
- 辅助线、临时状态、拾取几何和显示几何有明确生命周期。

## 第一阶段：先打通自由移动

一开始最重要的不是 Gizmo 本身，而是先验证 transform 预览链路是否成立。

我们先做的是自由移动：

```text
MousePress
  -> 记录 startPoint

MouseMove
  -> 计算 currentPoint
  -> delta = currentPoint - startPoint
  -> 根据 delta 构造 transform
  -> ViewStateAdaptor 预览对象 transform

MouseRelease
  -> CadController::TransformElements
  -> TransformElementsCommand
  -> Document
```

这一阶段的重点是把 `delta` 和 `ViewStateAdaptor` 打通。

也就是说，鼠标拖动时不立刻改 `Document`，而是只改视图里的临时 transform。真正落地的数据变更仍然走 command：

```text
preview during dragging
commit on mouse release
```

这是后续 undo / redo 能成立的基础。

## 第二阶段：加入按键式轴向约束

自由移动能跑通之后，我们开始补 `X / Y / Z` 约束。

这个阶段暴露出一个重要问题：约束不是简单地把 delta 的某些分量置零。鼠标在屏幕上移动，实际需要先把屏幕坐标转换到一个可解释的三维空间中。

当前处理方式大致是：

- 自由移动或平面移动：使用 ray-plane intersection。
- 轴向移动或轴向缩放：使用 mouse ray 和 constraint axis 的最近点关系。

对应逻辑在 `TransformElementTool` 中拆成：

```text
ResolvePlaneDelta
ResolveAxisDelta
BuildMoveTransform
BuildScaleTransform
BuildRotateTransform
```

这一阶段的设计重点是：

```text
constraint 应该影响 delta 的解析方式，而不只是影响最终 transform。
```

否则就会出现鼠标和对象运动方向不一致、拖动偏差明显、某些视角下无法稳定拖动的问题。

## 第三阶段：补充 Rotate 和 Scale

移动之后，我们继续加入旋转和缩放。

旋转的关键是：

```text
startVector = startPoint - pivot
currentVector = currentPoint - pivot
angle = signed angle around constraint axis
```

缩放的关键是：

- 自由缩放可以根据鼠标点到 pivot 的距离变化计算比例。
- 轴向缩放可以根据 delta 在 constraint axis 上的投影计算比例。

这里开始能看到 Blender 这类软件的交互思想：`G / R / S` 是主操作，`X / Y / Z` 是子约束。

当前 miniCad 还没有完整实现 Blender 那种连续键盘状态机，但方向已经明确：

```text
TransformMode: Move / Rotate / Scale
TransformConstraint: Free / X / Y / Z / XY / YZ / ZX
```

这套状态组合后续可以继续扩展到：

- 本地坐标系 / 世界坐标系
- 数值输入
- 连续输入
- snapping
- pivot mode

## 第四阶段：从“工具状态”走向“Gizmo handle”

一开始的 transform tool 更像是：

```text
开启工具后，点击任意位置都可以拖动。
```

这不符合正常 Gizmo 的交互预期。合理的行为应该是：

```text
必须点中某个 Gizmo handle，才开始对应的 transform。
```

因此我们引入了 `GizmoHandleId` 和 `GizmoPickTarget`。

`GizmoHandleId` 描述的是语义：

```text
mode: Move / Rotate / Scale
constraint: X / Y / Z / XY / YZ / ZX / Free
type: Axis / Plane / Ring / Center
```

`GizmoPickTarget` 则把拾取结果从普通 `ElementPickTarget` 中拆出来。

这个拆分很关键。原因是 Gizmo 不是 Document 里的模型对象，它是 interaction helper。它可以有几何、可以参与 picking，但它不应该被当成普通 CAD element。

当前语义是：

```text
ElementPickTarget: 命中了 Document 中的对象或子对象
GizmoPickTarget: 命中了交互控件上的 handle
```

这让 `TransformElementTool` 可以只关心 handle 语义：

```text
Axis X -> X axis move / scale / rotate
Plane XY -> XY plane move
Center Free -> free move
Ring Z -> Z axis rotate
```

而不需要知道具体命中了哪个三角形或线段。

## 第五阶段：把 Gizmo 接入 Scene picking

为了让 Gizmo 被自己的 picking 系统命中，我们把它作为 `SceneWidget` 接入 `Scene`。

这里的边界是：

```text
SceneElement: 来自 Document 的真实对象
SceneWidget: 来自 Interaction 的临时交互对象
```

两者都可以拥有 pick geometry，但语义不同。

当前拾取路径大致是：

```text
MousePress
  -> GizmoManager::Pick
  -> ScenePicker::Pick
  -> 优先 PickWidget
  -> PrimitivePicker
  -> GizmoPickTarget
```

这个设计有两个好处：

1. Gizmo 可以复用已有的 ray / primitive / BVH picking 逻辑。
2. Gizmo 不污染 `Document`，不会被 undo / redo、保存、导出等模型数据流程误处理。

## 第六阶段：补齐 Axis / Plane / Ring / Center

今天补齐了几个 Gizmo handle 类型：

- `Axis`：用于 X / Y / Z 轴向移动、缩放。
- `Plane`：用于 XY / YZ / ZX 平面移动。
- `Ring`：用于 X / Y / Z 轴旋转。
- `Center`：用于自由移动。

这里要注意一个设计点：

```text
Gizmo 的显示几何和 picking 几何可以相关，但不一定完全一样。
```

当前为了简单，显示和拾取都比较接近。但后续如果要做成屏幕空间 Gizmo，pick geometry 可能需要更适合鼠标命中的形状，而显示几何则更偏视觉表达。

例如：

- 视觉上是一根细轴。
- picking 上可以是一根带容差的 segment 或 cylinder-like proxy。

这也是为什么我们没有把 Gizmo 设计成普通 AIS 对象直接选择，而是让它进入自己的 `SceneWidget` / `GizmoPickTarget` 流程。

## 第七阶段：辅助线与拖动状态

当点击某个轴或平面开始拖动时，Gizmo 进入约束状态。

这里有两个视觉元素：

1. 主 Gizmo
   - 应该跟随对象移动。
   - 拖动时整体 pivot 会变化。

2. 辅助线
   - 应该固定在开始拖动时的 pivot。
   - 表达这次拖动的约束方向或约束平面。

这就是今天引入 `constraintPivot` 的原因。

```text
state.pivot
  当前主 Gizmo 显示位置

state.constraintPivot
  开始拖动时的约束参考点
```

对于轴向移动：

```text
主 Gizmo 跟随对象移动
辅助轴线固定在拖动开始的位置
```

对于平面移动：

```text
主 Gizmo 跟随对象移动
辅助平面方向线固定在拖动开始的位置
```

这个拆分避免了一个常见问题：所有东西都跟着对象跑，导致用户看不清这次 transform 的参考约束。

## 第八阶段：性能问题与 retained-mode 改造

今天后半段重点处理了 Gizmo 显示性能问题。

原来的问题是：

```text
ShowTransformGuide()
  -> ClearObjects()
  -> 重建所有 AIS object
  -> Display()
```

拖动时每一帧都走这条路径，就会导致明显的性能浪费。

我们把它改成了 retained-mode 思路：

```text
拓扑变化时：
  rebuild objects

姿态变化时：
  update local transform
```

也就是说：

- mode / constraint 改变，才需要重建几何。
- 拖动时主 Gizmo 只需要更新 `SetLocalTransformation`。
- 辅助线在开始拖动时建立，拖动期间只保持固定 transform。

当前 `TransformGuideAdaptor` 内部拆成：

```text
m_MainGuideObjects
m_ConstraintGuideObjects
m_Topology
```

主 Gizmo 和辅助线分开管理，生命周期也更清楚。

## 第九阶段：SceneWidget 也改成局部几何 + transform

显示侧做了 retained-mode 后，picking 侧也需要一致。

原来 Gizmo widget mesh 是直接用世界坐标构建的。这样拖动时如果 Gizmo 跟随对象，就需要不断重建 widget mesh 和 primitive BVH。

现在改成：

```text
widget mesh: local space
widget worldTransform: pivot transform
```

拾取时：

```text
world ray
  -> inverse widget worldTransform
  -> local ray
  -> primitive picking
```

这和普通 element picking 的思路一致，也为未来 BVH / renderer 统一打基础。

## 第十阶段：清理隐式刷新链路

最后我们又处理了一个职责混杂问题。

原来的 `ClearDragState()` 会调用 `RefreshGuideFromSelection()`，而后者内部又会调用 `UpdateIntersectionPlane()`，`UpdateIntersectionPlane()` 又会隐式调用 `UpdateGizmoState()`。

这条链路的问题是：函数名看起来是在“刷新选择中心”，实际却会触发 Gizmo 显示和几何重建。

现在拆成：

```text
RefreshPivotFromSelection()
  只计算 selection center

UpdateIntersectionPlane()
  只更新 drag plane / constraint axis

ShowIdleGizmoFromSelection()
  明确表示根据当前选择显示 idle Gizmo
```

这个修改的重点不是少写几行代码，而是把副作用显式化。

对于交互系统来说，隐式副作用很容易制造问题：

- 鼠标事件中多次重建几何。
- 状态切换时重复显示。
- 某个工具退出时误触发 view update。
- 后续调试时很难判断是谁触发了状态变化。

## 当前交互流总结

当前移动工具的核心流程可以概括为：

```text
开启 TransformElementTool
  -> 根据 selection center 显示 idle Gizmo

MouseMove idle
  -> GizmoManager::UpdateHover
  -> ScenePicker pick widget

MousePress
  -> GizmoManager::Pick
  -> 得到 GizmoPickTarget
  -> 设置 mode / constraint
  -> BeginDrag

MouseMove dragging
  -> ResolveDelta
  -> BuildTransform
  -> ViewStateAdaptor::ApplyElementTransforms
  -> GizmoManager::UpdatePose

MouseRelease
  -> CadController::TransformElements
  -> TransformElementsCommand
  -> Document
  -> ClearDragState
  -> 恢复 idle Gizmo
```

这个流程里有一个重要原则：

```text
拖动中只做 preview，松开后才提交 command。
```

## 今天遇到的主要坑

### 1. 点击任意位置就开始 transform

这是早期工具逻辑的问题。Transform tool 不能只看“当前是否选中了对象”，还必须看“鼠标是否命中了 Gizmo handle”。

解决方式是引入 `GizmoPickTarget`，让鼠标点击必须先经过 Gizmo picking。

### 2. PickResult 信息越多不一定越好

一开始容易把所有命中信息都塞进一个 `PickResult`。但普通对象和 Gizmo 的语义不同。

更清晰的做法是：

```text
PickResult
  contains PickTarget

PickTarget
  can be ElementPickTarget or GizmoPickTarget
```

这样 selection、gizmo、未来 sub-object edit 可以各自处理自己的语义。

### 3. Gizmo 不能进入 Document

Gizmo 是临时交互对象，不是 CAD 数据。

它可以进入 `Scene` 做 picking，可以进入 `Presentation` 做显示，但不应该进入 `Document`。

否则会影响：

- undo / redo
- save / load
- export
- selection semantics
- command history

### 4. 拖动时不要每帧重建几何

Gizmo 的轴、平面、环本身没有变，变的是整体 transform。

每帧清空重建 AIS object 和 pick widget 是不必要的。正确方向是：

```text
geometry is stable
transform is dynamic
```

### 5. 辅助线和主 Gizmo 的 pivot 不一样

主 Gizmo 应该跟随对象走，但辅助线应该固定在拖动开始位置。

所以需要区分：

```text
display pivot
constraint pivot
```

### 6. 函数名必须反映副作用

`RefreshGuideFromSelection()` 这种函数名容易掩盖内部副作用。

更好的拆法是：

```text
RefreshPivotFromSelection
UpdateIntersectionPlane
UpdateGizmoState
```

让每个函数只做一件明确的事情。

## 当前设计仍然存在的问题

今天完成后，Gizmo 已经能作为世界空间对象正常工作，但它还不是最终形态。

当前主要问题是：

1. Gizmo 是世界空间大小
   - 离相机远会变小。
   - 离相机近会变大。
   - 容易被模型遮挡。

2. Gizmo 和模型深度关系还不够理想
   - 屏幕空间操作器通常会有 overlay 或特殊 depth 策略。

3. hover / active 高亮还没有完整视觉反馈
   - 当前 `hovered` 和 `active` 状态已经存在，但还没有完整驱动显示样式。

4. 旋转和缩放交互还比较基础
   - 还没有数值输入。
   - 还没有 snapping。
   - 还没有本地坐标系 / 世界坐标系切换。

5. Gizmo geometry 构造仍然偏集中
   - `TransformGuideAdaptor` 里已经拆了缓存和生命周期，但几何构造函数还比较多。
   - 后续可以考虑独立成 `GizmoGeometryBuilder` 或 renderer-native gizmo draw path。

## 明天的方向：世界空间 Gizmo 到屏幕空间 Gizmo

明天适合收尾的一步是：把当前世界空间 Gizmo 改成更接近建模软件的屏幕空间 Gizmo。

核心目标是：

```text
Gizmo 在屏幕上保持稳定大小。
```

这通常需要解决几个问题：

1. 根据相机距离计算 world scale。

```text
screen size -> world size at pivot depth
```

2. Gizmo picking 使用同一套 scale。

显示多大，pick proxy 就应该对应多大。

3. 处理遮挡和 depth。

后续可以选择：

- 仍在 3D scene 中绘制，但动态缩放。
- 使用 overlay pass。
- 对 Gizmo 使用特殊 depth test / depth priority。

4. 统一显示几何和 pick 几何的 transform。

当前已经有：

```text
local geometry + worldTransform
```

这正好是实现屏幕空间 Gizmo 的基础。

## 今日结论

今天的核心成果不是“画出了一个 Gizmo”，而是把 Gizmo 放进了 miniCad 自己的 interaction architecture 里：

```text
Gizmo visual
Gizmo pick geometry
Gizmo semantic target
Transform tool state
Preview transform
Command commit
```

这几件事已经串起来了。

目前的设计边界可以总结为：

```text
TransformElementTool
  owns transform interaction session

GizmoManager
  owns gizmo semantic state and widget picking state

TransformGuideAdaptor
  owns temporary gizmo visual objects

SceneWidget
  owns gizmo pick geometry

Document
  only receives final committed transform
```

只要继续保持这个边界，后面做屏幕空间 Gizmo、hover highlight、snapping、local/world transform space、甚至自研 OpenGL gizmo renderer，都不会破坏当前主线。
