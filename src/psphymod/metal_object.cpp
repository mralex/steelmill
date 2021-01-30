#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "metal_object.hpp"


MetalObject::MetalObject() {
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
                // printf("inode pos: %.4f\n", inode->pos.x);
                // printf("neighbor pos: %.4f\n", neighbor->pos.x);
                // printf("dif pos: %.4f\n", dif.x);

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

void MetalObject::render(int sampleRate, size_t length, float *samples, float attenuation) {

}
