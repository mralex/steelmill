#include <stdlib.h>
#include <math.h>

#include "metal_object.hpp"


MetalObject::MetalObject(const int size) {
    nodes.reserve(size);
}

MetalObject::~MetalObject() {
    // delete[] nodes;
}

void MetalObject::perturb(const float speed, const float damp) {
    Vector3 sum;
    Vector3 dif;
    float temp;
    float sprinps_k;

    for(auto inode : nodes)
    {
        if (!inode->anchor)
        {
            sum = Vector3(0.f);

            for (auto neighbor : inode->neighbors)
            {
                dif = inode->pos - neighbor->pos;
                temp = 1.0f - sqrt(dif.square());
                sum = sum + dif * temp;
            }

            sprinps_k = 1.0f;
            inode->vel = (inode->vel + sum * speed * sprinps_k) * damp;
        }
    }

    for (auto inode : nodes)
    {
        if (!inode->anchor)
        {
            inode->pos = inode->pos + inode->vel * speed;
        }
    }
}
