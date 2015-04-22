#include "MeshObject.h"


MeshObject::MeshObject(){

}


MeshObject::~MeshObject()
{
}

void MeshObject::Init(float sideLength, int blockCount){
	this->Clear();
	this->isInit = true;

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);

	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	this->GenerateVertices(sideLength, blockCount, indices, vertices, this->pointVertexCount, this->lineVertexCount);

	this->CreateVertexBuffers(&vertices, &indices);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void MeshObject::GenerateVertices(float sideLength, int pointsPerSide, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices, int & pointIndexCount, int & lineIndexCount){
	if (pointsPerSide < 2){ return; }

	//The number of verticies needed to display a grid. 
	int vertexCount = pointsPerSide * pointsPerSide;

	//The last vertex, with the highest index.
	int vertexMaxIndex = vertexCount;

	//distance between two neighboring points
	float distanceDelta = sideLength / (pointsPerSide - 1.f); 

	float hos = sideLength / 2.f;

	outVertices.resize(vertexCount);
	for (int x = 0; x < pointsPerSide; x += 1){
		for (int y = 0; y < pointsPerSide; y += 1){
			outVertices[x * pointsPerSide + y].Position = Vector4f(5, y * distanceDelta, -1 * hos + x * distanceDelta, 1.f);
			outVertices[x * pointsPerSide + y].Normal = Vector3f(0.f, 1.f, 0.f);
		}
	}

	//The number of lines necessary to connect all of these vertices into a mesh.
	//The number of lines to connect one edge of vertices, times the number of parallel edges, times 2 for the perpendicular direction.
	int lineCount = (pointsPerSide - 1) * (pointsPerSide) * 2;

	//The number of indices needed to draw the vertices.
	pointIndexCount = vertexCount;

	//the number of indices needed to draw all of the lines
	lineIndexCount = lineCount * 2;

	//The total number of indices that will be passed to the shader.
	int VBOIndexCount = pointIndexCount + lineIndexCount;
	outIndices.resize(VBOIndexCount);
	for (int x = 0; x < vertexCount; x += 1){
		outIndices[x] = x;
	}

	int indexShift = vertexCount;
	for (int x = 0; x < vertexCount;x+=1){
		int outIndex = indexShift;
		int localIndexShift = 0;
		
		int rightVertexIndex = MeshObject::GetNeighborIndex(x, pointsPerSide, pointsPerSide, 0); //Index of vertex to the right
		if (rightVertexIndex != -1){
			outIndices[outIndex + localIndexShift++] = x;
			outIndices[outIndex + localIndexShift++] = rightVertexIndex;
		}
		int downVertexIndex = MeshObject::GetNeighborIndex(x, pointsPerSide, pointsPerSide, 1); //Index of vertex to the right
		if (downVertexIndex != -1){
			outIndices[outIndex + localIndexShift++] = x;
			outIndices[outIndex + localIndexShift++] = downVertexIndex;
		}
		indexShift += localIndexShift;
	}
}

int MeshObject::GetNeighborIndex(int currentIndex, int horizontalPointCount, int verticalPointCount, int direction){
	switch (direction){
		case(0) : { //To the right
			int horizontalIndex = currentIndex % horizontalPointCount;
			if (horizontalIndex >= (horizontalPointCount - 1)){ //if the current index is the rightmost point
				return -1; //There's nothing to the right of this point
			}
			else{
					return currentIndex + 1;
			}
			break;
		}
		case(1) : { //Downward
			if (currentIndex < horizontalPointCount * (verticalPointCount - 1)){
				return currentIndex + horizontalPointCount;
			}
			else{
				return -1;
			}
			break;
		}
		default:
			return -1;
	}
}

void MeshObject::Render(RenderData renderData){
	glBindVertexArray(this->objectVAO);

	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glDrawElementsBaseVertex(GL_POINTS, this->pointVertexCount, GL_UNSIGNED_INT, 0, 0);
	glDrawElementsBaseVertex(GL_LINES, this->lineVertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * this->pointVertexCount), 0);

	glDisable(GL_POINT_SMOOTH);
	//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}
