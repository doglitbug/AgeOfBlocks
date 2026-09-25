#pragma once
#include <string>
#include <vector>

#include "BaseShader.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

class CharacterShader: public BaseShader
{
public:
    CharacterShader()=default;
    
    void init() override;
    void finalize() override;
    void enable() override;

    void SetBoneTransform(uint index, const glm::mat4& transform) const;
private:
    GLuint m_shaderProgram{};
    std::vector<GLuint> m_shaders;

    GLuint m_boneLocation[100];
};
