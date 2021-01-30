#include <cstdio>
#include <cmath>

#include "metal_object_pipe.hpp"

MetalObjectPipe::MetalObjectPipe(int circumference, int height, float tension) {
    this->width = circumference;
    this->height = height;
    this->tension = tension;
    this->initializeNodes();
}

void MetalObjectPipe::initializeNodes()
{
    std::shared_ptr<ObjectNode> node;

    float radius = 0.5f / cos((M_PI * (width - 2.f)) / (width * 0.5f));

    nodes.clear();
    nodes.reserve(height * width);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float angle = x * 2.0f * M_PI / width;
            printf("angle? %.2f\n", angle);
            node = std::make_shared<ObjectNode>();
            node->vel = Vector3(0.f);
            node->pos = Vector3(cos(angle) * radius, sin(angle) * radius, y * tension);
            printf("node pos: %.2f, %.2f, %.2f\n", node->pos.x, node->pos.y, node->pos.z);
            nodes.push_back(node);
        }
    }

    int n = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            node = nodes[n];
            
            if (x == 0)
                node->neighbors.push_back(nodes[y * width + width - 1]);
            else
                node->neighbors.push_back(nodes[n - 1]);

            if (x == width - 1)
                node->neighbors.push_back(nodes[y * width]);
            else
                node->neighbors.push_back(nodes[n + 1]);

            if (y == 0)
                node->neighbors.push_back(nodes[n + width]);
            else if (y == height - 1)
                node->neighbors.push_back(nodes[n - width]);
            else
            {
                node->neighbors.push_back(nodes[n + width]);
                node->neighbors.push_back(nodes[n - width]);
            }

            n++;
        }
    }
}
