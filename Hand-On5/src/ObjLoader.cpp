#include "ObjLoader.h"
#include <vector>

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

void ObjLoader::LoadFromFile(const std::string& filename)
{
	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	//std::cout << "Pass" << std::endl;
	if (!file) {
		std::cerr << "Failed to Open File" << std::endl;
		throw std::runtime_error("Failed to open file");
	}

	/*numTriangles = 0;*/
	
	std::vector<unsigned int> Faceposition;//for face
	std::vector<unsigned int> Facetexture;//for face
	std::vector<unsigned int> Facenormal;//for face
	std::vector<glm::vec3>   positions;
	std::vector<glm::vec2>   texUvs;
	std::vector<glm::vec3>   normals;


	std::string line;
	while (std::getline(file, line)) {
		trim(line);
		if (line.substr(0, 1) == "#")
		{
			// Comment, no-op
		}
		else if (line.substr(0,2) == "v ") //if it is v
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}
		else if (line.substr(0, 2) == "vt") //if it is vt(a vertex texture coordinate)
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec2 uv;
			ss >> uv.x >> uv.y;
			texUvs.push_back(uv);
		}
		else if (line.substr(0, 2) == "vn") //if it is vn(vertex normal)
		{
			std::istringstream ss = std::istringstream(line.substr(2));
			glm::vec3 vn;
			ss >> vn.x >> vn.y>> vn.z;
			normals.push_back(vn);
		}
		else if (line.substr(0, 2) == "f ") {
			unsigned int p1, t1, n1, p2, t2, n2, p3, t3, n3;
			sscanf(line.c_str(),
				"f %d/%d/%d %d/%d/%d %d/%d/%d",
				&p1, &t1, &n1, //point texture nornaml
				&p2, &t2, &n2,
				&p3, &t3, &n3);
			Faceposition.push_back(p1);
			Faceposition.push_back(p2);
			Faceposition.push_back(p3);

			Facetexture.push_back(t1);
			Facetexture.push_back(t2);
			Facetexture.push_back(t3);

			Facenormal.push_back(n1);
			Facenormal.push_back(n2);
			Facenormal.push_back(n3);

			this->numTriangles++;
		}
	}
	this->VerticesNumbers = this->numTriangles * 3;


	std::vector<glm::vec3> indexedPositions;
	std::vector<glm::vec2> indexedTextureCoords;
	std::vector<glm::vec3> indexedNormals;
	std::vector<unsigned int> vertexIndices;

	for (unsigned int i = 0; i < this->VerticesNumbers; i++) {
		unsigned int positionIndex = Faceposition[i] - 1;
		unsigned int textureCoordIndex = Facetexture[i] - 1;
		unsigned int normalIndex = Facenormal[i] - 1;

		indexedPositions.push_back(positions[positionIndex]);
		indexedTextureCoords.push_back(texUvs[textureCoordIndex]);
		indexedNormals.push_back(normals[normalIndex]);

		vertexIndices.push_back(i);
	}

	this->mesh.Position = indexedPositions;
	this->mesh.UV = indexedTextureCoords;
	this->mesh.Normal = indexedNormals;
	this->mesh.Indices = vertexIndices;

}
