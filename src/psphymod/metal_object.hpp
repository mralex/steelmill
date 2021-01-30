/* Power Station PhyMod Library
 * Copyright (c) 2000 David A. Bartold
 * Copyright (c) 2021 Alex Roberts
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#pragma once

#include <vector>
#include <memory>
#include "vector3.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

struct ObjectNode {
    bool anchor;
    Vector3 pos;
    Vector3 vel;

    std::vector<std::shared_ptr<ObjectNode>> neighbors;

    ObjectNode() {
        neighbors.reserve(8);
    }
};

class MetalObject {
public:
    std::vector<std::shared_ptr<ObjectNode>> nodes;

    int width;
    int height;
    int innode, outnode;
    float innodeNormal, outnodeNormal;
    float tension, speed, damping;
    int actuation;
    float velocity;
    float percent;
    bool stop = false;

    MetalObject();
    ~MetalObject();

    virtual void initializeNodes() {}

    void perturb(const float, const float);
    void render(int sampleRate, size_t length, float *samples, float attenuation);
};
