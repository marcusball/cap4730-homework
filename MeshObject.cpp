#include "MeshObject.h"
#include "tga.h"


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
	this->GenerateVertices(sideLength, blockCount, indices, vertices, this->pointVertexCount, this->lineVertexCount, this->triangleVertexCount);

	this->CreateVertexBuffers(&vertices, &indices);

	long width, height;
	this->textureObject = load_texture_TGA("models/Ball.tga", &width, &height, GL_REPEAT, GL_REPEAT);
	if (width != 0 && height != 0){
		printf("Loaded texture (%d) (%d by %d)\n", this->textureObject, width, height);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	}

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void MeshObject::GenerateVertices(float sideLength, int pointsPerSide, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices, int & pointIndexCount, int & lineIndexCount, int & triangleIndexCount){
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
			Vertex * vtx = &outVertices[x * pointsPerSide + y];
			vtx->Position = Vector4f(5, y * distanceDelta, -1 * hos + x * distanceDelta, 1.f);
			vtx->Normal = Vector3f(0.f, 1.f, 0.f);
			vtx->Size = 5.f;

			float s = (float)x / (float)(pointsPerSide * 10.f);
			float t = (float)y / (float)(pointsPerSide * 10.f);
			//if (s > 1.f || t > 1.f){
				//printf("<%.3f, %.3f>\n", s, t);
			//}
			vtx->Texture = Vector2f(s, t);
		}
	}

	//The number of lines necessary to connect all of these vertices into a mesh.
	//The number of lines to connect one edge of vertices, times the number of parallel edges, times 2 for the perpendicular direction.
	int lineCount = (pointsPerSide - 1) * (pointsPerSide) * 2;

	//The number of indices needed to draw the vertices.
	pointIndexCount = vertexCount;

	//the number of indices needed to draw all of the lines
	lineIndexCount = lineCount * 2;

	triangleIndexCount = (pointsPerSide - 1) * (pointsPerSide - 1) * 6;

	//The total number of indices that will be passed to the shader.
	int VBOIndexCount = pointIndexCount + lineIndexCount + triangleIndexCount;
	outIndices.resize(VBOIndexCount);
	for (int x = 0; x < vertexCount; x += 1){
		outIndices[x] = x;
	}

	int indexShift = pointIndexCount;
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

	indexShift = pointIndexCount + lineIndexCount;
	for (int x = 0; x < vertexCount; x += 1){
		int outIndex = indexShift;
		int localIndexShift = 0;

		int rightVertexIndex = MeshObject::GetNeighborIndex(x, pointsPerSide, pointsPerSide, 0); //Index of vertex to the right
		int downVertexIndex = MeshObject::GetNeighborIndex(x, pointsPerSide, pointsPerSide, 1); //Index of vertex to the right
		int downRightVertexIndex = MeshObject::GetNeighborIndex(x, pointsPerSide, pointsPerSide, 2);

		if (rightVertexIndex != -1 && downVertexIndex != -1 && downRightVertexIndex != -1){
			outIndices[outIndex + localIndexShift++] = x;
			outIndices[outIndex + localIndexShift++] = rightVertexIndex;
			outIndices[outIndex + localIndexShift++] = downVertexIndex;

			outIndices[outIndex + localIndexShift++] = rightVertexIndex;
			outIndices[outIndex + localIndexShift++] = downRightVertexIndex;
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
		case(2) : { //Downward and right
			int horizontalIndex = currentIndex % horizontalPointCount;
			if (currentIndex < horizontalPointCount * (verticalPointCount - 1) && horizontalIndex < (horizontalPointCount - 1)){
				return currentIndex + 1 + horizontalPointCount;
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
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);

	glDrawElementsBaseVertex(GL_POINTS, this->pointVertexCount, GL_UNSIGNED_INT, 0, 0);
	glDrawElementsBaseVertex(GL_LINES, this->lineVertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * this->pointVertexCount), 0);
	glDrawElementsBaseVertex(GL_TRIANGLES, this->triangleVertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)* (this->pointVertexCount + this->lineVertexCount)), 0);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

	glBindVertexArray(0);
}


void MeshObject::CreateVertexBuffers(const std::vector<Vertex> * const vertices, const std::vector<unsigned int> * const indices){
	//// Create the VAO
	//glGenVertexArrays(1, &this->objectVAO);
	//glBindVertexArray(this->objectVAO);

	//// Create the buffers for the vertices atttributes
	//glGenBuffers(this->objectBuffers.size(), &this->objectBuffers[0]);


	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<Vector4f> normals;
	std::vector<glm::vec2> textures;
	std::vector<float> sizes;

	positions.reserve(vertices->size());
	normals.reserve(vertices->size());
	sizes.reserve(vertices->size());
	for (int x = 0; x < vertices->size(); x += 1){
		const Vertex * vtx = &(*vertices)[x];
		positions.push_back(vtx->Position);
		colors.push_back(vtx->Color);
		normals.push_back(vtx->Normal);
		textures.push_back(glm::vec2(vtx->Position[0], vtx->Position[1]));
		sizes.push_back((*vertices)[x].Size);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, positions[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[COLOR_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, colors[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[SIZE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sizes[0]) * sizes.size(), &sizes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, normals[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[TEXTURE_COORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures[0]) * textures.size(), &textures[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->objectBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof((*indices)[0]) * indices->size(), &(*indices)[0], GL_STATIC_DRAW);
}