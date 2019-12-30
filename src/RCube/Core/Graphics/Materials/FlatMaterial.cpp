#include "RCube/Core/Graphics/Materials/FlatMaterial.h"
#include <string>

namespace rcube
{

const std::string vert_src = R"(
#version 420

layout (location = 0) in vec3 vertex;
layout (location = 3) in vec3 color;

layout (std140, binding=0) uniform Matrices {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 viewport_matrix;
};

uniform mat4 model_matrix;

out vec3 frag_color;

void main() {
    vec4 world_vertex = model_matrix * vec4(vertex, 1.0);
    gl_Position = projection_matrix * view_matrix * world_vertex;
    frag_color = color;
}
)";

const static VertexShader FlatVertexShader{{ShaderAttributeDesc("vertex", GLDataType::Vec3f),
                                            ShaderAttributeDesc("colors", GLDataType::Vec3f)},
                                           {{"model_matrix", GLDataType::Mat4f}},
                                           vert_src};

const std::string frag_src = R"(
#version 420

in vec3 frag_color;
out vec4 out_color;

void main() {
    out_color = vec4(frag_color, 1.0);
}
)";

const static FragmentShader FlatFragmentShader{{}, {}, {}, "out_color", frag_src};

std::shared_ptr<ShaderProgram> makeFlatMaterial()
{
    auto prog = ShaderProgram::create(FlatVertexShader, FlatFragmentShader, true);
    prog->renderState().blending = false;
    prog->renderState().depth_write = true;
    prog->renderState().depth_test = true;
    prog->renderState().culling = false;
    return prog;
}

} // namespace rcube