#include <math.h>

#include "metal_object_sheet.hpp"

MetalObjectSheet::MetalObjectSheet(int width, int height, float tension) : MetalObject(height) {
    this->initialize(width, height, tension);
}

void MetalObjectSheet::initialize(int width, int height, float tension)
{
    std::shared_ptr<ObjectNode> node;

    nodes.clear();
    nodes.reserve(height);

    int n = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            node = std::make_shared<ObjectNode>();
            node->vel = Vector3();
            node->pos = Vector3(0.f, (float)x, y * tension);
            nodes.push_back(node);

            int count = 0;
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (x + dx >= 0 && x + dx < width && y + dy >= 0 && y + dy < height && (dx != 0 || dy != 0))
                    {
                        node->neighbors.push_back(nodes[(y + dy) * width + x + dx]);
                    }
                }
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
