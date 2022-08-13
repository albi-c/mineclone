#pragma once

namespace frustum {
    struct Plane {
        glm::vec3 normal;
        float distance;

        Plane() =default;
        Plane(const glm::vec3& p1, const glm::vec3& norm)
            : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}
        
        float signed_distance(const glm::vec3& point) const {
            return glm::dot(normal, point) - distance;
        }
    };

    struct Frustum {
        Plane top, bottom, left, right, far, near;
    };

    struct AABB {
        glm::vec3 center;
        glm::vec3 extents;
        bool enabled = false;

        AABB()
            : enabled(false) {}
        AABB(const glm::vec3& min, const glm::vec3& max)
            : center((max + min) * 0.5f), extents(max.x - center.x, max.y - center.y, max.z - center.z), enabled(true) {}

        bool on_or_forward_plane(const Plane& plane) const {
            const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);
            return -r <= plane.signed_distance(center);
        }

        bool in_frustum(const frustum::Frustum& frustum) const {
            return (!enabled ||
                    on_or_forward_plane(frustum.left) && on_or_forward_plane(frustum.right) &&
                    on_or_forward_plane(frustum.top) && on_or_forward_plane(frustum.bottom) &&
                    on_or_forward_plane(frustum.near) && on_or_forward_plane(frustum.far));
        }
    };
};
