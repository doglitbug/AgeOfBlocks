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

    void setAmbientColor(const glm::vec3& ambientColor);
    void setAmbientIntensity(float ambientIntensity);

    void SetBoneTransform(uint index, const glm::mat4& transform) const;
private:
    GLuint m_shaderProgram{};
    std::vector<GLuint> m_shaders;

    // Ambient color information
    glm::vec3 m_ambientColor=glm::vec3(0.9f, 0.95f, 1.0f);
    float m_ambientIntensity=0.6f;
    GLint m_ambientColorLocation{};
    GLint m_ambientIntensityLocation{};

    // Diffuse color information
    glm::vec3 m_lightPosition=glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 m_lightColor=glm::vec3(0.2f, 0.2f, 0.8f);
    GLint m_lightPositionLocation{};
    GLint m_lightColorLocation{};

    GLuint m_boneLocation[100];
};
