#pragma once

#include "metal_object.hpp"

class MetalObjectPipe : public MetalObject {
public:
    MetalObjectPipe(int, int, float);
    void initialize(int, int, float);
};
