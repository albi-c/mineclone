#include "material.hpp"

int MATERIAL_PROPERTIES[Material::_size()];

static const MaterialPropertiesInitializer material_properties_initializer;

MaterialPropertiesInitializer::MaterialPropertiesInitializer() {
    MATERIAL_PROPERTIES[Material::AIR] = MaterialProperty::TRANSPARENT;
    MATERIAL_PROPERTIES[Material::LEAVES] = MaterialProperty::TRANSPARENT;
    MATERIAL_PROPERTIES[Material::GRASS] = MaterialProperty::TRANSPARENT | MaterialProperty::PLANT;
    MATERIAL_PROPERTIES[Material::FLOWER] = MaterialProperty::TRANSPARENT | MaterialProperty::PLANT;
    MATERIAL_PROPERTIES[Material::DEAD_BUSH] = MaterialProperty::TRANSPARENT | MaterialProperty::PLANT;
}
