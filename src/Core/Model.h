#pragma once

#include <string>
#include <vector>
#include "Mesh.h"
#include  "tiny_obj_loader.h"

class Model {
private:

	//void ProcessNode(aiNode* node, const aiScene* scene);

	std::vector<Mesh> m_meshes;

	void AddMesh(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape);

public:

	Model(const std::string& path);

	void Draw(Shader& shader);

};