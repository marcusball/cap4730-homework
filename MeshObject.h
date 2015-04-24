#pragma once
#include <vector>

#include "RenderableObject.h"
#include "LoadedObject.h"
#include "ControlInterceptor.h"
#include "Vectors.h"
#include "Vertex.h"
#include "Picking.h"


class MeshObject : public RenderableObject, public ControlInterceptor
{
public:
	/*
	* Create a renderable single-plane grid. The grid will be a square, within the bounds of topLeft and bottomRight.
	* The grid will be within the plane in which normalAxis is the normal vector.
	* The grid will be broken up into rows and columns, both of which will contain blockCount divisions.
	*/
	MeshObject();
	~MeshObject();

	void Init(float sideLength, int pointsPerSide = 12);

	virtual void Render(RenderData renderData);
	virtual void Clear();

	/*
	* Receive keyboard input.
	* Return true if calling function should not receive the input passed to this.
	* Return false if calling function may continue to dispatch input elsewhere.
	*/
	//virtual bool KeyCallback(int key, int scancode, int action, int mods);
	virtual bool KeyCallback(int key, int scancode, int action, int mods);
	virtual bool MouseCallback(int button, int action, int mods);

	void MeshObject::CylinderFix(LoadedObject * object, Vector3f origin, float radius, float height, float angleAdjust = 0.f);

	void Load();
	void Save();

	bool HideMesh = false;
private:

	virtual void CreateVertexBuffers(const std::vector<Vertex> * const vertices, const std::vector<unsigned int> * const indices);
	void UpdateVertexBuffers(const std::vector<Vertex> * const vertices);

	/*
	* Generate all of the necessary verticies and data to display the grid.
	*/
	void GenerateVertices(float sideLength, int pointsPerSide, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices, int & vertexCount, int & pointIndexCount, int & lineIndexCount, int & triangleIndexCount);
	static int GetNeighborIndex(int currentIndex, int horizontalLength, int verticalLength, int direction);
	void ClearPickedVertices();
	void ClearPickedVertex(int id);
	bool IsVertexPicked(int id);
	int GetVertexIndexById(int id);

	int lineCount = 0;

	int pointVertexCount;
	int lineVertexCount;
	int triangleVertexCount;

	int PointsPerSide;
	int SideLength;

	bool MovingPoints = false;
	

	GLuint textureObject;

	Vector2d pickingStartPosition;

	struct PickedVertex{
		Vector4f OriginalPosition;
		Vertex * Vertex;
	};
	
	std::vector<PickedVertex> PickedVertices;

	int VertexCount;
	std::vector<Vertex> * Vertices;
};

