#include "LoadedObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "common/objloader.hpp"


LoadedObject::LoadedObject(){

}


LoadedObject::~LoadedObject()
{
}

void LoadedObject::SetColor(Vector4f color){
	this->overrideColor = color;
	this->overrideColorSet = true;
}

bool LoadedObject::LoadFromFile(std::string filename){
	this->Clear();
	this->isInit = true;

	glGenVertexArrays(1, &this->objectVAO);
	glBindVertexArray(this->objectVAO);

	glGenBuffers(this->objectBuffers.size(), &this->objectBuffers[0]);

	std::vector<unsigned int> indices;
	this->Vertices = new std::vector<Vertex>();
	if (LoadedObject::LoadObject(filename.c_str(), *this->Vertices, indices) == false){ //something bad happened
		return false;
	}
	this->vertexCount = indices.size();

	if (this->overrideColorSet){
		for (int i = 0; i < this->Vertices->size(); i += 1){
			(*this->Vertices)[i].Color = this->overrideColor;
		}
	}

	this->CreateVertexBuffers(this->Vertices, &indices);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere

	return true;
}

bool LoadedObject::LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices){
	printf("Loading object from file: \"%s\".\n", filepath.c_str());

	std::vector<unsigned int> positionIndices, normalIndices;
	std::vector<Vector4f> tempPositions;
	std::vector<Vector3f> tempNormals;

	FILE * file = fopen(filepath.c_str(), "r");
	if (file == NULL){
		printf("Error: Unable to open file!\n");
		return false;
	}

	while (true){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			Vector4f position;
			fscanf(file, "%f %f %f\n", &position[0], &position[1], &position[2]);
			position[3] = 1.f;
			tempPositions.push_back(position);
		}
		else if (strcmp(lineHeader, "vn") == 0){
			Vector3f normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
			tempNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0){
			//std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6){
				printf("ERROR: NO NORMALS PRESENT IN FILE! YOU NEED NORMALS FOR LIGHTING CALCULATIONS!\n");
				printf("File can't be read by our simple parser :-( Try exporting with other options. See the definition of the loadOBJ fuction.\n");
				return false;
			}
			positionIndices.push_back(vertexIndex[0]);
			positionIndices.push_back(vertexIndex[1]);
			positionIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<positionIndices.size(); i++){

		// Get the indices of its attributes
		unsigned int positionIndex = positionIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		Vector4f * position = &tempPositions[positionIndex - 1];
		Vector3f * normal = &tempNormals[normalIndex - 1];

		indices.emplace_back(i);
		vertices.emplace_back(*position, *normal);
	}

	return true;
}

void LoadedObject::Render(RenderData renderData){
	glBindVertexArray(this->objectVAO);

	glEnable(GL_POINT_SMOOTH);

	//glUniformMatrix4fv(renderData.MVPId, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(renderData.ModelMatrixId, 1, GL_FALSE, glm::value_ptr(*renderData.ModelMatrix));

	glDrawElementsBaseVertex(GL_TRIANGLES, this->vertexCount, GL_UNSIGNED_INT, 0, 0);

	glDisable(GL_POINT_SMOOTH);

	glBindVertexArray(0);
}
std::vector<Vertex> * const LoadedObject::GetVertices(){
	return this->Vertices;
}