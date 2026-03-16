# EUI（中文说明）

[English README](readme.md)

EUI 是一个轻量、头文件式、强调效率的 C++ UI 工具包。
核心 API（`include/EUI.h`）仅负责生成绘制命令，不依赖 GLFW/OpenGL。
可选的 GLFW + OpenGL 演示运行时通过 `EUI_ENABLE_GLFW_OPENGL_BACKEND` 启用。

## 已实现功能（v0.1.0）

- 头文件核心目标：`EUI::eui`
- 主题能力：
  - `ThemeMode`（`Light` / `Dark`）
  - 主色（`set_primary_color`）
  - 圆角（`set_corner_radius`）
- 布局原语：
  - `begin_panel` / `end_panel`
  - `begin_card` / `end_card`
  - `begin_row` / `end_row`
  - `spacer`
- 组件：
  - `label`
  - `button`（`Primary`、`Secondary`、`Ghost`）
  - `tab`
  - `slider_float`（拖动 + 右键数值直编）
  - `input_float`
  - `progress`
  - `begin_dropdown` / `end_dropdown`
- 渲染输出：
  - `end_frame()` 返回 `std::vector<DrawCommand>`
  - `text_arena()` 返回文本绘制命令使用的字符缓冲
- 可选 demo 运行时（`eui::demo::run`）：
  - `AppOptions`（`vsync`、`continuous_render`、`idle_wait_seconds`、`max_fps`、窗口大小与标题）
  - `FrameContext`（`ui`、framebuffer 尺寸、`dt`、`request_next_frame()`）

## 仓库结构

```text
EUI/
|- include/
|  `- EUI.h
|- examples/
|  `- basic_demo.cpp
|- tests/
|  `- header_check.cpp
|- CMakeLists.txt
|- index.html
|- readme.md
`- readme.zh-CN.md
```

## 构建

推荐使用 `Ninja` 生成器（尤其在 Windows 下）。

### 1）仅构建核心（不需要 GLFW）

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=OFF
cmake --build build
```

会生成：
- `EUI::eui`（interface target）
- `eui_header_check`

### 2）构建 demo（GLFW + OpenGL）

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=ON
cmake --build build
```

仅当 OpenGL 和 GLFW 都可用时才会生成 `eui_demo`。

开启 examples 后，CMake 行为如下：
- 先尝试本地 GLFW（`find_package(glfw3)`）。
- 若未找到且 `EUI_FETCH_GLFW_FROM_GIT=ON`，则通过 `FetchContent` 从 GitHub 拉取。
- OpenGL 目标解析顺序：
  - 优先 `OpenGL::GL`
  - Windows 回退 `opengl32`
  - macOS 回退 `OpenGL` framework

重要 CMake 选项：

```bash
-DEUI_BUILD_EXAMPLES=ON|OFF
-DEUI_STRICT_WARNINGS=ON|OFF
-DEUI_FETCH_GLFW_FROM_GIT=ON|OFF
-DEUI_GLFW_GIT_TAG=3.4
```

如果环境无法访问网络或 Git：

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=ON -DEUI_FETCH_GLFW_FROM_GIT=OFF
```

先本地安装 GLFW，再重新运行 CMake。

## 核心最小用法

```cpp
#include "EUI.h"

eui::Context ui;
eui::InputState input{};

float value = 0.5f;
bool advanced_open = false;

ui.begin_frame(1280.0f, 720.0f, input);
ui.begin_panel("Demo", 20.0f, 20.0f, 640.0f);

ui.begin_row(2, 8.0f);
ui.button("Run", eui::ButtonStyle::Primary);
ui.input_float("Value", value, 0.0f, 1.0f, 2);
ui.end_row();

if (ui.begin_dropdown("Advanced", advanced_open, 80.0f)) {
    ui.progress("Loading", 0.42f);
    ui.end_dropdown();
}

ui.end_panel();

const auto& commands = ui.end_frame();
const auto& text_arena = ui.text_arena();
```

将 `commands` 和 `text_arena` 接到你自己的渲染后端（OpenGL/Vulkan/DirectX 等）即可。

## 可选 Demo 运行时示例

```cpp
#define EUI_ENABLE_GLFW_OPENGL_BACKEND 1
#include "EUI.h"

int main() {
    eui::demo::AppOptions options{};
    options.width = 540;
    options.height = 960;
    options.vsync = true;
    options.continuous_render = false;
    options.max_fps = 240.0;

    return eui::demo::run(
        [&](eui::demo::FrameContext frame) {
            auto& ui = frame.ui;
            ui.set_theme_mode(eui::ThemeMode::Dark);

            ui.begin_panel("Demo", 20.0f, 20.0f, 320.0f);
            ui.label("Hello EUI");
            ui.end_panel();

            // 事件驱动模式下，如果需要动画，主动请求下一帧。
            frame.request_next_frame();
        },
        options
    );
}
```

## 说明

- `index.html` 是视觉原型/参考，不参与 C++ 构建。
- 当前测试目标（`eui_header_check`）主要是头文件编译检查，不是完整行为测试套件。
