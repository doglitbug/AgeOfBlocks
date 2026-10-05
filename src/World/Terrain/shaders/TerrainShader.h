#pragma once
#include "World/BaseShader.h"

class TerrainShader : public BaseShader
{
public:
    TerrainShader()= default;
    void init() override;
};
