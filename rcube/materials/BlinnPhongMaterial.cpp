#include "BlinnPhongMaterial.h"
#include "stb_image.h"

const std::string vert_str = \
                             R"(
#version 420
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;

layout (std140, binding=0) uniform Matrices {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 viewport_matrix;
};

uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;

out vec3 v_vertex;
out vec3 v_normal;
out vec3 v_color;
out vec2 v_texture;

void main() {
    vec4 cam_vertex = modelview_matrix * vec4(vertex, 1.0);
    gl_Position = projection_matrix * cam_vertex;
    v_vertex = cam_vertex.xyz;
    v_normal = normalize(normal_matrix * normal);
    v_color = color;
    v_texture = texcoord;
}
)";


const std::string geom_str = \
R"(
#version 420
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

layout (std140, binding=0) uniform Matrices {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 viewport_matrix;
};

in vec3 v_vertex[];
out vec3 g_vertex;
in vec3 v_normal[];
out vec3 g_normal;
in vec2 v_texture[];
out vec2 g_texture;
in vec3 v_color[];
out vec3 g_color;

noperspective out vec3 dist;

void main() {
    // Transform each vertex into viewport space
    vec3 p0 = vec3(viewport_matrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 p1 = vec3(viewport_matrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec3 p2 = vec3(viewport_matrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p0 - p1);

    // Interior angles
    float alpha = acos((b*b + c*c - a*a) / (2.0 * b * c));
    float beta = acos((a*a + c*c - b*b) / (2.0 * a * c));

    // Distance from vertex to opposite side using law of cosines
    float ha = c * sin(beta);
    float hb = c * sin(alpha);
    float hc = b * sin(alpha);

    // Emit vertex 1
    dist = vec3(ha, 0, 0);
    g_vertex = v_vertex[0];
    g_normal = v_normal[0];
    g_texture = v_texture[0];
    g_color = v_color[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    // Emit vertex 2
    dist = vec3(0, hb, 0);
    g_vertex = v_vertex[1];
    g_normal = v_normal[1];
    g_texture = v_texture[1];
    g_color = v_color[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    // Emit vertex 3
    dist = vec3(0, 0, hc);
    g_vertex = v_vertex[2];
    g_normal = v_normal[2];
    g_texture = v_texture[2];
    g_color = v_color[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}
)";


const std::string frag_str = \
R"(
#version 420

#define MAX_LIGHTS 99

// Interpolated input from geometry shader
in vec3 g_vertex;
in vec3 g_normal;
in vec2 g_texture;
in vec3 g_color;
noperspective in vec3 dist;

// Fragment shader output
out vec4 out_color;

// Scene uniforms
uniform int num_lights;

layout (std140, binding=0) uniform Matrices {
    mat4 view_matrix;
    mat4 projection_matrix;
    mat4 viewport_matrix;
};

// --------------------------------
// Light uniforms
// --------------------------------
struct Light {
    vec4 position;
    vec4 direction_radius;
    vec4 color_coneangle;
};

// uniform Light lights[MAX_LIGHTS];

layout (std140, binding=2) uniform Lights {
    Light lights[MAX_LIGHTS];
};

// --------------------------------
// Material properties
// --------------------------------
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float reflectivity;
};
uniform Material material;

layout (binding = 0) uniform sampler2D diffuse_tex;
layout (binding = 3) uniform samplerCube env_map;

uniform bool show_wireframe;
uniform bool show_backface;
uniform bool use_diffuse_texture, use_environment_map;
uniform int blend_environment_map;

struct Line {
    vec3 color;
    float thickness;
};
uniform Line line_props;

// Returns the attenuation factor that is multiplied with the light's color
float attenuation(float dist, float radius) {
    // float att = clamp(1.0 - dist * dist / (radius * radius), 0.0, 1.0);
    // return att * att;
    return 1.0 / (1.0 + (dist * dist) / (radius * radius));
}

bool close(float a, float b) {
    return abs(a - b) < 0.00001;
}

void main() {
    vec3 result = vec3(0.0);

    vec3 material_diffuse;
    if (use_diffuse_texture) {
        material_diffuse = vec3(texture(diffuse_tex, g_texture)) * g_color;
    }
    else {
        material_diffuse = material.diffuse * g_color;
    }

    vec3 N = normalize(g_normal);        // Surface normal
    vec3 V = normalize(vec3(-g_vertex)); // Surface to eye

    for (int i = 0; i < min(num_lights, MAX_LIGHTS); ++i)
    {
        vec3 L;          // Surface to light
        float att = 1.0; // Light attenuation

        if (close(lights[i].position.w, 0.0)) { // is directional?
            L = -lights[i].position.xyz;
        }
        else {
            L = lights[i].position.xyz - g_vertex;
            // att = attenuation(length(L), lights[i].radius);
            att = attenuation(length(L), lights[i].direction_radius.w);
        }
        L = normalize(L);

        // Accumulate output color using each light source
        vec3 H = normalize(L + V);  // Halfway vector
        float LdotN = dot(L, N);
        float diff_contrib = show_backface ? abs(LdotN) : max(LdotN, 0.0);
        float spec_contrib = 0.0;
        if (LdotN > 0.0) {
            spec_contrib = pow(max(0, dot(N, H)), material.shininess);
        }
        vec3 light_color = att * lights[i].color_coneangle.xyz;
        result += light_color * (diff_contrib * material_diffuse + spec_contrib * material.specular);
    }

    // Wireframe
    if (show_wireframe) {
        // Find the smallest distance
        float d = min(dist.x, dist.y);
        d = min(d, dist.z);
        float mix_val = smoothstep(line_props.thickness - 1.0, line_props.thickness + 1.0, d);
        result = mix(line_props.color, result, mix_val);
    }
    // Environment map
    if (use_environment_map) {
        vec3 I = normalize(g_vertex);
        vec3 R = reflect(I, normalize(g_normal));
        R = vec3(inverse(view_matrix) * vec4(R, 0.0));
        vec3 em = texture(env_map, R).rgb;
        if (blend_environment_map == 0) {
            result *= em;
        }
        else if (blend_environment_map == 1) {
            result += em;
        }
        else if (blend_environment_map == 2) {
            result = mix(em, result, material.reflectivity);
        }
    }

    vec3 gamma = vec3(1.0/2.2);
    vec3 final_color = pow(result, gamma);
    out_color = vec4(final_color, 1);
}
)";

BlinnPhongMaterial::BlinnPhongMaterial(glm::vec3 diffuse_color, glm::vec3 specular_color, float shininess)
    : Material(), diffuse_color(diffuse_color), specular_color(specular_color), shininess(shininess),
      reflectivity(0.5), show_wireframe(false), wireframe_thickness(1.0), wireframe_color(glm::vec3(0)),
      use_diffuse_texture(false), use_specular_texture(false), use_environment_map(false), show_backface(false) {
    diffuse_texture = std::make_shared<Texture2D>();
    specular_texture = std::make_shared<Texture2D>();
    environment_map = std::make_shared<TextureCube>();
    blend_environment_map = Blend::Multiply;
}
std::string BlinnPhongMaterial::vertexShader() {
    return vert_str;
}
std::string BlinnPhongMaterial::fragmentShader() {
    return frag_str;
}
std::string BlinnPhongMaterial::geometryShader() {
    return geom_str;
}
void BlinnPhongMaterial::use() {
    Material::use();
    if (diffuse_texture != nullptr && use_diffuse_texture) {
        diffuse_texture->use(0);
    }
    /*if (specular_texture != nullptr && use_specular_texture) {
        specular_texture->use(1);
    }*/
    if (environment_map != nullptr && use_environment_map) {
        environment_map->use(3);
    }
}
void BlinnPhongMaterial::setUniforms() {
    shader_->setUniform("material.diffuse", diffuse_color);
    shader_->setUniform("material.specular", specular_color);
    shader_->setUniform("material.shininess", shininess);
    shader_->setUniform("material.reflectivity", reflectivity);
    shader_->setUniform("show_wireframe", show_wireframe);
    shader_->setUniform("line_props.color", wireframe_color);
    shader_->setUniform("line_props.thickness", wireframe_thickness);
    shader_->setUniform("show_backface", show_backface);

    shader_->setUniform("use_diffuse_texture", use_diffuse_texture);

    shader_->setUniform("use_environment_map", use_environment_map);
    shader_->setUniform("blend_environment_map", static_cast<int>(blend_environment_map));
}

int BlinnPhongMaterial::renderPriority() const {
    return RenderPriority::Opaque;
}