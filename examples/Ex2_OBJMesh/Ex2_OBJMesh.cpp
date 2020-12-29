#include "RCube/Core/Graphics/MeshGen/Obj.h"
#include "RCubeViewer/RCubeViewer.h"

int main()
{
    using namespace rcube;

    // Properties to configure the viewer
    viewer::RCubeViewerProps props;
    props.resolution = glm::vec2(1280 /*4096*/, 720 /*2160*/); // 720p
    props.MSAA = 2;                                            // turn on 2x multisampling

    // Create a viewer
    viewer::RCubeViewer viewer(props);

    // Load obj file
    std::string input_obj_file = std::string(OBJ_RESOURCE_PATH) + "/" + "armadillo.obj";
    TriangleMeshData mesh = rcube::loadOBJ(input_obj_file);

    // Make sure the mesh is OK
    assert(mesh.valid() && "Input OBJ file is not valid");

    // Scale the mesh into a unit cube and translate to origin
    mesh.scaleAndCenter();

    // Add the loaded obj to the viewer
    EntityHandle mesh_handle = viewer.addSurface("OBJMesh", mesh);

    // Change its diffuse color by getting the Drawable component
    auto material = mesh_handle.get<Material>();
    material->albedo = glm::vec3(0.0, 0.3, 0.7);
    material->roughness = 0.5f;

    // Show viewer
    viewer.execute();

    return 0;
}