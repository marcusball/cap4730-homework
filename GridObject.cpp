#include "GridObject.h"


GridObject::GridObject(){

}


GridObject::~GridObject()
{
}

void GridObject::Init(float sideLength, int blockCount){
	this->Clear();
	this->isInit = true;

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);

	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	this->GenerateVertices(sideLength, blockCount, indices, vertices);

	this->CreateVertexBuffers(&vertices, &indices);

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void GridObject::GenerateVertices(float sideLength, int blockCount, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices){
	//The number of verticies needed to display a grid. These are all verticies around the outer edge of the square.
	int vertexCount = blockCount * 4; 

	//The last vertex, with the highest index. The "- 1" is because the vertex at index 0 doubles as the last, therefore, the max index is at the vertex just prior.
	int vertexMaxIndex = vertexCount - 1; 

	//The number of vertices that are on each side of the grid.
	int verticesPerSide = blockCount + 1;

	//(vertex difference) The difference between the max vertex index, and the min vertex index on any given side of the grid. 
	//Ex, in a grid divided into 5 blocks, the first side will contain vertex indices 0 through 5, the next side will be 5 -> 10. 
	int vd = blockCount;

	//For each vertex, this is the relative value (between 0 and 1) that each block's vertex will be from the outer edge's vertex.
	//That comment's probably not clear, but 0 will be an outer edge, 1 will be the other parallel edge, 
	//  and each multiple of this value (until the multiple is 1) will be another line on the grid.
	float positionFactor = 1.0 / (float)blockCount;

	float hos = sideLength / 2.f;

	outVertices.resize(vertexCount);
	for (int x = 0; x < vd; x += 1){
		//outVertices.emplace(outVertices.begin() + x, Vector4f(-1 * hos, -1* hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on left side
		//outVertices.emplace(outVertices.begin() + blockCount + x, Vector4f(-1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), hos, 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on top side
		//outVertices.emplace(outVertices.begin() + 2 * blockCount + x, Vector4f(hos, hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on right side
		//outVertices.emplace(outVertices.begin() + 3 * blockCount + x, Vector4f(hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), -1 * hos, 0.f, 1.f), Vector4f(0.f, 0.f, 1.f, 1.f)); //Vertex on bottom side

		//Vertex on left side
		outVertices[x].Position = Vector4f(-1 * hos, 0.f, -1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 1.f); 
		outVertices[x].Normal = Vector3f(0.f, 0.f, 1.f);

		//Vertex on top side
		outVertices[blockCount + x].Position = Vector4f(-1 * hos * (1 - (positionFactor * x)) + hos * (positionFactor * x), 0.f, hos, 1.f);
		outVertices[blockCount + x].Normal = Vector3f(0.f, 0.f, 1.f); 

		//Vertex on right side
		outVertices[2 * blockCount + x].Position = Vector4f(hos, 0.f, hos * (1 - (positionFactor * x)) + -1 * hos * (positionFactor * x), 1.f);
		outVertices[2 * blockCount + x].Normal = Vector3f(0.f, 0.f, 1.f); 

		//Vertex on bottom side
		outVertices[3 * blockCount + x].Position = Vector4f(hos * (1 - (positionFactor * x)) + -1 * hos * (positionFactor * x), 0.f, -1 * hos, 1.f);
		outVertices[3 * blockCount + x].Normal = Vector3f(0.f, 0.f, 1.f); 
	}

	this->lineCount = verticesPerSide * 2; // VPS multipled by 2 to account for two adjacent sides.
	outIndices.resize(this->lineCount * 2 /* the second vertex to complete a line */);
	for (int n = 0; n <= vd; n += 1){
		outIndices[2 * n] = n;
		outIndices[2 * n + 1] = (3 * vd - n); //This formula works for connecting the indices of the first side (indices 0 to vd), to the opposite side. (2*vd to 3*vd).
	}

	for (int n = vd; n <= 2 * vd; n+=1){
		int nshift = n + 1; //We need to shift the n value up one for use in the index to account for the overlap of n=vd in the previous loop.
		outIndices[2 * nshift] = n;
		outIndices[2 * nshift + 1] = (4 * vd - (n - vd)) % (vertexCount); //Connect the second side (index vd through 2*vd) to the last side (3*vd through 4*vd, however 4*vd wraps around to the 0 index).
	}
}

void GridObject::Render(){
	glBindVertexArray(this->objectVAO);

	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glDrawElementsBaseVertex(GL_LINES, this->lineCount * 2, GL_UNSIGNED_INT, 0, 0);

	glDisable(GL_POINT_SMOOTH);
	//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}
