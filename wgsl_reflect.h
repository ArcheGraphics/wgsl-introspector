//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef WGSL_INTROSPECTOR_WGSL_REFLECT_H
#define WGSL_INTROSPECTOR_WGSL_REFLECT_H

#include "wgsl_parser.h"

class WgslReflect {
public:
    struct InputInfo {
        std::string name;
        std::string type;
        AST* node;
        std::string locationType;
        uint32_t location;
    };

    // type: align, size
    static std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> TypeInfo;

    static std::string TextureTypes(const std::string &key);

    static std::string SamplerTypes(const std::string &key);

    WgslReflect(const std::string &code);

    void initialize(const std::string &code);

    bool isTextureVar(AST *node);

    bool isSamplerVar(AST *node);

    bool isUniformVar(AST *node);

    AST* getAlias(AST* node);

    AST* getAlias(const std::string &name);

    AST* getStruct(AST* node);

    AST* getStruct(const std::string &name);

    static AST *getAttribute(AST *node, const std::string &name);

    void getBindGroups();

    void getUniformBufferInfo(AST *node);

    void getTypeInfo(const std::string &type);

private:

    void _getInputs(const std::string &args, std::vector<InputInfo> &inputs);

    std::optional<InputInfo> _getInputInfo(AST *node);

    void _roundUp(int k, int n);

public:
    std::vector<std::unique_ptr<AST>> ast;

    // All top-level structs in the shader.
    std::vector<AST *> structs{};
    // All top-level uniform vars in the shader.
    std::vector<AST *> uniforms{};
    // All top-level texture vars in the shader;
    std::vector<AST *> textures{};
    // All top-level sampler vars in the shader.
    std::vector<AST *> samplers{};
    // All top-level functions in the shader.
    std::vector<AST *> functions{};
    // All top-level type aliases in the shader.
    std::vector<AST *> aliases{};
    // All entry functions in the shader: vertex, fragment, and/or compute.
    std::unordered_map<std::string, std::vector<AST *>> entry;
};

#endif //WGSL_INTROSPECTOR_WGSL_REFLECT_H
