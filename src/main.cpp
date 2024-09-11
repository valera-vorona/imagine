#include "Model.h"
#include "device.h"
#include "misc.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "FileBrowser.h"

#include <iostream>

/* consts */
const int         DEFAULT_WINDOW_WIDTH  = 1600;
const int         DEFAULT_WINDOW_HEIGHT = 1000;

const std::string IMAGINE_DIR           = ".imagine";
const std::string IMAGINE_CONFIG        = "imagine.conf";

int main(int argc, char *argv[])
{
    /* Platform */
    static GLFWwindow *win;
    int width = 0, height = 0;
    int display_width = 0, display_height = 0;

    /* GUI */
    struct device device;
    struct nk_context ctx;
    struct nk_font *font;
    struct nk_font_atlas atlas;

    std::string config_file = get_home_dir();

    if (config_file.empty()) {
        //TODO: Home dir is not found, try to run without config somehow
    } else {
        config_file = std::filesystem::path(config_file) / IMAGINE_DIR;
        if (!std::filesystem::exists(config_file)) {
            std::filesystem::create_directory(config_file);
        }

        config_file = std::filesystem::path(config_file) / IMAGINE_CONFIG;
        if (!std::filesystem::exists(config_file)) {
            std::ofstream{ config_file };
        }
    }

    std::fstream config_stream{ config_file };
    nlohmann::json config = nlohmann::json::parse(config_stream, nullptr, false, true);
    config_stream.close();

    if (config.is_discarded()) {
        width = DEFAULT_WINDOW_WIDTH;
        height = DEFAULT_WINDOW_HEIGHT;
    } else {
        try {
            width  = config["width"].get<int>();
        } catch (nlohmann::json::basic_json::exception &e) {
            width = DEFAULT_WINDOW_WIDTH;
        }

        try {
            height  = config["height"].get<int>();
        } catch (nlohmann::json::basic_json::exception &e) {
            height = DEFAULT_WINDOW_HEIGHT;
        }
    }

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    win = glfwCreateWindow(width, height, "Imagine", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetCharCallback(win, text_input);
    glfwSetKeyCallback(win, key_input);
    glfwSetCursorPosCallback(win, cursor_position_input);
    glfwSetMouseButtonCallback(win, mouse_button_input);
    glfwSetScrollCallback(win, scroll_input);

    /* OpenGL */
    glViewport(0, 0, width, height);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    {/* GUI */
    device_init(&device);
    {const void *image; int w, h;
    const char *font_path = (argc > 1) ? argv[1]: 0;

    struct nk_font_config cfg = nk_font_config(14);
    cfg.range = nk_font_cyrillic_glyph_ranges();

    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    if (font_path) font = nk_font_atlas_add_from_file(&atlas, font_path, 24.0f, &cfg);
    else font = nk_font_atlas_add_default(&atlas, 24.0f, &cfg);
    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    device_upload_atlas(&device, image, w, h);
    nk_font_atlas_end(&atlas, nk_handle_id((int)device.font_tex), &device.tex_null);
    }

    nk_init_default(&ctx, &font->handle);}

    /* icons */
    glEnable(GL_TEXTURE_2D);

    //TODO: I'm not sure if I shell provide here width/height, or display_width/display_height
    // but on my device they are the same
    glfwGetWindowSize(win, &width, &height);
    Model model(config_file, win, &ctx, width, height);
    model.add_browser(std::make_shared<FileBrowser>());

    glfwSetWindowUserPointer(win, &model);

    while (!glfwWindowShouldClose(win))
    {
        auto start = glfwGetTime();

        /* High DPI displays */
        struct nk_vec2 scale;
        glfwGetWindowSize(win, &width, &height);
        glfwGetFramebufferSize(win, &display_width, &display_height);
        scale.x = (float)display_width/(float)width;
        scale.y = (float)display_height/(float)height;

        //TODO: I'm not sure if I shell provide here width/height, or display_width/display_height
        // but on my device they are the same
        model.set_size(width, height);

        if (model.what_showing() == Showing::VIDEO) {
            auto fps = model.get_video_fps();
            if (fps > 0.) {
                auto timeout_sec = 1. / fps - (glfwGetTime() - start);
                if (timeout_sec > 0.) {
                    glfwWaitEventsTimeout(timeout_sec);
                }

                auto timeout_ms = std::chrono::milliseconds(int(1000. / fps - ((glfwGetTime() - start)) * 1000.));
                if (timeout_ms > std::chrono::milliseconds(0)) {
                    std::this_thread::sleep_for(timeout_ms);
                }

                model.draw();
            }
        } else {
            glfwWaitEvents();
            model.draw();
        }


        /* Draw */
        glViewport(0, 0, display_width, display_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        device_draw(&device, &ctx, width, height, scale, NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }

    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    device_shutdown(&device);
    glfwTerminate();
    return 0;
}

