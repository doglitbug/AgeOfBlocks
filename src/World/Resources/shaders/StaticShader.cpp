#include "StaticShader.h"

#include <string>

void StaticShader::init()
{
    BaseShader::init();

    std::string vs, fs;

    if (!Utils::readFileToString("src/World/Resources/shaders/vertex.glsl", vs))
    {
        exit(1);
    };

    addShader(GL_VERTEX_SHADER, vs.c_str());

    if (!Utils::readFileToString("src/World/Resources/shaders/fragment.glsl", fs))
    {
        exit(1);
    };

    addShader(GL_FRAGMENT_SHADER, fs.c_str());

    finalize();
}