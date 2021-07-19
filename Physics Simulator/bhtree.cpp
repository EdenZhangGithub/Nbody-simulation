#include "bhtree.h"

#include "body.h"

#include <iostream>

const float G_CONST = 6.67e-3;
const float E_CONST = 1e-20;
const float THRESHOLD = 0.5;

const int MAX_DEPTH = 40;

const bool Oct::Contains(Body* b)
{
    return (
        abs(b->Position.x - center.x) < (length / 2.0f) &&
        abs(b->Position.y - center.y) < (length / 2.0f) &&
        abs(b->Position.z - center.z) < (length / 2.0f)
        );
}

const int Oct::GetSubtree(Body* b)
{
    int x, y, z;

    if (b->Position.x > center.x)
    {
        x = 1;
    }
    else
    {
        x = 0;
    }

    if (b->Position.y > center.y)
    {
        y = 1;
    }
    else
    {
        y = 0;
    }

    if (b->Position.z > center.z)
    {
        z = 1;
    }
    else
    {
        z = 0;
    }

    return (x << 0) + (y << 1) + (z << 2);
}

BHTree::BHTree(Oct o)
    : oct(o)
    , is_external(false)
    , contains_body(false)
    , mass(0.0f)
    , center_of_mass(glm::vec3(0, 0, 0))
{
}

BHTree::~BHTree()
{
    for (int i = 0; i < 8; i++)
    {
        delete subtree[i];
    }
}

void BHTree::Insert(Body* b, int depth)
{
    if (depth > MAX_DEPTH)
    {
        return;
    }

    if (!contains_body)
    {
        body = b;
        contains_body = true;
        is_external = true;

    }
    else
    {
        if (!is_external)
        {
            CreateSubtree(b);
            subtree[oct.GetSubtree(b)]->Insert(b, depth + 1);
        }
        else
        {
            CreateSubtree(b);
            subtree[oct.GetSubtree(b)]->Insert(b, depth + 1);

            CreateSubtree(body);
            subtree[oct.GetSubtree(body)]->Insert(body, depth + 1);

            body = nullptr;
            is_external = false;
        }
    }
    center_of_mass = (center_of_mass * mass + b->Position * b->Mass) / (mass + b->Mass);
    mass = mass + b->Mass;
}

void BHTree::UpdateForce(Body* b)
{
    if (is_external)
    {
        if (body != b)
        {
            float F = G_CONST * b->Mass * body->Mass /
                sqrt(glm::distance2(b->Position, body->Position) + E_CONST);

            glm::vec3 Direction = glm::normalize(b->Position - body->Position);

            b->Velocity -= Direction * F / b->Mass;

        }

    }
    else
    {
        float sd = oct.length / glm::distance(b->Position, center_of_mass);

        if (sd > THRESHOLD)
        {
            float F = G_CONST * b->Mass * mass /
                sqrt(glm::distance2(b->Position, center_of_mass) + E_CONST);

            glm::vec3 Direction = glm::normalize(b->Position - center_of_mass);

            b->Velocity -= Direction * F / b->Mass;

        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                if (subtree[i])
                {
                    subtree[i]->UpdateForce(b);
                }
            }

        }
    }
}

void BHTree::CreateSubtree(Body* b)
{
    int i = oct.GetSubtree(b);

    // checks if null ptr or not
    if (subtree[i])
    {
        return;
    }

    Oct o{ oct.center, oct.length / 2.0f };

    int x = i % 2;
    int y = (i / 2) % 2;
    int z = (i / 4) % 2;

    if (x == 1)
    {
        o.center.x = o.center.x + o.length / 2.0f;
    }
    else
    {
        o.center.x = o.center.x - o.length / 2.0f;
    }

    if (y == 1)
    {
        o.center.y = o.center.y + o.length / 2.0f;
    }
    else
    {
        o.center.y = o.center.y - o.length / 2.0f;
    }

    if (z == 1)
    {
        o.center.z = o.center.z + o.length / 2.0f;
    }
    else
    {
        o.center.z = o.center.z - o.length / 2.0f;
    }

    subtree[i] = new BHTree(o);
}
