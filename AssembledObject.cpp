#include "AssembledObject.h"
#include <glm/glm.hpp>
#include "common/objloader.hpp"
#include <algorithm>
#include <queue>

AssembledObject::AssembledObject(){

}


AssembledObject::~AssembledObject()
{
}

bool AssembledObject::LoadFromFile(std::string filename){
	this->Clear();
	this->isInit = true;

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);

	if (AssembledObject::LoadObjectCombination(filename.c_str(), this->ObjectStructure) == false){
		return false; //Something went horribly wrong
	}
	this->RootJoint = &this->ObjectStructure[0];

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere

	return true;
}

bool AssembledObject::LoadObjectCombination(std::string filepath, std::vector<Joint> & joints){
	std::vector<std::string> files;
	//std::vector<Joint> joints;
	std::vector<Vector4f> colors;
	std::vector<glm::vec4> transpositions;

	printf("Loading object collection from file: \"%s\".\n", filepath.c_str());

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

		if (strcmp(lineHeader, "f") == 0){
			//std::string filename;
			char filename[256];
			fgets(filename, 255, file);
			files.emplace_back(filename);
			
			//Remove undesired characters
			char toRemove[] = " \"\n";
			for (unsigned int i = 0; i < strlen(toRemove); i++){
				files.back().erase(std::remove(files.back().begin(), files.back().end(), toRemove[i]), files.back().end());
			}

			printf("Parsed File [%d]: \"%s\"\n", files.size() - 1, files.back().c_str());
		}
		else if (strcmp(lineHeader, "j") == 0){
			glm::vec4 position;
			fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
			position.w = 1.f;
			
			Joint newJoint;
			newJoint.Position = position;
			joints.push_back(newJoint);

			Joint * debug = &joints[joints.size() - 1]; //Pointer to the newly inserted joint
			printf("Parsed Joint [%d]: <%.3f, %.3f, %.3f>\n", joints.size() - 1, debug->Position.x, debug->Position.y, debug->Position.z);
		}
		else if (strcmp(lineHeader, "b") == 0){
			int rootJoint, attachedJoint;
			fscanf(file, "%d %d\n", &attachedJoint, &rootJoint);

			if (rootJoint >= joints.size()){
				printf("Error: bone received input of invalid joint index %d. Max joint index is %d!\n", rootJoint, joints.size() - 1);
				return false;
			}
			if (attachedJoint >= joints.size()){
				printf("Error: bone received input of invalid joint index %d. Max joint index is %d!\n", attachedJoint, joints.size() - 1);
				return false;
			}

			Joint * root = &joints[rootJoint];
			root->Bones.push_back(&joints[attachedJoint]);

			Joint * debugA = root; //Pointer to the newly inserted joint
			Joint * debugR = root->Bones.back();
			printf("Parsed Bone: Connecting joint %d <%.3f, %.3f, %.3f> to joint %d <%.3f, %.3f, %.3f>\n", 
				attachedJoint, debugA->Position.x, debugA->Position.y, debugA->Position.z,
				rootJoint, debugR->Position.x, debugR->Position.y, debugR->Position.z);
		}
		else if (strcmp(lineHeader, "t") == 0){
			glm::vec4 trans;
			fscanf(file, "%f %f %f\n", &trans.x, &trans.y, &trans.z);
			trans.w = 1.f;

			transpositions.push_back(trans);

			printf("Parsed Transposition [%d]: <%.3f, %.3f, %.3f>\n", transpositions.size() - 1, trans.x, trans.y, trans.z);
		}
		else if (strcmp(lineHeader, "c") == 0){
			Vector4f color;
			fscanf(file, "%f %f %f\n", &color[0], &color[1], &color[2]);
			color[3] = 1.f;

			colors.push_back(color);
			printf("Parsed color [%d]: <%.3f, %.3f, %.3f>\n", colors.size() - 1, color[0], color[1], color[2]);
		}
		else if (strcmp(lineHeader, "a") == 0){
			int fileIndex, jointIndex, transposeIndex, colorIndex;
			int matches = fscanf(file, "%d %d %d %d\n", &fileIndex, &jointIndex, &transposeIndex, &colorIndex);

			if (fileIndex >= files.size()){
				printf("Error: attachment received input of invalid file index %d. Max file index is %d!\n", fileIndex, files.size() - 1);
				return false;
			}
			if (jointIndex >= joints.size()){
				printf("Error: attachment received input of invalid joint index %d. Max joint index is %d!\n", jointIndex, joints.size() - 1);
				return false;
			}

			if (matches >= 2){ //If we received the file and joint indexes 
				Joint * joint = &joints[jointIndex];
				joint->Components.emplace_back();

				AssemblyComponent * newObject = &(joint->Components.back());
				
				if (matches >= 3){ //if we also received a transpose index
					if (transposeIndex >= transpositions.size()){
						printf("Error: attachment received input of invalid transposition index %d. Max transposition index is %d!\n", transposeIndex, transpositions.size() - 1);
						return false;
					}

					newObject->Transpose = transpositions[transposeIndex];
					printf("Object %d will receive transpose %d for attachment.\n", fileIndex, transposeIndex);

					if (matches >= 4){ // received a color index
						if (colorIndex >= colors.size()){
							printf("Error: attachment received input of invalidcolor index %d. Max color index is %d!\n", colorIndex, colors.size() - 1);
							return false;
						}

						newObject->Object.SetColor(colors[colorIndex]);
						printf("Object %d has been given color <%.3f, %.3f, %.3f>.\n", fileIndex, colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
					}
				}

				if (newObject->Object.LoadFromFile(files[fileIndex]) == false){ //Initialize the sub-object
					printf("Error: attachment unable to load object from file \"%s\"!\n", files[fileIndex]);
					return false;
				}

				printf("Successfully loaded and attached object %d.\n", fileIndex);
			}
			else{
				printf("Error: attachment received invalid number of inputs (%d)!\n", matches);
				return false;
			}
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	return true;
}

bool AssembledObject::LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices){
	printf("Loading object collection from file: \"%s\".\n", filepath.c_str());

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

void AssembledObject::Render(){
	for (int x = 0; x < this->ObjectStructure.size(); x += 1){
		this->ObjectStructure[x].InRenderQueue = false;
	}

	std::vector<bool> visitedJoints;
	visitedJoints.resize(this->ObjectStructure.size());

	std::queue<Joint *> RenderQueue;
	RenderQueue.push(this->RootJoint);
	this->RootJoint->InRenderQueue = true;
	while (!RenderQueue.empty()){
		Joint * currentJoint = RenderQueue.front();
		RenderQueue.pop();

		int numComponents = currentJoint->Components.size();
		for (int x = 0; x < numComponents; x += 1){
			currentJoint->Components[x].Object.Render();
		}

		int bones = currentJoint->Bones.size();
		for (int x = 0; x < bones; x += 1){
			if (currentJoint->Bones[x]->InRenderQueue == false){
				RenderQueue.push(currentJoint->Bones[x]);
				currentJoint->Bones[x]->InRenderQueue = true;
			}
		}
	}
}