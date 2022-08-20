#pragma once

#include "material.hpp"
#include "lib/opengl.hpp"

struct BlockPosition {
    int x, y, z;

    BlockPosition()
        : x(0), y(0), z(0) {}
    BlockPosition(int x, int y, int z)
        : x(x), y(y), z(z) {}
    
    BlockPosition operator+(BlockPosition other) const {
        return BlockPosition(x + other.x, y + other.y, z + other.z);
    }
};

struct Block {
    Material material = Material::AIR;
    int state = 0;

    Block()
        : material(Material::AIR), state(0) {}
    Block(Material material)
        : material(material), state(0) {}
    Block(Material material, int state)
        : material(material), state(state) {}
    
    operator Material() const {
        return material;
    }

    operator bool() const {
        return material != (Material)Material::AIR;
    }

    Block& operator=(const Block& other) {
        material = other.material;
        state = other.state;
        return *this;
    }
    Block& operator=(Material mat) {
        material = mat;
        state = 0;
        return *this;
    }

    bool operator==(const Block& other) const {
        return material == other.material && state == other.state;
    }

    bool transparent() const {
        return MATERIAL_PROPERTIES[material] & MaterialProperty::TRANSPARENT;
    }
    bool plant() const {
        return MATERIAL_PROPERTIES[material] & MaterialProperty::PLANT;
    }

    std::string face_texture(int face) {
        switch (material) {
            case Material::GRASS_BLOCK:
                if (face == 5)
                    return "GRASS_BLOCK_TOP";
                else if (face == 4)
                    return "DIRT";
                else
                    return "GRASS_BLOCK_SIDE";
            case Material::LOG:
                if (face >= 4)
                    return "LOG_TOP";
                else
                    return "LOG";
            default:
                return material._to_string();
        }
    }
};
