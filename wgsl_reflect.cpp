//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_reflect.h"

std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> WgslReflect::TypeInfo = {
        {"i32", {4, 4}},
        {"u32", {4, 4}},
        {"f32", {4, 4}},
        {"atomic", {4, 4}},
        {"vec2", {8, 8}},
        {"vec3", {16, 12}},
        {"vec4", {16, 16}},
        {"mat2x2", {8, 16}},
        {"mat3x2", {8, 24}},
        {"mat4x2", {8, 32}},
        {"mat2x3", {16, 32}},
        {"mat3x3", {16, 48}},
        {"mat4x3", {16, 64}},
        {"mat2x4", {16, 32}},
        {"mat3x4", {16, 48}},
        {"mat4x4", {16, 64}}
};


WgslReflect::WgslReflect(const std::string &code) {

}

void WgslReflect::initialize(const std::string &code) {

}

bool WgslReflect::isTextureVar(AST *node) {
    return false;
}

bool WgslReflect::isSamplerVar(AST *node) {
    return false;
}

bool WgslReflect::isUniformVar(AST *node) {
    return false;
}

void WgslReflect::getAlias(const std::string &name) {

}

void WgslReflect::getStruct(const std::string &name) {

}

void WgslReflect::getAttribute(AST *node, const std::string &name) {

}

void WgslReflect::getBindGroups() {

}

void WgslReflect::getUniformBufferInfo(AST *node) {

}

void WgslReflect::getTypeInfo(const std::string &type) {

}

void WgslReflect::_getInputs(const std::string &args, const std::string &inputs) {

}

void WgslReflect::_getInputInfo(AST *node) {

}

void WgslReflect::_parseInt(const std::string &s) {

}

void WgslReflect::_roundUp(int k, int n) {

}