#pragma once

#include "world/material.hpp"
#include "opengl.hpp"

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

    Block()
        : material(Material::AIR) {}
    Block(Material material)
        : material(material) {}
    
    operator Material() const {
        return material;
    }

    operator bool() const {
        return material != (Material)Material::AIR;
    }

    Block& operator=(const Block& other) {
        material = other.material;
        return *this;
    }
    Block& operator=(Material mat) {
        material = mat;
        return *this;
    }

    bool operator==(const Block& other) const {
        return material == other.material;
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
