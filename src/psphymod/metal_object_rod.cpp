#include "metal_object_rod.hpp"

MetalObjectRod::MetalObjectRod(int height, float tension) {
    this->height = height;
    this->tension = tension;
    this->initializeNodes();
}

void MetalObjectRod::initializeNodes()
{
    std::shared_ptr<ObjectNode> node;

    nodes.clear();
    nodes.reserve(height);

    for (int i = 0; i < height; i++) {
        node = std::make_shared<ObjectNode>((i == 0 || i == height - 1) ? 1 : 2);
        node->vel = Vector3();
        node->pos = Vector3(0.f, 0.f, i * tension);
        nodes.push_back(node);
    }

    for (int i = 0; i < height; i++)
    {
        if (i == 0)
        {
            nodes[i]->neighbors.push_back(nodes[1]);
            nodes[i]->anchor = true;
        }
        else if (i == height - 1)
        {
            nodes[i]->neighbors.push_back(nodes[i - 1]);
            nodes[i]->anchor = true;
        }
        else
        {
            nodes[i]->neighbors.push_back(nodes[i - 1]);
            nodes[i]->neighbors.push_back(nodes[i + 1]);
        }
    }
}
