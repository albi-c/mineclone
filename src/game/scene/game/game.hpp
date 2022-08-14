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

#include "entity/player.hpp"

#include "graphics/render/renderer.hpp"

#include "game/scene/scene.hpp"
#include "graphics/gui/imgui.hpp"

namespace game {
    struct SceneGameChunk {
        std::shared_ptr<Chunk> chunk;
        std::shared_ptr<Mesh> mesh;

        SceneGameChunk(SceneGameChunk* other)
            : chunk(other->chunk), mesh(other->mesh) {}
        SceneGameChunk(std::shared_ptr<Chunk> chunk, std::shared_ptr<Mesh> mesh = nullptr)
            : chunk(chunk), mesh(mesh) {}
    };
    struct SceneGameData {
        Player player;

        std::map<std::pair<int, int>, std::shared_ptr<SceneGameChunk>> chunks;

        std::map<std::string, std::shared_ptr<Mesh>> gui_meshes;
    };

    struct SceneGameResources {
        std::shared_ptr<TextureArray> block_textures;
        std::map<std::string, std::shared_ptr<Texture>> block_textures_map;
        std::shared_ptr<Shader> block_shader;

        std::shared_ptr<Shader> gui_shader;

        SceneGameResources()
            : block_textures(ResourceLoader::texture_array("minecraft", true)),
              block_textures_map({{"textureArray", block_textures}}),
              block_shader(new Shader(BuiltinShader::BLOCK)),

              gui_shader(new Shader(BuiltinShader::GUI))
        {
            block_textures->generate();
        }
    };

    class SceneGame;
    struct SceneGameHandlers {
        void listen();
        void process();

        static inline SceneGame* game;
        static void event_mouse_move_handler(const EventMouseMove& e);
        static void event_mouse_click_handler(const EventMouseClick& e);

        FunctionEventQueue<EventMouseMove> mouse_move_event_queue = FunctionEventQueue<EventMouseMove>(SceneGameHandlers::event_mouse_move_handler);
        FunctionEventQueue<EventMouseClick> mouse_click_event_queue = FunctionEventQueue<EventMouseClick>(SceneGameHandlers::event_mouse_click_handler);
    };

    class SceneGame : public Scene {
    public:
        SceneGame() {}
        SceneGame(SceneGame* other) {}

        void init();

        void update(float dt);
        void render();

    private:
        SceneGameData d;
        SceneGameResources r;
        SceneGameHandlers handlers;

        friend struct SceneGameHandlers;
        static void on_mouse_move(const EventMouseMove& e);
        static void on_mouse_click(const EventMouseClick& e);
    };
};

