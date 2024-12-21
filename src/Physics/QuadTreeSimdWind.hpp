#pragma once 
#include <iostream>
#include <glm/glm.hpp>
#include "particle.hpp"
#include <memory>
#include <vector>
#include <array>
#include <limits>
#include <immintrin.h> // For AVX2 intrinsics

    class OctreeWind {
    private:
        struct Node {
            glm::vec3 Center;
            float size;
            float TotalMass;
            glm::vec3 CenterOfMass;
            std::vector<const Particle*> particles;
            std::array<std::unique_ptr<Node>, 8> children;

            Node(const glm::vec3& center, float size) : Center(center), size(size), TotalMass(0), CenterOfMass(0) {}
            bool isLeaf() const {
                return children[0] == nullptr;
            }
        };

        std::unique_ptr<Node> root;
        float theta;

        void insertParticle(Node* node, const Particle* P) {
            if (node == nullptr) return;

            if (node->isLeaf() && node->particles.empty()) {
                node->particles.push_back(P);
                node->TotalMass = P->mass;
                node->CenterOfMass = P->position;
            }
            else {
                if (node->isLeaf() && !node->particles.empty()) {
                    subdivide(node);
                    for (const Particle* oldP : node->particles) {
                        insertParticle(node, oldP);
                    }
                    node->particles.clear();
                }
                int octant = getOctant(node, P->position);
                insertParticle(node->children[octant].get(), P);
            }
            updateNodeProperties(node, P);
        }

        void subdivide(Node* node) {
            for (int i = 0; i < 8; ++i) {
                glm::vec3 newCenter = node->Center;
                newCenter.x += node->size * 0.25f * ((i & 1) ? 1 : -1);
                newCenter.y += node->size * 0.25f * ((i & 2) ? 1 : -1);
                newCenter.z += node->size * 0.25f * ((i & 4) ? 1 : -1);
                node->children[i] = std::make_unique<Node>(newCenter, node->size * 0.5f);
            }
        }

        int getOctant(const Node* node, const glm::vec3& position) const {
            int octant = 0;
            if (position.x > node->Center.x) octant |= 1;
            if (position.y > node->Center.y) octant |= 2;
            if (position.z > node->Center.z) octant |= 4;
            return octant;
        }

        void updateNodeProperties(Node* node, const Particle* P) {
            float newTotalMass = node->TotalMass + P->mass;
            node->CenterOfMass = (node->CenterOfMass * node->TotalMass + P->position * P->mass) / newTotalMass;
            node->TotalMass = newTotalMass;
        }

        void calculateForceRecursive(const Particle* p, const Node* node, float G, glm::vec3& force) const {
            if (node == nullptr || (node->isLeaf() && node->particles.empty())) return;

            float d = glm::distance(p->position, node->CenterOfMass);
            if (node->isLeaf() || (node->size / d < theta)) {
                if (d > 1e-6f && (node->particles.empty() || node->particles[0] != p)) {
                    glm::vec3 dir = glm::normalize(node->CenterOfMass - p->position);
                    float forceMagnitude = G * p->mass * node->TotalMass / (d * d);
                    force += dir * forceMagnitude;
                }
            }
            else {
                for (const auto& child : node->children) {
                    calculateForceRecursive(p, child.get(), G, force);
                }
            }
        }

    public:
        OctreeWind(float theta = 0.5f) : theta(theta) {}

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




    void calculateForceRecursiveSIMD(const Particle* p, const Node* node, float G, __m256& forceX, __m256& forceY, __m256& forceZ) const {
        if (node == nullptr || (node->isLeaf() && node->particles.empty())) return;

        __m256 pPosX = _mm256_set1_ps(p->position.x);
        __m256 pPosY = _mm256_set1_ps(p->position.y);
        __m256 pPosZ = _mm256_set1_ps(p->position.z);
        __m256 nodeCenterX = _mm256_set1_ps(node->CenterOfMass.x);
        __m256 nodeCenterY = _mm256_set1_ps(node->CenterOfMass.y);
        __m256 nodeCenterZ = _mm256_set1_ps(node->CenterOfMass.z);

        __m256 dx = _mm256_sub_ps(nodeCenterX, pPosX);
        __m256 dy = _mm256_sub_ps(nodeCenterY, pPosY);
        __m256 dz = _mm256_sub_ps(nodeCenterZ, pPosZ);

        __m256 distSq = _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(dx, dx), _mm256_mul_ps(dy, dy)), _mm256_mul_ps(dz, dz));
        __m256 dist = _mm256_sqrt_ps(distSq);

        __m256 threshold = _mm256_set1_ps(node->size / theta);
        __m256 cmp = _mm256_cmp_ps(dist, threshold, _CMP_GT_OQ);

        if (_mm256_movemask_ps(cmp) == 0xFF || node->isLeaf()) {
            __m256 minDist = _mm256_set1_ps(1e-6f);
            __m256 mask = _mm256_cmp_ps(dist, minDist, _CMP_GT_OQ);

            if (_mm256_movemask_ps(mask)) {
                __m256 invDist = _mm256_div_ps(_mm256_set1_ps(1.0f), dist);
                __m256 invDistCube = _mm256_mul_ps(_mm256_mul_ps(invDist, invDist), invDist);

                __m256 forceMag = _mm256_mul_ps(_mm256_set1_ps(G * p->mass * node->TotalMass), invDistCube);

                forceX = _mm256_add_ps(forceX, _mm256_mul_ps(forceMag, dx));
                forceY = _mm256_add_ps(forceY, _mm256_mul_ps(forceMag, dy));
                forceZ = _mm256_add_ps(forceZ, _mm256_mul_ps(forceMag, dz));
            }
        }
        else {
            for (const auto& child : node->children) {
                if (child) {
                    calculateForceRecursiveSIMD(p, child.get(), G, forceX, forceY, forceZ);
                }
            }
        }
    }

public:
    // ... (previous public methods)

    glm::vec3 calculateForce(const Particle* p, float G) const {
        __m256 forceX = _mm256_setzero_ps();
        __m256 forceY = _mm256_setzero_ps();
        __m256 forceZ = _mm256_setzero_ps();

        if (root) {
            calculateForceRecursiveSIMD(p, root.get(), G, forceX, forceY, forceZ);
        }

        float* fx = (float*)&forceX;
        float* fy = (float*)&forceY;
        float* fz = (float*)&forceZ;

        glm::vec3 totalForce(0);
        for (int i = 0; i < 8; ++i) {
            totalForce.x += fx[i];
            totalForce.y += fy[i];
            totalForce.z += fz[i];
        }

        return totalForce;
    }
};