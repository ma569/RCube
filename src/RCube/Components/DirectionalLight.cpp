#include "RCube/Components/DirectionalLight.h"
namespace rcube
{

DirectionalLight::DirectionalLight(glm::vec3 rgb)
{
    light_.pos_w = 0.f;
    light_.color = rgb;
    shadow_map_ = Texture2D::create(1024, 1024, 1, TextureInternalFormat::Depth24Stencil8);
}

const glm::vec3 &DirectionalLight::color() const
{
    return light_.color;
}

void DirectionalLight::setColor(const glm::vec3 &rgb)
{
    light_.color = rgb;
}

} // namespace rcube
