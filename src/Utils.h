#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <assimp/matrix4x4.h>
#include <SDL3/SDL_log.h>
#include "glm/gtc/type_ptr.hpp"

#include "glm/fwd.hpp"

// TODO Move to utilities file
inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    // Assimp is row-major (a, b, c, d are rows; 1, 2, 3, 4 are columns)
    // GLM is column-major (to[col][row])
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

class Utils
{
public:
    static bool readFileToString(const std::string& filePath, std::string& output)
    {
        std::ifstream inputFile(filePath, std::ios::in | std::ios::binary);
        if (!inputFile.is_open())
        {
            SDL_Log("Unable to open file %s", filePath.c_str());
            return false;
        }
        std::ostringstream streamBuffer;
        streamBuffer << inputFile.rdbuf();
        output = streamBuffer.str();
        return true;
    }

    static void checkOpenGLError(const std::string &location= "nowhere")
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::string errorStr;
            switch (err)
            {
            case GL_INVALID_ENUM:
                errorStr = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorStr = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorStr = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                errorStr = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                errorStr = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                errorStr = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorStr = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                errorStr = "UNKNOWN_ERROR";
                break;
            }
            SDL_Log("OpenGL Error at %s : %s (0x%04X)", location.c_str(), errorStr.c_str(), static_cast<unsigned int>(err));
        }
    }
};
