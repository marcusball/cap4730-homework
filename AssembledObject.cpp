#include "AssembledObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "common/objloader.hpp"
#include <algorithm>
#include <queue>
#define _USE_MATH_DEFINES
#include <math.h>

AssembledObject::AssembledObject(){

}


AssembledObject::~AssembledObject()
{
}

void AssembledObject::Clear(){
	RenderableObject::Clear();

	this->RootJoint = nullptr;
	this->ObjectStructure.clear();
}

bool AssembledObject::LoadFromFile(std::string filename){
	this->Clear();
	this->isInit = true;

	/*glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);*/

	if (AssembledObject::LoadObjectCombination(filename.c_str(), this->ObjectStructure) == false){
		return false; //Something went horribly wrong
	}
	this->RootJoint = &this->ObjectStructure[0];

	//glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere

	return true;
}

bool AssembledObject::LoadObjectCombination(std::string filepath, std::vector<Joint> & joints){
	std::vector<std::string> files;
	//std::vector<Joint> joints;
	std::vector<Vector4f> colors;
	std::vector<glm::vec3> transformations;
	std::vector<AttachmentContainer> attachmentInfo;
	std::vector<AttachmentTransform> attachmentTransforms;
	std::vector<AssemblyComponent *> fileComponents;

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
			newJoint.SubModelMatrixTransform = glm::translate(glm::vec3(position));
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
			glm::vec3 trans;
			fscanf(file, "%f %f %f\n", &trans.x, &trans.y, &trans.z);

			transformations.push_back(trans);

			printf("Parsed Transposition [%d]: <%.3f, %.3f, %.3f>\n", transformations.size() - 1, trans.x, trans.y, trans.z);
		}
		else if (strcmp(lineHeader, "c") == 0){
			Vector4f color;
			fscanf(file, "%f %f %f\n", &color[0], &color[1], &color[2]);
			color[3] = 1.f;

			colors.push_back(color);
			printf("Parsed color [%d]: <%.3f, %.3f, %.3f>\n", colors.size() - 1, color[0], color[1], color[2]);
		}
		else if (strcmp(lineHeader, "a") == 0){
			//int fileIndex, jointIndex, transposeIndex, colorIndex;
			AttachmentContainer attachment;
			int matches = fscanf(file, "%d %d %d\n", &attachment.FileIndex, &attachment.JointIndex, &attachment.ColorIndex);
			
			if (attachment.FileIndex >= files.size()){
				printf("Error: attachment received input of invalid file index %d. Max file index is %d!\n", attachment.FileIndex, files.size() - 1);
				return false;
			}
			if (attachment.JointIndex >= joints.size()){
				printf("Error: attachment received input of invalid joint index %d. Max joint index is %d!\n", attachment.JointIndex, joints.size() - 1);
				return false;
			}
			if (attachment.ColorIndex >= colors.size()){
				printf("Error: attachment received input of invalidcolor index %d. Max color index is %d!\n", attachment.ColorIndex, colors.size() - 1);
				return false;
			}

			attachmentInfo.push_back(attachment);
		}
		else if (strcmp(lineHeader, "ta") == 0){
			AttachmentTransform attachmentTransform;
			fscanf(file, "%d %d %d %c\n", &attachmentTransform.FileIndex, &attachmentTransform.ShiftTransformIndex, &attachmentTransform.AlignTransformIndex, &attachmentTransform.AlignAlong);

			if (attachmentTransform.FileIndex >= files.size()){
				printf("Error: attachment transform received input of invalid file index %d. Max file index is %d!\n", attachmentTransform.FileIndex, files.size() - 1);
				return false;
			}
			if (attachmentTransform.ShiftTransformIndex >= transformations.size()){
				printf("Error: attachment shift transform received input of invalid transposition index %d. Max transposition index is %d!\n", attachmentTransform.ShiftTransformIndex, transformations.size() - 1);
				return false;
			}
			if (attachmentTransform.AlignTransformIndex > 0 && attachmentTransform.AlignTransformIndex >= transformations.size()){
				printf("Error: attachment align transform received input of invalid transposition index %d. Max transposition index is %d!\n", attachmentTransform.AlignTransformIndex, transformations.size() - 1);
				return false;
			}

			attachmentTransforms.push_back(attachmentTransform);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	//A first pass over the attachments to reserve spots for the components.
	//Hella necessary because I spent a day trying to figure out a bug resulting from the RenderableObjects being deconstructed when the vector is resized and reallocated.
	for (int x = 0; x < attachmentInfo.size(); x += 1){
		AttachmentContainer * attachment = &attachmentInfo[x];
		Joint * joint = &joints[attachment->JointIndex];
		joint->Components.reserve(joint->Components.capacity() + 1);
		printf("Reserved %d attachment slots for Joint %d.\n", joint->Components.capacity(), attachment->JointIndex);
	}

	for (int x = 0; x < attachmentInfo.size(); x += 1){
		AttachmentContainer * attachment = &attachmentInfo[x];
		Joint * joint = &joints[attachment->JointIndex];
		joint->Components.emplace_back();

		AssemblyComponent * newObject = &(joint->Components.back());
		fileComponents.push_back(newObject);
		newObject->AttachedTo = joint;

		if (attachment->ColorIndex != -1){
			newObject->Object.SetColor(colors[attachment->ColorIndex]);
			printf("Object %d has been given color <%.3f, %.3f, %.3f>.\n", attachment->FileIndex, colors[attachment->ColorIndex][0], colors[attachment->ColorIndex][1], colors[attachment->ColorIndex][2]);
		}

		newObject->File = files[attachment->FileIndex];
		if (newObject->Object.LoadFromFile(files[attachment->FileIndex]) == false){ //Initialize the sub-object
			printf("Error: attachment unable to load object from file \"%s\"!\n", files[attachment->FileIndex]);
			return false;
		}

		printf("Successfully loaded and attached object %d.\n", attachment->FileIndex);
	}

	for (int x = 0; x < attachmentTransforms.size(); x += 1){
		AttachmentTransform * at = &attachmentTransforms[x];
		if (at->AlignTransformIndex != -1){
			glm::vec3 targetDirection = glm::normalize(transformations[at->AlignTransformIndex]);

			glm::vec3 alignmentAxis;
			switch (at->AlignAlong){
			case Axis::X:
				alignmentAxis = AxisVector::X;
				break;
			case Axis::Y:
				alignmentAxis = AxisVector::Y;
				break;
			case Axis::Z:
			default:
				alignmentAxis = AxisVector::Z;
			}

			float rotationAngle = std::acos(glm::dot(targetDirection, alignmentAxis));
			if (std::fabs(rotationAngle) > 0.0001){
				glm::vec3 rotationAxis = glm::normalize(glm::cross(targetDirection, alignmentAxis));

				rotationAngle = -rotationAngle / M_PI*180.f; //rad to deg
				glm::mat4 rotation = glm::rotate(rotationAngle, rotationAxis);

				if (at->FileIndex < fileComponents.size() && fileComponents[at->FileIndex]){
					fileComponents[at->FileIndex]->Transform = fileComponents[at->FileIndex]->Transform * rotation;

					printf("Applied alignment for object %d.\n", at->FileIndex);
				}
			}
		}
		if (at->ShiftTransformIndex != -1){
			glm::mat4 shift = glm::translate(transformations[at->ShiftTransformIndex]);
			if (at->FileIndex < fileComponents.size() && fileComponents[at->FileIndex]){
				fileComponents[at->FileIndex]->Transform = fileComponents[at->FileIndex]->Transform * shift;
				printf("Applied shift for object %d.\n", at->FileIndex);
			}
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

void AssembledObject::Render(RenderData renderData){
	if (this->RootJoint == nullptr){ return;  }

	this->RenderRecurse(this->RootJoint, renderData);
}

void AssembledObject::RenderRecurse(Joint * renderJoint, RenderData data){
	glm::mat4 modelMatrix = renderJoint->SubModelMatrixTransform * *data.ModelMatrix;
	for (int x = 0; x < renderJoint->Components.size(); x += 1){
		AssemblyComponent * component = &renderJoint->Components[x];
		glm::mat4 componentModelMatrix = modelMatrix * component->Transform;

		RenderData subData(data);
		subData.ModelMatrix = &componentModelMatrix;

		component->Object.Render(subData);
	}

	data.ModelMatrix = &modelMatrix;
	for (int x = 0; x < renderJoint->Bones.size(); x += 1){
		this->RenderRecurse(renderJoint->Bones[x], data);
	}
}
