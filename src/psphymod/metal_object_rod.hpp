#pragma once

#include "metal_object.hpp"

class MetalObjectRod : public MetalObject {
public:
    MetalObjectRod(int, float);
    void initializeNodes() override;
};
