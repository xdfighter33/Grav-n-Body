#pragma once 
#include <iostream>
#include <glm/glm.hpp>
#include "particle.hpp"
#include <memory>
#include <vector>
#include <limits>

class Octree {
private:
    struct Node {
        glm::vec3 Center;
        float size;
        float TotalMass;
        glm::vec3 CenterOfMass;
        std::vector<const Particle*> particles;  // Store const pointers to particles
        std::array<std::unique_ptr<Node>, 8> children;
        
        Node(const glm::vec3& center, float size) : Center(center), size(size), TotalMass(0), CenterOfMass(0) {}
        bool isLeaf() const {
            return children[0] == nullptr;
        }
    };

    std::unique_ptr<Node> root;
    float theta;

    void insertParticle(Node* node, const Particle* P) {
        if (node->isLeaf() && node->particles.empty()) {
            node->particles.push_back(P);
            node->TotalMass = P->mass;
            node->CenterOfMass = P->position;
        } else { 
            if (node->isLeaf() && !node->particles.empty()) {
                for (int i = 0; i < 8; ++i) {
                    glm::vec3 newCenter = node->Center;
                    newCenter.x += node->size * 0.25f * ((i & 1) ? 1 : -1);
                    newCenter.y += node->size * 0.25f * ((i & 2) ? 1 : -1);
                    newCenter.z += node->size * 0.25f * ((i & 4) ? 1 : -1);
                    node->children[i] = std::make_unique<Node>(newCenter, node->size * 0.5f);
                }
                for (const Particle* oldP : node->particles) {
                    insertParticle(node, oldP);
                }
                node->particles.clear();
            }
            int octant = 0;
            if (P->position.x > node->Center.x) octant |= 1;
            if (P->position.y > node->Center.y) octant |= 2;
            if (P->position.z > node->Center.z) octant |= 4;
            insertParticle(node->children[octant].get(), P);
        }
        node->TotalMass += P->mass;
        node->CenterOfMass = (node->CenterOfMass * node->TotalMass + P->position * P->mass) / (node->TotalMass + P->mass);
    }

    void calculateForceRecursive(const Particle* p, const Node* node, float G, glm::vec3& force) const {
        if (node == nullptr || (node->isLeaf() && node->particles.empty())) return;
        float d = glm::distance(p->position, node->CenterOfMass);
        if (node->isLeaf() || (node->size / d < theta)) {
            if (d > 1e-6f && node->particles[0] != p) {  // Avoid self-interaction
                glm::vec3 dir = glm::normalize(node->CenterOfMass - p->position);
                float forceMagnitude = G * p->mass * node->TotalMass / (d * d);
                force += dir * forceMagnitude;
            }
        } else {
            for (const auto& child : node->children) {
                if (child) {
                    calculateForceRecursive(p, child.get(), G, force);
                }
            }
        }
    }

public:
    Octree(float theta = 0.5f) : theta(theta) {}

    void build(const std::vector<Particle>& particles) {
            if (particles.empty()) return;

            glm::vec3 min(std::numeric_limits<float>::max());
            glm::vec3 max(std::numeric_limits<float>::lowest());
            for (const auto& p : particles) {
                min = glm::min(min, p.position);
                max = glm::max(max, p.position);
            }
        glm::vec3 center = (min + max) * 0.5f;
        float size = glm::length(max - min) * 0.5f;
        root = std::make_unique<Node>(center, size);
            for (const auto& p : particles) {
                insertParticle(root.get(), &p);
            }
    }

    glm::vec3 calculateForce(const Particle* p, float G) const {
        glm::vec3 force(0);
        if (root) {
            calculateForceRecursive(p, root.get(), G, force);
        }
        return force;
    }
};