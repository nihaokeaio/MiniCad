# miniCad Technical Architecture Summary

Date: 2026-06-10

This document records the current architecture of miniCad at the end of the current development phase. It is a stage summary, not a final design specification. Some modules are intentionally lightweight because the project is still transitioning from an OCCT-driven prototype toward a custom CAD interaction and rendering pipeline.

## Project Goal

miniCad is a learning-oriented CAD prototype. The current implementation uses Qt for UI and event dispatch, OpenCASCADE for geometry construction and temporary display, and a growing custom control/scene/picking layer for CAD interaction logic.

The long-term direction is:

- Qt handles UI, widgets, and mouse/keyboard event delivery.
- OCCT provides geometry modeling utilities and data types where useful.
- miniCad owns document data, commands, scene state, picking, BVH, mesh data, and eventually OpenGL rendering.
- OCCT AIS remains a temporary presentation backend until the custom renderer is ready.

## Current Module Layout

```text
AppContext
  Owns and wires core application services.

Data
  Document, Element, properties, element factory, element mesh data.

Controller
  CadController, Command system, Interaction system, Preview manager.

Scene
  Runtime scene cache derived from Document.

Presentation
  OCCT AIS display adaptor, preview adaptor, reference overlay, selection state.

Rendering
  CadView, OCCT viewer/widget integration.

Utils
  Messages, geometry calculations, IDs, macros, helper utilities.
```

## Ownership And Data Authority

`Document` is the authoritative data model. It owns all `Element` instances and is responsible for element registration, lookup, and change notification.

`Scene` is not another source of truth. It is a runtime cache derived from `Document`, currently storing spatial information needed by interaction systems:

- `ElementId`
- local transform
- world transform
- bounding box

This distinction is important:

- `Document` answers "what objects exist and what are their properties?"
- `Scene` answers "what objects are currently available for spatial queries?"
- `Presentation` answers "how are those objects currently shown in the OCCT viewer?"

## Application Bootstrapping

`AppContext::Initialize()` wires the current system:

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

`AppContext` is currently a service container and composition root. It is acceptable at this stage, but it is growing. If construction complexity continues to increase, a later cleanup could split viewer setup, controller setup, and presentation setup into smaller factory functions.

## Element And Property System

`Element` is the base class for CAD objects. Current concrete elements include:

- `BoxElement`
- `CylinderElement`

Each element owns a `PropertySet`. The current important transform property is:

```text
LocalTransform
```

`LocalTransform` stores a `GeometryTypes::RTransform`, currently backed by OCCT transform types. Position is no longer stored as a separate property. This keeps translation, rotation, and scale under one conceptual transform path.

`Element::OnSetProperty()` returns an `ElementUpdateHint`, allowing the system to distinguish between:

- geometry update
- transform update
- property update
- full update

This is the first step toward dirty flags and a future dependency/update graph.

## Command System

The command layer is responsible for undoable operations.

Current commands:

- `CreateElementCommand`
- `ChangePropertyCommand`

`CadController` is the high-level control API used by UI and tools. It creates commands and submits them to `CommandManager`.

Current command flow:

```text
UI or Tool
  -> CadController
  -> CommandManager
  -> Command::Execute()
  -> Document mutation
  -> Document notification
```

This keeps UI code from directly mutating `Document`.

## Document Notification Flow

`Document` emits element change payloads through a callback registered by `DocumentObserver`.

Current flow:

```text
Document
  -> DocumentObserver
  -> Scene::AddOrUpdate / Remove
  -> ViewAdaptor::UpdateElement
```

`DocumentObserver` currently updates both runtime scene data and OCCT presentation. This is acceptable for the current stage because it keeps `Document` independent of both `Scene` and `ViewAdaptor`.

Later, this could become an event bus or frame-based update queue if update timing becomes more complex.

## Scene Layer

`Scene` currently stores `SceneObject` instances indexed by `ElementId`.

Current `SceneObject` data:

```cpp
struct SceneObject {
    ElementId elementId;
    GeometryTypes::RTransform localTransform;
    GeometryTypes::RTransform worldTransform;
    Bnd_Box boundingBox;
};
```

There is no hierarchy yet, so:

```text
worldTransform = localTransform
```

The scene provides pick candidates through:

```cpp
std::vector<ElementId> GetPickCandidates() const;
```

This name is intentional. These are not selected objects. They are objects that may participate in picking. Later, this function can filter invisible, locked, or non-selectable objects.

## Interaction System

The interaction system is centered around:

- `InteractionManager`
- `InteractionContext`
- `InteractionHandler`
- `NavigationHandler`
- `SelectionHandler`
- `CreateElementTool`
- `ViewController`
- `CoordinateResolver`
- `ScenePicker`

`InteractionManager` receives mouse, wheel, and keyboard events from `CadView`.

Global handlers currently process navigation. The active handler processes mode-specific behavior such as selection or element creation.

Current interaction structure:

```text
CadView Qt event
  -> InteractionManager
  -> global handlers
  -> active handler
```

## Selection And Picking

Selection has started moving away from OCCT AIS selection.

Current custom picking flow:

```text
MouseRelease
  -> GeomCalculator::GetMouseScreenRay
  -> ScenePicker::PickByBoundingBox
  -> Scene::GetPickCandidates
  -> GeomCalculator::RayIntersectBox
  -> nearest ElementId
  -> SelectionManager::SetSelected
  -> AIS selection state updated for temporary visual highlight
```

This is the current key architectural milestone. The selected object is now determined by miniCad's own scene and ray/AABB logic, not by `AIS_InteractiveContext::MoveTo()` or `SelectedInteractive()`.

OCCT AIS is still used to show selection highlight, but it is no longer the source of selection truth.

## Preview Flow

`CreateElementTool` begins preview when activated. Mouse movement is resolved onto the current work plane through `CoordinateResolver`, then preview transform is updated.

Current creation preview flow:

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

The preview object is not stored in `Document`. It is temporary presentation state.

## View And Presentation

`CadView` owns the OCCT viewer, view, and interactive context. It is currently the Qt widget that receives raw UI events.

`ViewAdaptor` maps `ElementId` to AIS objects through `ViewObjectRegistry`.

Current display flow:

```text
Document change
  -> ViewAdaptor
  -> Element::BuildShape
  -> AIS_Shape
  -> ViewObjectRegistry registration
  -> AIS display/update/remove
```

`ReferenceOverlay` currently displays grid and axes using AIS primitives. This is temporary. Once the custom OpenGL renderer exists, reference grid and axes should move to renderer-native drawing.

## Geometry Utilities

`GeomCalculator` currently contains:

- ray-plane intersection
- ray-AABB intersection
- mouse screen ray generation through OCCT view conversion

The ray-AABB path is already used by custom picking.

Current AABB is represented with OCCT `Bnd_Box`. This is acceptable for the current bridge stage. A future custom renderer/BVH layer may introduce a smaller `Bounds3D` type to avoid depending on OCCT in core acceleration structures.

## ElementMesh Status

`Data/ElementMesh` currently defines a lightweight mesh representation:

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

This is the correct direction, but it is not fully integrated yet.

The important design point is that CAD picking needs both triangles and segments:

- triangles for face picking
- segments for edge, polyline, axis, sketch, and helper picking

BVH should not be introduced before deciding what primitive data it will accelerate.

## Current Technical Debt

Known areas to revisit:

- `AppContext` is becoming a large composition root.
- `DocumentObserver` directly updates both `Scene` and `ViewAdaptor`; a queued update/event system may become useful later.
- OCCT types still exist in several core-facing structures, including transforms and bounding boxes.
- `ViewAdaptor` still builds AIS shapes directly from elements.
- Picking currently uses object-level AABB only, so precision is coarse.
- Selection visual feedback still depends on OCCT AIS selection state.
- `ElementMesh` exists but is not yet generated by elements or consumed by picking/rendering.

## Recommended Next Phase

The next phase should not jump directly to BVH. The better sequence is:

1. Stabilize object-level picking.
   - Verify empty click clears selection.
   - Verify nearest object wins when objects overlap.
   - Verify undo/redo and transform changes keep `SceneObject::boundingBox` correct.

2. Introduce pick geometry.
   - Generate `ElementMesh` for simple elements.
   - Start with box triangles and segments.
   - Add cylinder triangulation later.

3. Add precise picking.
   - ray vs triangle
   - ray vs segment with tolerance
   - return richer pick results: object, primitive type, primitive index, distance, hit point

4. Introduce acceleration structures.
   - object-level BVH over `SceneObject::boundingBox`
   - optional per-object primitive BVH over triangles and segments

5. Prepare custom rendering.
   - Convert `ElementMesh` into OpenGL buffers.
   - Move grid/axis/reference rendering away from AIS.
   - Gradually reduce `ViewAdaptor` responsibility.

## Current Architectural Direction

The current architecture is reasonable for the project's stated goal. It has crossed the important boundary where object selection is no longer controlled by OCCT's picking pipeline.

The main principle for the next iterations should be:

```text
Document owns CAD data.
Scene owns runtime spatial state.
Picker owns spatial queries.
SelectionManager owns selected IDs.
Presentation only displays current state.
```

Keeping these boundaries clear will make it much easier to add BVH, precise primitive picking, dependency updates, and custom OpenGL rendering without rewriting the entire application.
