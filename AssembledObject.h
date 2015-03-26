#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "RenderableObject.h"
#include "LoadedObject.h"
#include "Vectors.h"
#include "Vertex.h"

struct AssemblyComponent{
	LoadedObject Object; //The actual mesh that will be displayed for this component
	glm::vec4 Transpose; //A the transform to connect this to a joint in the correct location
};

struct Joint{
	glm::vec4 Position; //The initial starting position for this joint, in relation to the rest of the joints
	glm::vec4 SubModelMatrix; //A model matrix representing any transforms for this joint

	std::vector<AssemblyComponent> Components; //Components attached to this joint
	std::vector<Joint *> Bones;

	bool InRenderQueue = false;
};

class AssembledObject : public RenderableObject
{
public:
	AssembledObject();
	~AssembledObject();

	bool LoadFromFile(std::string filename);

	virtual void Render();

private:

	static bool LoadObjectCombination(std::string filepath, std::vector<Joint> & structure);
	static bool LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices);

	int vertexCount;

	Joint * RootJoint;
	std::vector<Joint> ObjectStructure;
};
