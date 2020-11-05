#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> //03
#include <string> //03
#include <math.h>
#include <vector>
#include <GLM/glm.hpp> //04
#include <glm/gtc/matrix_transform.hpp> //04

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

struct MeshData {
	std::vector<glm::vec3> Position;
	std::vector<glm::vec3> Normal;
	std::vector<glm::vec2> UV;
	std::vector<unsigned int> Indices;
	
};

class ObjLoader
{
public:
	MeshData mesh; 
	GLuint vao;
	void LoadFromFile(const std::string& filename);
	ObjLoader() = default;
	~ObjLoader() = default;
	unsigned int getVertNumber() {
		return this->VerticesNumbers;
	}
	unsigned int getTriNumber() {
		return this->numTriangles;
	}
protected:
	unsigned int numTriangles = 0;
	unsigned int VerticesNumbers = 0;
};

