#include "OrbitController.h"
#include <limits>
#include "glm/gtc/constants.hpp"
#include "glm/gtx/string_cast.hpp"

namespace rcube {

void cartesianToSpherical(const glm::vec3 &v, float &r, float &azimuth, float &polar,
                          float polar_eps=1e-3f) {
    r = glm::length(v);
    if (r < 1e-6f) {
        azimuth = 0.f;
        polar = 0.f;
    }
    else {
        azimuth = std::atan2(v.x, v.z);
        polar = glm::acos(glm::clamp(v.y / r, -1.f, 1.f));
    }
    polar = glm::clamp(polar, polar_eps, glm::pi<float>() - polar_eps);
}

glm::vec3 sphericalToCartesian(float r, float azimuth, float polar) {
    float x = r * glm::sin(polar) * glm::sin(azimuth);
    float y = r * glm::cos(polar);
    float z = r * glm::sin(polar) * glm::cos(azimuth);
    return glm::vec3(x, y, z);
}

OrbitController::OrbitController() : min_horizontal_angle(-std::numeric_limits<float>::infinity()),
    max_horizontal_angle(std::numeric_limits<double>::infinity()),
    min_vertical_angle(0.1f), max_vertical_angle(glm::pi<float>() - 0.1f), orbiting_(false) {
}

void OrbitController::update(const CameraController::InputState &state) {
    PanZoomController::update(state);
    if (orbiting_ != state.mouse_right) {
        orbiting_ = state.mouse_right;
        if (orbiting_) {
            last_x_ = state.x;
            last_y_ = state.y;
        }
    }

    if (orbiting_ && last_x_ != state.x && last_y_ != state.y) {
        glm::vec3 pos = transform_->position();
        float r, ha, va;
        cartesianToSpherical(pos, r, ha, va);
        float dx = static_cast<float>(last_x_ - state.x) / static_cast<float>(width_);
        float dy = static_cast<float>(last_y_ - state.y) / static_cast<float>(height_);
        dx *= orbit_speed;
        dy *= orbit_speed;
        va = glm::clamp(va + dy, min_vertical_angle, max_vertical_angle);
        ha = glm::clamp(ha + dx, min_horizontal_angle, max_horizontal_angle);
        pos = sphericalToCartesian(r, ha, va);
        transform_->setPosition(pos);
        last_x_ = state.x;
        last_y_ = state.y;
    }

}

} // namespace rcube
