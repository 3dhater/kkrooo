#ifndef __FORWARD_H__
#define __FORWARD_H__
class Gizmo;
class GeometryCreator;
class EventConsumer;
class ShortcutManager;
class ShaderSimple;
class ShaderPoint;
class ShaderLineModel;
class ShaderScene3DObjectDefault;
class ShaderScene3DObjectDefault_polymodeforlinerender;
class Cursor;
class PluginGUIWindow;
class RenderManager;
class Scene3DObject;
struct CursorRay;
enum class AppState_keyboard : u32;
enum class AppState_main : u32;
#ifdef KK_PLATFORM_WINDOWS
    struct IFileSaveDialog;
#endif
class Viewport;
class ViewportCamera;
class ViewportObject;
class Scene3D;
struct ColorTheme;

enum class ShortcutCommandCategory : u32;
enum class ShortcutCommand_General : u32;
enum class ShortcutCommand_Viewport : u32;
enum class ShortcutCommand_Camera : u32;
enum class ShortcutCommand_Edit : u32;
enum class DrawMode : u32;
struct ShortcutCommandDesc;
struct ShortcutCommandNode;
class Application;
#endif