#pragma once

#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <utility>
#include <thread>
#include <utility>
#include <mutex>

#include "resources/loader.hpp"

#include "event/event.hpp"
#include "window/event.hpp"
#include "game/event.hpp"

#include "util/ray.hpp"
#include "util/string.hpp"

#include "world/world.hpp"
#include "world/material.hpp"

#include "entity/player.hpp"

#include "graphics/render/renderer.hpp"
#include "graphics/mesh/mesh_group.hpp"

#include "game/options.hpp"
#include "game/event.hpp"
#include "game/scene/scene.hpp"
#include "graphics/gui/imgui.hpp"

namespace game {
    struct SceneGameData {
        Player player;

        std::shared_ptr<World> world;
        int world_seed;

        std::shared_ptr<MeshGroup<std::pair<int, int>>> chunk_mesh_group;

        std::map<std::string, std::shared_ptr<Mesh>> gui_meshes;

        SceneGameData() {}
    };

    struct SceneGameResources {
        std::shared_ptr<TextureArray> block_textures;
        std::map<std::string, std::shared_ptr<Texture>> block_textures_map;
        std::shared_ptr<Shader> block_shader;

        std::shared_ptr<Texture> crosshair_texture;
        std::map<std::string, std::shared_ptr<Texture>> crosshair_textures_map;
        std::shared_ptr<Shader> crosshair_shader;
        
        SceneGameResources()
            : block_textures(ResourceLoader::texture_array("minecraft", true)),
              block_textures_map({{"textureArray", block_textures}}),
              block_shader(ResourceLoader::shader("block")),

              crosshair_texture(ResourceLoader::texture("crosshair")),
              crosshair_textures_map({{"texture0", crosshair_texture}}),
              crosshair_shader(ResourceLoader::shader("gui"))
        {
            block_textures->generate();
        }
    };

    class SceneGame;
    struct SceneGameHandlers {
        void listen();
        void process();
        void clear();

        static inline SceneGame* game;
        static void event_window_resize_handler(const EventFramebufferResize& e);
        static void event_mouse_move_handler(const EventMouseMove& e);
        static void event_mouse_click_handler(const EventMouseClick& e);
        static void event_key_press_handler(const EventKeyPress& e);
        static void event_option_change_handler(const EventOptionChange& e);
        static void event_chunk_load_handler(const EventChunkLoad& e);
        static void event_chunk_unload_handler(const EventChunkUnload& e);

        FunctionEventQueue<EventFramebufferResize> window_resize_event_handler = \
            FunctionEventQueue<EventFramebufferResize>(SceneGameHandlers::event_window_resize_handler);
        FunctionEventQueue<EventMouseMove> mouse_move_event_queue = FunctionEventQueue<EventMouseMove>(SceneGameHandlers::event_mouse_move_handler);
        FunctionEventQueue<EventMouseClick> mouse_click_event_queue = FunctionEventQueue<EventMouseClick>(SceneGameHandlers::event_mouse_click_handler);
        FunctionEventQueue<EventKeyPress> key_press_event_queue = FunctionEventQueue<EventKeyPress>(SceneGameHandlers::event_key_press_handler);
        FunctionEventQueue<EventOptionChange> option_change_event_queue = \
            FunctionEventQueue<EventOptionChange>(SceneGameHandlers::event_option_change_handler);
        FunctionEventQueue<EventChunkLoad> chunk_load_event_queue = FunctionEventQueue<EventChunkLoad>(SceneGameHandlers::event_chunk_load_handler);
        FunctionEventQueue<EventChunkUnload> chunk_unload_event_queue = \
            FunctionEventQueue<EventChunkUnload>(SceneGameHandlers::event_chunk_unload_handler);
    };

    class SceneGame : public Scene {
    public:
        SceneGame() {}
        SceneGame(SceneGame* other) {}

        void init();

        void enable();

        void update(float dt);
        void update_worker();
        void update_keyboard(float dt, const bool* pressed);
        void render();

        SceneFlags get_options();

    private:
        SceneGameData d;
        SceneGameResources r;
        SceneGameHandlers handlers;

        friend struct SceneGameHandlers;
        void on_window_resize(const EventFramebufferResize& e);
        void on_mouse_move(const EventMouseMove& e);
        void on_mouse_click(const EventMouseClick& e);
        void on_key_press(const EventKeyPress& e);
        void on_option_change(const EventOptionChange& e);
        void on_chunk_load(const EventChunkLoad& e);
        void on_chunk_unload(const EventChunkUnload& e);
    };
};

