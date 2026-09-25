#include "CharacterShader.h"

#include <bits/stl_algo.h>

#include "glm/gtc/type_ptr.hpp"

void CharacterShader::init()
{
    BaseShader::init();

    std::string vs, fs;
    if (!Utils::readFileToString("src/World/shaders/vertex.glsl", vs))
    {
        exit(1);
    };

    addShader(GL_VERTEX_SHADER, vs.c_str());

    if (!Utils::readFileToString("src/World/shaders/fragment.glsl", fs))
    {
        exit(1);
    };

    addShader(GL_FRAGMENT_SHADER, fs.c_str());

    finalize();
}

void CharacterShader::finalize()
{
    BaseShader::finalize();

    for (unsigned int i = 0; i < std::size(m_boneLocation); ++i)
    {
        std::string name = "gBones[" + std::to_string(i) + "]";
        m_boneLocation[i] = getUniformLocation(name);
    }
}

void CharacterShader::enable()
{
    BaseShader::enable();
}

void CharacterShader::SetBoneTransform(const unsigned int index, const glm::mat4& transform) const
{
    // Ensure index is within valid bounds of the array
    if (index >= std::size(m_boneLocation))
    {
        return;
    }

    // Upload the matrix using the standard GLM value pointer
    glUniformMatrix4fv(m_boneLocation[index], 1, GL_FALSE, glm::value_ptr(transform));
}
