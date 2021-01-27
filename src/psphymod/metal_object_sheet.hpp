#pragma once

#include "metal_object.hpp"

class MetalObjectSheet : public MetalObject {
public:
    MetalObjectSheet(int, int, float);
    void initialize(int, int, float);
};
