#include "Influencer.h"



Influencer::Influencer(type t, shape s, vec2 p, vec2 si, Shader* shade, std::string n)
    :ty(t), sh(s), pos(p), size(si), shader(shade), name(n)
{
    //create vertex data for drawing quad
    /*float positions[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };*/

    std::vector<float> positions;
    std::vector<unsigned int> indices;

    if (sh == OVOID) {
        int vertices = 100;

        positions = { 0.0f, 0.0f,
                      1.0f, 0.0f };

        for (int i = 2; i <= vertices; ++i) {
            positions.push_back(cos(2*M_PI / float(vertices) * (float(i-1)))); //x
            positions.push_back(sin(2*M_PI / float(vertices) * (float(i-1)))); //y
            indices.push_back(0);
            indices.push_back(i - 1);
            indices.push_back(i);
        }
        indices.push_back(0);
        indices.push_back(vertices);
        indices.push_back(1);
    }
    else if (sh == QUAD) {
        positions = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
        };

        indices = {
        0, 1, 2,
        2, 3, 0
        };
    }

    //create index data for drawing quad
    /*unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };*/

    //Setup VA and IB
    p_VA = new VertexArray;
    p_VB = new VertexBuffer(&positions[0], positions.size() * sizeof(float));
    p_VA->AddBuffer(*p_VB, 2);
    p_IB = new IndexBuffer(&indices[0], indices.size());
}

Influencer::~Influencer()
{
}

