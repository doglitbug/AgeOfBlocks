#pragma once
#include "World/BaseShader.h"

class StaticShader : public BaseShader
{
public:
    StaticShader()= default;
    void init() override;
};
