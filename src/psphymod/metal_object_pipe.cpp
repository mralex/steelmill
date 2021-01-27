#include <math.h>

#include "metal_object_pipe.hpp"

MetalObjectPipe::MetalObjectPipe(int circumference, int height, float tension) : MetalObject(height) {
    this->initialize(circumference, height, tension);
}

void MetalObjectPipe::initialize(int circumference, int height, float tension)
{
    std::shared_ptr<ObjectNode> node;

    float radius = 0.5f / cos((M_PI * (circumference - 2.f)) / (circumference * 0.5f));

    nodes.clear();
    nodes.reserve(height * circumference);

    int n = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < circumference; x++)
        {
            float angle = x * 2.0f * M_PI / circumference;

            node = std::make_shared<ObjectNode>();
            node->vel = Vector3();
            node->pos = Vector3(cos(angle) * radius, sin(angle) * radius, y * tension);
            nodes.push_back(node);

            if (x == 0)
                node->neighbors.push_back(nodes[y * circumference + circumference - 1]);
            else
                node->neighbors.push_back(nodes[n - 1]);

            if (x == circumference - 1)
                node->neighbors.push_back(nodes[y * circumference]);
            else
                node->neighbors.push_back(nodes[n + 1]);

            if (y == 0)
                node->neighbors.push_back(nodes[n + circumference]);
            else if (y == height - 1)
                node->neighbors.push_back(nodes[n - circumference]);
            else
            {
                node->neighbors.push_back(nodes[n + circumference]);
                node->neighbors.push_back(nodes[n - circumference]);
            }

            n++;
        }
    }

    // for (int i = 0; i < height; i++) {
    //     node = std::make_shared<ObjectNode>();
    //     node->vel = Vector3();
    //     node->pos = Vector3(0.f, 0.f, i * tension);
    //     nodes.push_back(node);
    // }

    // for (int i = 0; i < height; i++)
    // {
    //     if (i == 0)
    //     {
    //         nodes[i]->neighbors[0] = nodes[1];
    //         nodes[i]->anchor = true;
    //     }
    //     else if (i == height - 1)
    //     {
    //         nodes[i]->neighbors[0] = nodes[i - 1];
    //         nodes[i]->anchor = true;
    //     }
    //     else
    //     {
    //         nodes[i]->neighbors[0] = nodes[i - 1];
    //         nodes[i]->neighbors[1] = nodes[i + 1];
    //     }
    // }
}
