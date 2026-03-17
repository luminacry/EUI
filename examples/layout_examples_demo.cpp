#ifndef EUI_ENABLE_GLFW_OPENGL_BACKEND
#define EUI_ENABLE_GLFW_OPENGL_BACKEND 1
#endif
#include "EUI.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

enum class SidebarPage {
    Dashboard,
    Projects,
    Tasks,
    Settings,
};

struct LayoutDemoState {
    bool dark_mode{false};
    SidebarPage page{SidebarPage::Dashboard};

    bool top_actions_open{false};
    bool auto_refresh{true};
    bool notifications_on{true};
    bool show_archived{false};

    std::string search_query{};

    float global_radius{12.0f};
    float custom_card_radius{20.0f};
    float ui_density{1.0f};
    float color_r{23.0f};
    float color_g{43.0f};
    float color_b{87.0f};
    int accent_preset{0};

    float refresh_progress{0.38f};
    int selected_project{0};
    int selected_task{0};

    bool task_review_done{false};
    bool task_sync_done{false};
    bool task_report_done{false};
};

static void draw_nav_item(eui::Context& ui, LayoutDemoState& state, float h, const char* label,
                          SidebarPage page) {
    const eui::ButtonStyle style = (state.page == page) ? eui::ButtonStyle::Primary : eui::ButtonStyle::Secondary;
    if (ui.button(label, style, h)) {
        state.page = page;
    }
}

static void draw_sidebar(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };
    const float item_h = dp(34.0f);

    ui.begin_card("", 0.0f, dp(10.0f));
    ui.label("Aino Corp", dp(26.0f), false);
    ui.spacer(dp(10.0f));

    ui.label("OVERVIEW", dp(11.0f), true);
    draw_nav_item(ui, state, item_h, "Dashboard", SidebarPage::Dashboard);
    draw_nav_item(ui, state, item_h, "Projects", SidebarPage::Projects);
    draw_nav_item(ui, state, item_h, "Tasks", SidebarPage::Tasks);

    ui.spacer(dp(6.0f));
    ui.label("SYSTEM", dp(11.0f), true);
    draw_nav_item(ui, state, item_h, "Settings", SidebarPage::Settings);

    ui.spacer(dp(8.0f));
    if (ui.button(state.dark_mode ? "Use Light" : "Use Dark", eui::ButtonStyle::Ghost, item_h)) {
        state.dark_mode = !state.dark_mode;
    }

    ui.spacer(dp(8.0f));
    ui.begin_card("Aino", 0.0f, dp(8.0f));
    ui.label("sudoevolve@gmail.com", dp(12.0f), true);
    ui.end_card();

    ui.end_card();
}

static void draw_topbar(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };
    const float item_h = dp(34.0f);

    ui.begin_card("", 0.0f, dp(10.0f));
    ui.begin_row(20, dp(8.0f));

    ui.set_next_item_span(7);
    ui.input_text("", state.search_query, item_h, "Search components, files...");

    ui.row_flex_spacer(4, item_h);
    if (ui.button(state.notifications_on ? "Bell" : "Muted", eui::ButtonStyle::Secondary, item_h)) {
        state.notifications_on = !state.notifications_on;
    }

    ui.set_next_item_span(2);
    ui.button("New Project", eui::ButtonStyle::Primary, item_h);
    ui.end_row();

    if (ui.begin_dropdown("Top Actions", state.top_actions_open, 0.0f, dp(8.0f))) {
        static const std::array<eui::Color, 6> kAccentPresets = {
            eui::rgba(0.09f, 0.17f, 0.34f, 1.0f),
            eui::rgba(0.30f, 0.74f, 0.77f, 1.0f),
            eui::rgba(0.95f, 0.58f, 0.25f, 1.0f),
            eui::rgba(0.56f, 0.43f, 0.92f, 1.0f),
            eui::rgba(0.30f, 0.79f, 0.46f, 1.0f),
            eui::rgba(0.93f, 0.29f, 0.48f, 1.0f),
        };

        ui.begin_row(2, dp(8.0f));
        if (ui.button("Reset Layout", eui::ButtonStyle::Secondary, item_h)) {
            state.global_radius = 12.0f;
            state.custom_card_radius = 20.0f;
            state.ui_density = 1.0f;
        }
        if (ui.button("Next Accent", eui::ButtonStyle::Primary, item_h)) {
            state.accent_preset = (state.accent_preset + 1) % static_cast<int>(kAccentPresets.size());
            const eui::Color color = kAccentPresets[static_cast<std::size_t>(state.accent_preset)];
            state.color_r = color.r * 255.0f;
            state.color_g = color.g * 255.0f;
            state.color_b = color.b * 255.0f;
        }
        ui.end_row();

        ui.begin_row(2, dp(8.0f));
        if (ui.tab("Auto Refresh", state.auto_refresh, item_h)) {
            state.auto_refresh = !state.auto_refresh;
        }
        if (ui.tab("Dark Theme", state.dark_mode, item_h)) {
            state.dark_mode = !state.dark_mode;
        }
        ui.end_row();

        ui.slider_float("UI Density", state.ui_density, 0.80f, 1.35f, 2, dp(36.0f));
        ui.end_dropdown();
    }

    ui.end_card();
}

static void draw_stats_row(eui::Context& ui, float scale, float progress) {
    const auto dp = [scale](float value) { return value * scale; };

    auto stat_card = [&](const char* title, const char* value, const char* note) {
        ui.begin_card("", dp(118.0f), dp(10.0f));
        ui.label(title, dp(12.0f), true);
        ui.label(value, dp(20.0f), false);
        ui.label(note, dp(12.0f), true);
        ui.progress("", progress, dp(6.0f));
        ui.end_card();
    };

    ui.begin_row(4, dp(8.0f));
    stat_card("Total Revenue", "$45,231.89", "+20.1% from last month");
    stat_card("Active Projects", "+12", "+2 since yesterday");
    stat_card("Tasks Completed", "142", "+19% from last week");
    stat_card("Active Now", "+573", "+201 since last hour");
    ui.end_row();
}

static void draw_recent_projects_card(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };
    static const std::array<const char*, 14> kProjects = {
        "E-commerce Redesign",
        "Mobile App API",
        "Marketing Campaign",
        "Design System V2",
        "Billing Portal",
        "Automation Suite",
        "Growth Dashboard",
        "Payments Refactor",
        "Customer Portal",
        "Data Sync Service",
        "QA Benchmark",
        "Mobile Push Revamp",
        "Partner Integrations",
        "AI Assistant Ops",
    };

    state.selected_project = std::clamp(state.selected_project, 0, static_cast<int>(kProjects.size()) - 1);

    ui.begin_card("Recent Projects", dp(360.0f), dp(10.0f));
    ui.label("You have active projects this month.", dp(12.0f), true);

    if (ui.begin_scroll_area("RECENT_PROJECTS_SCROLL", dp(250.0f))) {
        for (int i = 0; i < static_cast<int>(kProjects.size()); ++i) {
            const eui::ButtonStyle style =
                (state.selected_project == i) ? eui::ButtonStyle::Primary : eui::ButtonStyle::Ghost;
            if (ui.button(kProjects[static_cast<std::size_t>(i)], style, dp(34.0f))) {
                state.selected_project = i;
            }
        }
        ui.end_scroll_area();
    }

    ui.end_card();
}

static void draw_task_side_cards(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };
    const float item_h = dp(32.0f);

    ui.begin_card("", dp(360.0f), dp(10.0f));

    ui.begin_card("Upcoming Tasks", dp(180.0f), dp(10.0f));
    if (ui.tab("Review design system", state.task_review_done, item_h)) {
        state.task_review_done = !state.task_review_done;
    }
    if (ui.tab("Weekly sync with engineering", state.task_sync_done, item_h)) {
        state.task_sync_done = !state.task_sync_done;
    }
    if (ui.tab("Prepare monthly report", state.task_report_done, item_h)) {
        state.task_report_done = !state.task_report_done;
    }
    ui.button("Add Task", eui::ButtonStyle::Secondary, item_h);
    ui.end_card();

    ui.spacer(dp(8.0f));
    ui.begin_card("Upgrade to Pro", dp(140.0f), dp(10.0f), state.custom_card_radius * scale);
    ui.label("Custom radius card.", dp(12.0f), true);
    ui.label("Default cards use global radius * 0.5", dp(12.0f), true);
    ui.button("Upgrade Now", eui::ButtonStyle::Primary, item_h);
    ui.end_card();

    ui.end_card();
}

static void draw_dashboard_page(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };

    ui.label("Welcome back, Aino", dp(46.0f), false);
    ui.label("Here is what's happening with your projects today.", dp(16.0f), true);
    ui.spacer(dp(6.0f));

    draw_stats_row(ui, scale, state.refresh_progress);
    ui.spacer(dp(8.0f));

    ui.begin_row(3, dp(8.0f));
    ui.set_next_item_span(2);
    draw_recent_projects_card(ui, state, scale);
    draw_task_side_cards(ui, state, scale);
    ui.end_row();
}

static void draw_projects_page(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };

    ui.label("Projects", dp(30.0f), false);
    ui.label("Switch projects from the left list and inspect details.", dp(13.0f), true);
    ui.spacer(dp(6.0f));

    ui.begin_row(2, dp(8.0f));
    draw_recent_projects_card(ui, state, scale);

    ui.begin_card("Project Detail", dp(360.0f), dp(10.0f));
    ui.label("Selected Project", dp(12.0f), true);
    ui.label("Status: In Progress", dp(14.0f), false);
    ui.label("Owner: Aino", dp(12.0f), true);
    ui.progress("Delivery", state.refresh_progress, dp(8.0f));
    ui.begin_row(2, dp(8.0f));
    ui.button("Open", eui::ButtonStyle::Primary, dp(32.0f));
    ui.button("Archive", eui::ButtonStyle::Ghost, dp(32.0f));
    ui.end_row();
    ui.end_card();

    ui.end_row();
}

static void draw_tasks_page(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };

    ui.label("Tasks", dp(30.0f), false);
    ui.label("Toggle tasks directly from this board.", dp(13.0f), true);
    ui.spacer(dp(6.0f));

    ui.begin_waterfall(2, dp(8.0f));

    ui.begin_card("Today", dp(220.0f), dp(10.0f));
    if (ui.tab("Review design system", state.task_review_done, dp(34.0f))) {
        state.task_review_done = !state.task_review_done;
    }
    if (ui.tab("Weekly sync", state.task_sync_done, dp(34.0f))) {
        state.task_sync_done = !state.task_sync_done;
    }
    ui.end_card();

    ui.begin_card("Upcoming", dp(220.0f), dp(10.0f));
    if (ui.tab("Prepare report", state.task_report_done, dp(34.0f))) {
        state.task_report_done = !state.task_report_done;
    }
    ui.button("Create Task", eui::ButtonStyle::Secondary, dp(34.0f));
    ui.end_card();

    ui.end_waterfall();
}

static void draw_settings_page(eui::Context& ui, LayoutDemoState& state, float scale) {
    const auto dp = [scale](float value) { return value * scale; };

    ui.label("Settings", dp(30.0f), false);
    ui.label("Card radius defaults to global radius * 0.5. You can still override per card.", dp(13.0f), true);
    ui.spacer(dp(6.0f));

    ui.begin_card("Theme + Radius", 0.0f, dp(10.0f));
    ui.slider_float("Global Radius", state.global_radius, 8.0f, 28.0f, 0, dp(36.0f));
    ui.slider_float("Custom Card Radius", state.custom_card_radius, 8.0f, 36.0f, 0, dp(36.0f));
    ui.end_card();

    ui.begin_card("Primary Color RGB", 0.0f, dp(10.0f));
    ui.slider_float("R", state.color_r, 0.0f, 255.0f, 0, dp(36.0f));
    ui.slider_float("G", state.color_g, 0.0f, 255.0f, 0, dp(36.0f));
    ui.slider_float("B", state.color_b, 0.0f, 255.0f, 0, dp(36.0f));
    const float r = std::clamp(state.color_r / 255.0f, 0.0f, 1.0f);
    const float g = std::clamp(state.color_g / 255.0f, 0.0f, 1.0f);
    const float b = std::clamp(state.color_b / 255.0f, 0.0f, 1.0f);
    char rgba_text[80];
    std::snprintf(rgba_text, sizeof(rgba_text), "rgba(%.2ff, %.2ff, %.2ff, 1.0f)", r, g, b);

    ui.begin_row(12, dp(8.0f));
    ui.set_next_item_span(2);
    ui.label("RGBA", dp(13.0f), true, dp(34.0f));
    ui.row_flex_spacer(4, dp(34.0f));
    ui.set_next_item_span(3);
    ui.input_readonly("", rgba_text, dp(34.0f), true, 1.0f, false);
    ui.set_next_item_span(1);
    ui.button(" ", eui::ButtonStyle::Primary, dp(34.0f));
    ui.end_row();
    ui.end_card();

    ui.begin_row(2, dp(8.0f));
    ui.begin_card("Default Card", dp(110.0f), dp(10.0f));
    ui.label("Uses global radius * 0.5", dp(12.0f), true);
    ui.end_card();

    ui.begin_card("Custom Card", dp(110.0f), dp(10.0f), state.custom_card_radius * scale);
    ui.label("Uses explicit radius", dp(12.0f), true);
    ui.end_card();
    ui.end_row();
}

int main() {
    LayoutDemoState state{};

    eui::demo::AppOptions options{};
    options.title = "EUI Layout Examples";
    options.width = 1080;
    options.height = 820;
    options.vsync = true;
    options.continuous_render = false;
    options.max_fps = 144.0;
    options.text_font_family = "Segoe UI";
    options.text_font_weight = 600;
    options.icon_font_family = "Segoe MDL2 Assets";
    options.enable_icon_font_fallback = true;

    return eui::demo::run(
        [&](eui::demo::FrameContext frame) {
            auto& ui = frame.ui;
            const float dpi_scale = std::max(1.0f, frame.dpi_scale);
            const float layout_scale = dpi_scale * std::clamp(state.ui_density, 0.80f, 1.35f);
            const auto dp = [layout_scale](float value) { return value * layout_scale; };

            if (state.auto_refresh) {
                state.refresh_progress += frame.dt * 0.20f;
                if (state.refresh_progress > 1.0f) {
                    state.refresh_progress = 0.0f;
                }
                frame.request_next_frame();
            }

            ui.set_theme_mode(state.dark_mode ? eui::ThemeMode::Dark : eui::ThemeMode::Light);
            ui.set_primary_color(eui::rgba(state.color_r / 255.0f, state.color_g / 255.0f,
                                           state.color_b / 255.0f, 1.0f));
            ui.set_corner_radius(state.global_radius * layout_scale);

            const float margin = dp(16.0f);
            const float sidebar_w = dp(260.0f);
            const float main_x = margin * 2.0f + sidebar_w;
            const float main_w = std::max(dp(320.0f), static_cast<float>(frame.framebuffer_w) - main_x - margin);

            ui.begin_panel("", margin, margin, sidebar_w, dp(8.0f), 0.0f);
            draw_sidebar(ui, state, layout_scale);
            ui.end_panel();

            ui.begin_panel("", main_x, margin, main_w, dp(8.0f), 0.0f);
            draw_topbar(ui, state, layout_scale);

            switch (state.page) {
                case SidebarPage::Dashboard:
                    draw_dashboard_page(ui, state, layout_scale);
                    break;
                case SidebarPage::Projects:
                    draw_projects_page(ui, state, layout_scale);
                    break;
                case SidebarPage::Tasks:
                    draw_tasks_page(ui, state, layout_scale);
                    break;
                case SidebarPage::Settings:
                    draw_settings_page(ui, state, layout_scale);
                    break;
                default:
                    break;
            }

            ui.end_panel();
        },
        options);
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main();
}
#endif
