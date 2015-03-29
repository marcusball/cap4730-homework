#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "RenderableObject.h"
#include "LoadedObject.h"
#include "Vectors.h"
#include "Vertex.h"

struct AssemblyComponent{
	std::string File;
	LoadedObject Object; //The actual mesh that will be displayed for this component
	glm::vec3 Translation; //A the transform to connect this to a joint in the correct location
};

struct Joint{
	glm::vec4 Position; //The initial starting position for this joint, in relation to the rest of the joints
	glm::mat4 SubModelMatrixTransform; //A model matrix representing any transforms for this joint

	std::vector<AssemblyComponent> Components; //Components attached to this joint
	std::vector<Joint *> Bones;

	bool InRenderQueue = false;
};

struct AttachmentContainer{
	AttachmentContainer() : FileIndex(-1), JointIndex(-1), TransformIndex(-1), ColorIndex(-1){}

	int FileIndex;
	int JointIndex;
	int TransformIndex;
	int ColorIndex;
};

class AssembledObject : public RenderableObject
{
public:
	AssembledObject();
	~AssembledObject();

	bool LoadFromFile(std::string filename);

	virtual void Render(RenderData renderData);
	virtual void Clear();

private:
	static bool LoadObjectCombination(std::string filepath, std::vector<Joint> & structure);
	static bool LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices);

	//int vertexCount;

	Joint * RootJoint;
	std::vector<Joint> ObjectStructure;

	void RenderRecurse(Joint * renderJoint, RenderData data);
};
