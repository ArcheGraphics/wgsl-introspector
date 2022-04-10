//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_reflect.h"

std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> WgslReflect::TypeInfo = {
        {"i32",    {4,  4}},
        {"u32",    {4,  4}},
        {"f32",    {4,  4}},
        {"atomic", {4,  4}},
        {"vec2",   {8,  8}},
        {"vec3",   {16, 12}},
        {"vec4",   {16, 16}},
        {"mat2x2", {8,  16}},
        {"mat3x2", {8,  24}},
        {"mat4x2", {8,  32}},
        {"mat2x3", {16, 32}},
        {"mat3x3", {16, 48}},
        {"mat4x3", {16, 64}},
        {"mat2x4", {16, 32}},
        {"mat3x4", {16, 48}},
        {"mat4x4", {16, 64}}
};

std::string WgslReflect::TextureTypes(const std::string &key) {
    auto iter = Token::TextureType.find(key);
    if (iter != Token::TextureType.end()) {
        return iter->second.name;
    } else {
        return "-1";
    }
}

std::string WgslReflect::SamplerTypes(const std::string &key) {
    auto iter = Token::SamplerType.find(key);
    if (iter != Token::SamplerType.end()) {
        return iter->second.name;
    } else {
        return "-1";
    }
}

WgslReflect::WgslReflect(const std::string &code) {
    initialize(code);
}

void WgslReflect::initialize(const std::string &code) {
    auto parser = WgslParser();
    ast = parser.parse(code);

    // All top-level structs in the shader.
    structs = {};
    // All top-level uniform vars in the shader.
    uniforms = {};
    // All top-level texture vars in the shader;
    textures = {};
    // All top-level sampler vars in the shader.
    samplers = {};
    // All top-level functions in the shader.
    functions = {};
    // All top-level type aliases in the shader.
    aliases = {};
    // All entry functions in the shader: vertex, fragment, and/or compute.
    entry = {
            {"vertex",   {}},
            {"fragment", {}},
            {"compute",  {}},
    };

    for (const auto &node: ast) {
        auto nodePtr = node.get();
        if (nodePtr->type() == "struct")
            structs.push_back(nodePtr);

        if (nodePtr->type() == "alias")
            aliases.push_back(nodePtr);

        if (isUniformVar(nodePtr)) {
            auto group = getAttribute(nodePtr, "group");
            nodePtr->setGroup(group && !group->nameVec("value").empty() ?
                              std::stoi(group->nameVec("value")[0]) : 0);
            auto binding = getAttribute(nodePtr, "binding");
            nodePtr->setBinding(binding && !binding->nameVec("value").empty() ?
                                std::stoi(binding->nameVec("value")[0]) : 0);
            uniforms.push_back(nodePtr);
        }

        if (isTextureVar(nodePtr)) {
            auto group = getAttribute(nodePtr, "group");
            nodePtr->setGroup(group && !group->nameVec("value").empty() ?
                              std::stoi(group->nameVec("value")[0]) : 0);
            auto binding = getAttribute(nodePtr, "binding");
            nodePtr->setBinding(binding && !binding->nameVec("value").empty() ?
                                std::stoi(binding->nameVec("value")[0]) : 0);
            textures.push_back(nodePtr);
        }

        if (isSamplerVar(nodePtr)) {
            auto group = getAttribute(nodePtr, "group");
            nodePtr->setGroup(group && !group->nameVec("value").empty() ?
                              std::stoi(group->nameVec("value")[0]) : 0);
            auto binding = getAttribute(nodePtr, "binding");
            nodePtr->setBinding(binding && !binding->nameVec("value").empty() ?
                                std::stoi(binding->nameVec("value")[0]) : 0);
            samplers.push_back(nodePtr);
        }

        if (node->type() == "function") {
            functions.push_back(nodePtr);
            auto stage = getAttribute(nodePtr, "stage");
            if (stage) {
                std::vector<InputInfo> inputs{};
                _getInputs(nodePtr, inputs);
                node.inputs = _getInputs(nodePtr);

                // TODO give error about non-standard stages.
                if (!entry[stage->nameVec("value")[0]].empty())
                    entry[stage->nameVec("value")[0]].push_back(nodePtr);
                else
                    entry[stage->nameVec("value")[0]] = {nodePtr};
            }
        }
    }
}

bool WgslReflect::isTextureVar(AST *node) {
    return node->type() == "var" && WgslReflect::TextureTypes(node->child("type")->name()) != "-1";
}

bool WgslReflect::isSamplerVar(AST *node) {
    return node->type() == "var" && WgslReflect::SamplerTypes(node->child("type")->name()) != "-1";
}

bool WgslReflect::isUniformVar(AST *node) {
    return node && node->type() == "var" && node->nameVec("storage")[0] == "uniform";
}

AST *WgslReflect::getAlias(AST *node) {
    if (!node) return nullptr;
    if (node->type() != "type")
        return nullptr;
    auto name = node->name();
    for (auto u: aliases) {
        if (u->name() == name)
            return u->child("alias");
    }
    return nullptr;
}

AST *WgslReflect::getAlias(const std::string &name) {
    if (name.empty()) return nullptr;
    for (auto u: aliases) {
        if (u->name() == name)
            return u->child("alias");
    }
    return nullptr;
}

AST *WgslReflect::getStruct(AST *node) {
    if (!node) return nullptr;
    if (node->type() == "struct")
        return node;
    if (node->type() != "type")
        return nullptr;
    auto name = node->name();
    for (const auto &u: structs) {
        if (u->name() == name)
            return u;
    }
    return nullptr;
}

AST *WgslReflect::getStruct(const std::string &name) {
    if (name.empty()) return nullptr;
    for (const auto u: structs) {
        if (u->name() == name)
            return u;
    }
    return nullptr;
}

AST *WgslReflect::getAttribute(AST *node, const std::string &name) {
    if (!node || !node->childVec("attributes").empty()) return nullptr;
    for (const auto &a: node->childVec("attributes")) {
        if (a->name() == name)
            return a.get();
    }
    return nullptr;
}

void WgslReflect::getBindGroups() {

}

void WgslReflect::getUniformBufferInfo(AST *node) {

}

void WgslReflect::getTypeInfo(const std::string &type) {

}

void WgslReflect::_getInputs(const std::string &args, std::vector<InputInfo> &inputs) {

}

std::optional<WgslReflect::InputInfo> WgslReflect::_getInputInfo(AST *node) {

}

void WgslReflect::_roundUp(int k, int n) {

}