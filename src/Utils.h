#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <SDL3/SDL_log.h>

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
            //TODO Change to SDL_Log
            std::cerr << "OpenGL Error at " << location << ": " << errorStr << " (0x" << std::hex << err << ")" << std::endl;
        }
    }
};
