# EUI

[中文说明](readme.zh-CN.md)

EUI is a lightweight, header-only C++ UI toolkit focused on efficiency.
The core API (`include/EUI.h`) only produces draw commands and does not depend on GLFW/OpenGL.
An optional GLFW + OpenGL demo runtime is available behind `EUI_ENABLE_GLFW_OPENGL_BACKEND`.

## Implemented Features (v0.1.0)

- Header-only core target: `EUI::eui`
- Theme controls:
  - `ThemeMode` (`Light` / `Dark`)
  - Primary color (`set_primary_color`)
  - Corner radius (`set_corner_radius`)
- Layout primitives:
  - `begin_panel` / `end_panel`
  - `begin_card` / `end_card`
  - `begin_row` / `end_row`
  - `spacer`
- Widgets:
  - `label`
  - `button` (`Primary`, `Secondary`, `Ghost`)
  - `tab`
  - `slider_float` (drag + right-click numeric edit)
  - `input_float`
  - `progress`
  - `begin_dropdown` / `end_dropdown`
- Rendering output:
  - `end_frame()` returns `std::vector<DrawCommand>`
  - `text_arena()` returns text storage used by text draw commands
- Optional demo runtime (`eui::demo::run`):
  - `AppOptions` (`vsync`, `continuous_render`, `idle_wait_seconds`, `max_fps`, window size/title)
  - `FrameContext` (`ui`, framebuffer size, `dt`, `request_next_frame()`)

## Repository Layout

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

## Build

Recommended generator: `Ninja` (especially on Windows).

### 1) Build core only (no GLFW required)

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=OFF
cmake --build build
```

This builds:
- `EUI::eui` (interface target)
- `eui_header_check`

### 2) Build demo (GLFW + OpenGL)

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=ON
cmake --build build
```

`eui_demo` is created only when both OpenGL and GLFW are available.

CMake behavior when examples are enabled:
- Tries local GLFW first (`find_package(glfw3)`).
- If GLFW is missing and `EUI_FETCH_GLFW_FROM_GIT=ON`, uses `FetchContent` from GitHub.
- Resolves OpenGL as:
  - `OpenGL::GL` when found
  - `opengl32` fallback on Windows
  - `OpenGL` framework fallback on macOS

Important CMake options:

```bash
-DEUI_BUILD_EXAMPLES=ON|OFF
-DEUI_STRICT_WARNINGS=ON|OFF
-DEUI_FETCH_GLFW_FROM_GIT=ON|OFF
-DEUI_GLFW_GIT_TAG=3.4
```

If network or Git access is restricted:

```bash
cmake -S . -B build -G Ninja -DEUI_BUILD_EXAMPLES=ON -DEUI_FETCH_GLFW_FROM_GIT=OFF
```

Install GLFW locally first, then rerun CMake.

## Minimal Core Usage

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

Use `commands` + `text_arena` in your own renderer (OpenGL/Vulkan/DirectX/etc).

## Optional Demo Runtime Example

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

            // Call this when animation is needed in event-driven mode.
            frame.request_next_frame();
        },
        options
    );
}
```

## Notes

- `index.html` is a visual prototype/reference and is not part of the C++ build.
- Current test target (`eui_header_check`) is a compile check, not a full behavior test suite.
