# miniCad 技术架构阶段总结

日期：2026-06-10

本文档用于记录 miniCad 当前阶段的架构状态。它不是最终设计规范，而是一个阶段性总结。当前项目还处在从 OCCT 驱动的原型，逐步过渡到自研 CAD interaction、scene、picking、BVH 和 renderer 的阶段。

## 项目目标

miniCad 是一个偏学习和实验性质的 CAD 原型项目。当前实现中：

- Qt 负责 UI、widget、mouse/keyboard event dispatch。
- OpenCASCADE 负责 geometry construction 和临时显示能力。
- miniCad 自己逐步接管 document data、command、scene state、picking、mesh、BVH，以及未来的 OpenGL rendering。
- OCCT AIS 当前仍作为 temporary presentation backend 使用。

长期方向是：让 OCCT 更多承担 geometry kernel 的角色，而不是控制整个 selection 和 rendering 流程。

## 当前模块结构

```text
AppContext
  应用级 composition root，负责创建并连接核心服务。

Data
  Document, Element, Property, ElementFactory, ElementMesh。

Controller
  CadController, Command system, Interaction system, PreviewManager。

Scene
  从 Document 派生出来的 runtime scene cache。

Presentation
  ViewAdaptor, PreviewAdaptor, ReferenceOverlay, SelectionManager, ViewObjectRegistry。

Rendering
  CadView，以及当前 OCCT viewer/widget integration。

Utils
  MessageInfo, GeomCalculator, GlobalUniqueId, MacroSet 等工具。
```

## 数据权威和职责边界

当前最重要的边界是：

```text
Document owns CAD data.
Scene owns runtime spatial state.
Picker owns spatial query.
SelectionManager owns selected IDs.
Presentation only displays current state.
```

`Document` 是权威数据源。它 owns all `Element` instances，并负责 element registration、lookup 和 change notification。

`Scene` 不是第二份数据源，而是从 `Document` 同步出来的 runtime cache。它当前保存用于 interaction 和 spatial query 的信息：

- `ElementId`
- `localTransform`
- `worldTransform`
- `boundingBox`

因此可以这样理解：

- `Document` 回答：“对象是什么？属性是什么？”
- `Scene` 回答：“对象当前在空间中处于什么状态？能否参与空间查询？”
- `Presentation` 回答：“对象当前如何显示？”

这个边界后续会直接影响 BVH、custom picking 和 OpenGL renderer 的设计。

## Application Bootstrapping

当前由 `AppContext::Initialize()` 完成系统组装。

```text
Document
Scene
SelectionManager
ViewObjectRegistry
CadView
ViewAdaptor
PreviewAdaptor
ReferenceOverlay
CoordinateResolver
DocumentObserver
CommandManager
CadController
PreviewManager
InteractionManager
```

`AppContext` 当前是 service container 和 composition root。这个设计在当前阶段可以接受，因为项目规模还不大，而且它让对象依赖关系比较集中。

不过它已经开始变重。后续如果继续增加 manager，可以考虑拆成：

- viewer setup
- controller setup
- presentation setup
- interaction setup

这样可以避免 `AppContext::Initialize()` 无限膨胀。

## Element 和 Property System

`Element` 是 CAD 对象的基类。当前已有的 concrete elements：

- `BoxElement`
- `CylinderElement`

每个 `Element` 持有一个 `PropertySet`。当前关键 transform property 是：

```text
LocalTransform
```

`LocalTransform` 存储 `GeometryTypes::RTransform`。之前单独的 `Position` 已经被移除。这样可以让 translation、rotation、scale 都走统一的 transform 路径。

`Element::OnSetProperty()` 会返回 `ElementUpdateHint`，用于区分不同更新类型：

- `Geometry`
- `Transform`
- `Property`
- `All`

这为后续 dirty flag 和 dependency graph 留出了接口基础。

## Command System

`Command system` 负责 undoable operation。当前核心命令包括：

- `CreateElementCommand`
- `ChangePropertyCommand`

`CadController` 是 UI 和 Tool 访问模型层的主要入口。它不直接让 UI 修改 `Document`，而是创建 command 并提交给 `CommandManager`。

当前命令流：

```text
UI or Tool
  -> CadController
  -> CommandManager
  -> Command::Execute()
  -> Document mutation
  -> Document notification
```

这条路径的价值是：后续 undo/redo、macro command、transaction 都可以在 command 层扩展，而不是散落在 UI 或 Tool 里。

## Document Notification Flow

`Document` 通过 callback 发出 element change payload。该 callback 当前由 `DocumentObserver` 注册。

当前同步流：

```text
Document
  -> DocumentObserver
  -> Scene::AddOrUpdate / Remove
  -> ViewAdaptor::UpdateElement
```

`DocumentObserver` 当前同时更新 `Scene` 和 `ViewAdaptor`。这在当前阶段是可以接受的，因为它避免了 `Document` 直接依赖 `Scene` 或 `ViewAdaptor`。

后续如果更新关系变复杂，可以考虑引入 event bus 或 frame-based update queue。例如：

```text
Document emits changes
  -> UpdateQueue
  -> end of frame
  -> Scene update
  -> Presentation update
  -> Renderer update
```

这会更适合复杂 scene、renderer 和 dependency graph。

## Scene Layer

`Scene` 当前以 `ElementId` 为 key 保存 `SceneObject`。

当前 `SceneObject`：

```cpp
struct SceneObject {
    ElementId elementId;
    GeometryTypes::RTransform localTransform;
    GeometryTypes::RTransform worldTransform;
    Bnd_Box boundingBox;
};
```

当前还没有 parent-child hierarchy，所以：

```text
worldTransform = localTransform
```

`Scene` 提供 picking candidate 查询：

```cpp
std::vector<ElementId> GetPickCandidates() const;
```

这个命名是刻意的。它不是“已选对象”，而是“当前可以参与 pick 的候选对象”。后续可以在这里过滤：

- invisible object
- locked object
- non-selectable helper object
- hidden layer object

当前版本先返回全部有效 `ElementId`。

## Interaction System

当前 interaction system 主要包含：

- `InteractionManager`
- `InteractionContext`
- `InteractionHandler`
- `NavigationHandler`
- `SelectionHandler`
- `CreateElementTool`
- `ViewController`
- `CoordinateResolver`
- `ScenePicker`

`CadView` 接收 Qt mouse、wheel、keyboard events，然后转发给 `InteractionManager`。

当前事件结构：

```text
CadView Qt event
  -> InteractionManager
  -> global handlers
  -> active handler
```

`NavigationHandler` 属于 global handler，负责 view navigation。`SelectionHandler` 和 `CreateElementTool` 属于 active handler，用于当前模式下的交互行为。

这种结构可以继续扩展出更多 tool：

- `MoveElementTool`
- `RotateElementTool`
- `SketchLineTool`
- `MeasureTool`
- `TransformTool`

## Selection And Picking

当前最重要的阶段成果是：selection 已经开始脱离 OCCT AIS picking。

当前 custom picking flow：

```text
MouseRelease
  -> GeomCalculator::GetMouseScreenRay
  -> ScenePicker::Pick
  -> Scene::GetPickCandidates
  -> GeomCalculator::RayIntersectBox
  -> nearest ElementId
  -> SelectionManager::SetSelected
  -> AIS selection state updated for temporary visual highlight
```

也就是说，当前 selected object 的来源已经不是：

```text
AIS_InteractiveContext::MoveTo
AIS_InteractiveContext::SelectedInteractive
```

而是 miniCad 自己的：

```text
Ray
Scene
AABB
ElementId
```

OCCT AIS 现在只负责临时高亮显示。selection truth 已经转移到 `SelectionManager`。

这是一个重要边界：后续 BVH、triangle picking、segment picking、box selection 都可以沿着这条链路继续扩展。

## Preview Flow

`CreateElementTool` 激活时会开始 preview。鼠标移动时，`CoordinateResolver` 把 screen position 转换到当前 work plane 上，然后更新 preview transform。

当前创建预览流程：

```text
Tool activation
  -> PreviewManager::BeginElementPreview
  -> PreviewAdaptor displays temporary AIS object

MouseMove
  -> CoordinateResolver::ScreenToWorkPlane
  -> LocalTransform update
  -> PreviewManager::UpdateElementPreview
  -> PreviewAdaptor::UpdateTransform

MousePress
  -> CoordinateResolver::ScreenToWorkPlane
  -> CadController::CreateElement
  -> CreateElementCommand
  -> Document
```

preview object 不进入 `Document`。它只是临时 presentation state。

当前设计的好处是：真正创建对象仍然走 command/document 流程，preview 不污染模型数据。

## View And Presentation

`CadView` 当前负责 OCCT viewer、view 和 interactive context 的初始化，同时也是 Qt widget event source。

`ViewAdaptor` 负责把 `ElementId` 映射到 AIS objects。映射关系由 `ViewObjectRegistry` 保存。

当前显示流程：

```text
Document change
  -> ViewAdaptor
  -> Element::BuildShape
  -> AIS_Shape
  -> ViewObjectRegistry registration
  -> AIS display/update/remove
```

`ReferenceOverlay` 当前用 AIS primitives 显示 grid 和 axes。这是临时方案。等 custom OpenGL renderer 接入后，grid、axis、helper overlay 应该迁移到 renderer-native drawing。

## Geometry Utilities

`GeomCalculator` 当前包含：

- ray-plane intersection
- ray-AABB intersection
- mouse screen ray generation

当前 picking 已经使用 `RayIntersectBox`。

AABB 当前使用 OCCT 的 `Bnd_Box`。这在过渡阶段没问题，因为当前 shape、AIS、bounding box 仍和 OCCT 绑定较多。

后续如果希望进一步降低 OCCT 耦合，可以引入自定义：

```cpp
struct Bounds3D {
    Point3D min;
    Point3D max;
};
```

然后在 Scene/BVH/Picking 内部优先使用 `Bounds3D`。

## ElementMesh Status

当前已有 `Data/ElementMesh` 的轻量结构：

```cpp
struct MeshVertex {
    GeometryTypes::Point3D position;
    GeometryTypes::Vector3D normal;
};

struct TriangleIndex {
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
};

struct SegmentIndex {
    uint32_t v0;
    uint32_t v1;
};

struct ElementMesh {
    std::vector<MeshVertex> vertices;
    std::vector<TriangleIndex> triangles;
    std::vector<SegmentIndex> segments;
};
```

这个方向是对的，但当前还没有完全接入主流程。

CAD 里只做 triangle mesh 不够。原因是 CAD picking 不只需要选面，也需要选：

- edge
- line
- axis
- sketch segment
- control handle
- helper/reference geometry

所以 `ElementMesh` 里同时保留：

- `triangles` for face picking and future rendering
- `segments` for edge/line picking

后续要做 precise picking，应该先接入 `ElementMesh`，再考虑 BVH。

## 为什么不应该立刻上 BVH

当前已经完成 object-level AABB picking，但还没有确定 primitive-level data 的完整来源。

如果现在直接写 BVH，会遇到一个问题：BVH 里到底放什么？

可选层级应该是：

```text
Object-level BVH
  stores SceneObject bounding boxes
  returns candidate ElementId

Primitive-level BVH
  stores triangles / segments / points
  returns exact primitive hit
```

因此推荐顺序是：

1. 先稳定 object-level picking。
2. 再让 `Element` 或 geometry builder 生成 `ElementMesh`。
3. 再做 ray-triangle / ray-segment precise picking。
4. 最后再把 linear traversal 替换成 BVH。

这样 BVH 会加在正确的数据结构上，而不是提前绑定到一个临时结构。

## 当前技术债

当前已知需要后续处理的问题：

- `AppContext` 正在变成较大的 composition root。
- `DocumentObserver` 同时更新 `Scene` 和 `ViewAdaptor`，后续可能需要 event queue。
- OCCT types 仍出现在 core-facing structures 中，例如 `Bnd_Box` 和 `gp_Trsf`。
- `ViewAdaptor` 仍直接从 `Element::BuildShape()` 创建 AIS object。
- 当前 picking 只做到 object-level AABB，精度较粗。
- selection visual highlight 仍依赖 AIS selection state。
- `ElementMesh` 已存在，但还没有由 Element 生成，也没有被 picking/rendering 消费。

这些问题当前不是阻塞项，但应该在后续阶段逐步收敛。

## 推荐下一阶段路线

下一阶段建议按这个顺序推进：

1. Stabilize object-level picking
   - 空白点击清空 selection。
   - 多对象重叠时选择最近对象。
   - transform update 后 bounding box 正确同步。
   - undo/redo 后 Scene 状态正确。

2. Introduce PickGeometry / ElementMesh generation
   - 先支持 `BoxElement`。
   - 生成 triangles 和 segments。
   - 再支持 `CylinderElement`。

3. Add precise primitive picking
   - ray vs triangle
   - ray vs segment with tolerance
   - `PickResult` 扩展为包含 primitive type、primitive index、hit point、distance。

4. Introduce acceleration structures
   - object-level BVH over `SceneObject::boundingBox`
   - primitive-level BVH over triangles and segments

5. Prepare custom OpenGL renderer
   - 将 `ElementMesh` 转成 OpenGL buffers。
   - 将 grid/axis/reference overlay 从 AIS 迁移到 renderer-native drawing。
   - 逐步削弱 `ViewAdaptor` 对业务显示逻辑的控制。

## 当前阶段结论

当前架构总体是合理的，并且已经完成一个关键过渡：

```text
selection source
  from OCCT AIS picking
  to miniCad Scene + Ray + AABB picking
```

这意味着项目已经不再只是一个 OCCT viewer wrapper，而是开始形成自己的 CAD runtime architecture。

后续开发时需要继续保持边界清晰：

```text
Document: persistent CAD data
Scene: runtime spatial cache
ScenePicker: picking algorithm
SelectionManager: selected state
ViewAdaptor: temporary OCCT presentation
ElementMesh: future picking/rendering geometry
```

只要这个边界不被打乱，后续接入 precise picking、BVH、dependency graph 和 custom OpenGL renderer 都会比较顺。
