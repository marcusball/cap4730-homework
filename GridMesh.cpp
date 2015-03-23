#include "GridMesh.h"


GridMesh::GridMesh(float sideLength, int blockCount){
}


GridMesh::~GridMesh()
{
}

void GridMesh::GenerateVertices(float sideLength, int blockCount, std::vector<int> & outIndices, std::vector<Vertex> & outVertices){
	//The number of verticies needed to display a grid. These are all verticies around the outer edge of the square.
	int vertexCount = blockCount * 4; 

	//The last vertex, with the highest index. The "- 1" is because the vertex at index 0 doubles as the last, therefore, the max index is at the vertex just prior.
	int vertexMaxIndex = vertexCount - 1; 

	//For each vertex, this is the relative value (between 0 and 1) that each block's vertex will be from the outer edge's vertex.
	//That comment's probably not clear, but 0 will be an outer edge, 1 will be the other parallel edge, 
	//  and each multiple of this value (until the multiple is 1) will be another line on the grid.
	float positionFactor = 1.0 / (float)blockCount;

	float hos = sideLength / 2.f;

	outVertices.reserve(vertexCount);
	for (int x = 0; x < blockCount; x += 1){
		outVertices.emplace(outVertices.begin() + x, Vector4f(-1 * hos, -1* hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on left side
		outVertices.emplace(outVertices.begin() + blockCount + x, Vector4f(-1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), hos, 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on top side
		outVertices.emplace(outVertices.begin() + 2 * blockCount + x, Vector4f(hos, -1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on right side
		outVertices.emplace(outVertices.begin() + 3 * blockCount + x, Vector4f(-1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), -1 * hos, 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on bottom side
	}
}

void GridMesh::Render(){

}
