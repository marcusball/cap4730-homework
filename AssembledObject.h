#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "RenderableObject.h"
#include "LoadedObject.h"
#include "ControlInterceptor.h"
#include "Vectors.h"
#include "Vertex.h"
#include "Axis.h"

struct AssemblyComponent;
struct Joint;

struct Joint{
	glm::vec4 Position; //The initial starting position for this joint, in relation to the rest of the joints
	//glm::mat4 SubModelMatrixTransform; //A model matrix representing any transforms for this joint
	glm::mat4 Rotation;
	glm::mat4 Translation;

	std::vector<AssemblyComponent> Components; //Components attached to this joint
	std::vector<Joint *> Bones;

	bool InRenderQueue = false;
};

struct AssemblyComponent{
	AssemblyComponent() : Transform(1.f), AttachedTo(nullptr){};

	std::string File;
	LoadedObject Object; //The actual mesh that will be displayed for this component
	//glm::vec3 Translation; //A the transform to connect this to a joint in the correct location
	glm::mat4 Transform;

	Vector4f Color;

	Joint * AttachedTo;
};

struct AttachmentContainer{
	AttachmentContainer() : FileIndex(-1), JointIndex(-1), TransformIndex(-1), ColorIndex(-1){}

	int FileIndex;
	int JointIndex;
	int TransformIndex;
	int ColorIndex;
};

struct AttachmentTransform{
	AttachmentTransform() : FileIndex(-1), AlignTransformIndex(-1), ShiftTransformIndex(-1), AlignAlong(Axis::Z){}

	int FileIndex;
	int AlignTransformIndex;
	int ShiftTransformIndex;
	Axis AlignAlong;
};

struct SelectKeyBinding{
	GLuint KeyId;
	int FileIndex;

	AssemblyComponent * BoundComponent;
};

struct DirectionKeyBinding{
	DirectionKeyBinding() : LeftRightMovement(Axis::None), UpDownMovement(Axis::None), ShiftLeftRightMovement(Axis::None){}

	int FileIndex;
	Axis LeftRightMovement;
	Axis UpDownMovement;
	Axis ShiftLeftRightMovement;

	AssemblyComponent * BoundComponent;
};

class AssembledObject : public RenderableObject, public ControlInterceptor
{
public:
	AssembledObject();
	~AssembledObject();

	bool LoadFromFile(std::string filename);

	virtual void Render(RenderData renderData);
	virtual void Clear();

	/*
	 * Receive keyboard input. 
	 * Return true if calling function should not receive the input passed to this.
	 * Return false if calling function may continue to dispatch input elsewhere. 
	 */
	virtual bool KeyCallback(int key, int scancode, int action, int mods);
	virtual bool MouseCallback(int button, int action, int mods);

	Joint * RootJoint;
private:
	static bool LoadObjectCombination(std::string filepath, std::vector<Joint> & structure, std::vector<SelectKeyBinding> & keyBindings, std::vector<DirectionKeyBinding> & moveKeyBindings);
	static bool LoadObject(std::string filepath, std::vector<Vertex> & vertices, std::vector<unsigned int> & indices);

	//int vertexCount;

	
	std::vector<Joint> ObjectStructure;
	std::vector<SelectKeyBinding> SelectBindings;
	std::vector<DirectionKeyBinding> MovementBindings;

	AssemblyComponent * SelectedComponent;

	void RenderRecurse(Joint * renderJoint, RenderData data);

	bool hide = false;
};
