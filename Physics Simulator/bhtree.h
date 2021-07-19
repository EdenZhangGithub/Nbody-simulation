#pragma once

#include <glm/gtx/norm.hpp>

class Body;

struct Oct
{
    glm::vec3 center;
    float length;

    const bool Contains(Body* b);
    const int GetSubtree(Body* b);
};

class BHTree
{
public:
    BHTree(Oct o);
    ~BHTree();
    void Insert(Body* b, int depth = 0);
    void UpdateForce(Body* b);
    void CreateSubtree(Body* b);

    bool contains_body;
    bool is_external;
    Body* body;
    Oct oct;

    glm::vec3 center_of_mass;
    float mass;

    BHTree* subtree[8];
};