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
    // type: align, size
    static std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> TypeInfo;

    static std::string TextureTypes(const std::string& key);

    static std::string SamplerTypes(const std::string& key);

    WgslReflect(const std::string &code);

    void initialize(const std::string &code);

    bool isTextureVar(AST *node);

    bool isSamplerVar(AST *node);

    bool isUniformVar(AST *node);

    void getAlias(const std::string &name);

    void getStruct(const std::string &name);

    void getAttribute(AST *node, const std::string &name);

    void getBindGroups();

    void getUniformBufferInfo(AST *node);

    void getTypeInfo(const std::string &type);

private:
    void _getInputs(const std::string &args, const std::string &inputs);

    void _getInputInfo(AST *node);

    void _parseInt(const std::string &s);

    void _roundUp(int k, int n);

};

#endif //WGSL_INTROSPECTOR_WGSL_REFLECT_H
