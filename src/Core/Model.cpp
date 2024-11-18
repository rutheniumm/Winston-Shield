#include "Model.h"
#include "Game.h"
Model::Model(const std::string& path) {

    // TinyObjLoader's objects for loading the model
    std::string err;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // Load the OBJ file
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str());

    if (!ret) {
        print("Error loading model '" << path << "': " << err);
        return;
    }

    // Process the shapes in the OBJ file
    for (const auto& shape : shapes) {
        AddMesh(attrib, shape);
    }
}

void Model::Draw(Shader& shader) {
	for (Mesh& mesh : m_meshes)
		mesh.Draw(shader);
}

void Model::AddMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape) {
    std::vector<Vertex> vertices;
    std::vector<Index> inds;

    // Loop through the faces in the shape
    for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
        tinyobj::index_t idx = shape.mesh.indices[i];
        Vertex vertex;

        // Position data (x, y, z)
        vertex.pos = {
            attrib.vertices[3 * idx.vertex_index + 0],
            attrib.vertices[3 * idx.vertex_index + 1],
            attrib.vertices[3 * idx.vertex_index + 2]
        };

        // Normal data (if available)
        if (!attrib.normals.empty()) {
            vertex.normal = {
                attrib.normals[3 * idx.normal_index + 0],
                attrib.normals[3 * idx.normal_index + 1],
                attrib.normals[3 * idx.normal_index + 2]
            };
        }

        // Texture coordinates (if available)
    

        vertices.push_back(vertex);
        inds.push_back(i); // Indexing into the vertex array
    }

    m_meshes.emplace_back(vertices, inds);
}
