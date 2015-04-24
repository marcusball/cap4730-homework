#define _USE_MATH_DEFINES
#include <math.h>

#include "MeshObject.h"
#include "tga.h"
#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


MeshObject::MeshObject(){

}


MeshObject::~MeshObject()
{
}

void MeshObject::Init(float sideLength, int pointsPerSide){
	this->Clear();
	this->isInit = true;
	this->PointsPerSide = pointsPerSide;

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(objectBuffers.size(), &objectBuffers[0]);

	std::vector<unsigned int> indices;
	this->Vertices = new std::vector<Vertex>();
	this->GenerateVertices(sideLength, this->PointsPerSide, indices, *this->Vertices, this->VertexCount, this->pointVertexCount, this->lineVertexCount, this->triangleVertexCount);

	this->CreateVertexBuffers(this->Vertices, &indices);

	long width, height;
	this->textureObject = load_texture_TGA("models/Ball.tga", &width, &height, GL_REPEAT, GL_REPEAT);
	if (width != 0 && height != 0){
		printf("Loaded texture (%d) (%d by %d)\n", this->textureObject, width, height);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	}

	glBindVertexArray(0); //Unbind the VAO so it's not changed elsewhere
}

void MeshObject::GenerateVertices(float sideLength, int pointsPerSide, std::vector<unsigned int> & outIndices, std::vector<Vertex> & outVertices, int & vertexCount, int & pointIndexCount, int & lineIndexCount, int & triangleIndexCount){
	if (pointsPerSide < 2){ return; }

	//The number of verticies needed to display a grid. 
	vertexCount = pointsPerSide * pointsPerSide;

	//The last vertex, with the highest index.
	int vertexMaxIndex = vertexCount;

	//distance between two neighboring points
	float distanceDelta = sideLength / (pointsPerSide - 1.f); 

	float hos = sideLength / 2.f;

	//Multiplied by two so the first set of vertexCount vertices will be colored, pickable points
	//The second half will be white with texture coordinates
	outVertices.resize(vertexCount * 2);
	for (int x = 0; x < pointsPerSide; x += 1){
		for (int y = 0; y < pointsPerSide; y += 1){
			Vertex * vtx = &outVertices[x * pointsPerSide + y];
			vtx->Id = x * pointsPerSide + y;
			vtx->Position = Vector4f(5, y * distanceDelta, -1 * hos + x * distanceDelta, 1.f);
			vtx->Normal = Vector3f(0.f, 1.f, 0.f);
			vtx->Size = 5.f;
			vtx->Color = ColorVectors::GREEN;

			Vertex * ivtx = &outVertices[(x * pointsPerSide + y) + vertexCount]; //A vertex on which the image will be projected
			float s = (float)x / (float)pointsPerSide;
			float t = (float)y / (float)pointsPerSide;
			
			ivtx->Position = Vector4f(5, y * distanceDelta, -1 * hos + x * distanceDelta, 1.f);
			ivtx->Normal = Vector3f(0.f, 1.f, 0.f);
			ivtx->Texture = Vector2f(s, t);
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
			outIndices[outIndex + localIndexShift++] = vertexCount + x;
			outIndices[outIndex + localIndexShift++] = vertexCount + rightVertexIndex;
			outIndices[outIndex + localIndexShift++] = vertexCount + downVertexIndex;

			outIndices[outIndex + localIndexShift++] = vertexCount + rightVertexIndex;
			outIndices[outIndex + localIndexShift++] = vertexCount + downRightVertexIndex;
			outIndices[outIndex + localIndexShift++] = vertexCount + downVertexIndex;
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

	if (this-MovingPoints){
		Game * game = Game::GetInstance();
		Vector2d position = game->GetCursorPosition();
		//printf("(%d, %d)\n", position[0], position[1]);

		Vector2f relativeWindow = Vector2f(position[0] / (float)game->RESOLUTION_WIDTH, position[1] / (float)game->RESOLUTION_HEIGHT);
		Vector2f relativeWindowOriginal = Vector2f(this->pickingStartPosition[0] / (float)game->RESOLUTION_WIDTH, this->pickingStartPosition[1] / (float)game->RESOLUTION_HEIGHT);

		Vector2f delta = relativeWindow - relativeWindowOriginal;
		//printf("Delta: (%.3f, %.3f)\n", delta[0], delta[1]);

		glm::vec4 movement = glm::vec4(delta[0],delta[1],0,0);
		glm::mat4 mvp = (*renderData.ProjectionMatrix) * (*renderData.ViewMatrix) * (*renderData.ModelMatrix);

		for (int x = 0; x < this->PickedVertices.size(); x += 1){
			PickedVertex * picked = &this->PickedVertices[x];
			Vertex * textureVertex = &(*this->Vertices)[picked->Vertex->Id + this->VertexCount];

			glm::vec4 originalPositon = glm::vec4(picked->OriginalPosition[0], picked->OriginalPosition[1], picked->OriginalPosition[2], picked->OriginalPosition[3]);
			glm::vec4 cameraPosition = mvp * originalPositon;
			glm::vec4 cameraDelta = mvp * movement * 2.f;
			glm::vec4 pointMovement = originalPositon + cameraDelta;
			Vector4f newPosition = Vector4f(pointMovement.x, pointMovement.y, pointMovement.z, 1);

			//printf("Pos: (%.3f, %.3f, %.3f) Mov: (%.3f, %.3f,%.3f)\n", cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraDelta.x, cameraDelta.y, cameraDelta.z);

			picked->Vertex->Color = ColorVectors::RED;
			picked->Vertex->Size = 10.f;
			picked->Vertex->Position = newPosition;

			textureVertex->Position = newPosition;
		}
		this->UpdateVertexBuffers(this->Vertices);
	}

	if (renderData.RenderType != RenderType::Picking){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureObject);
	}

	glDrawElementsBaseVertex(GL_POINTS, this->pointVertexCount, GL_UNSIGNED_INT, 0, 0);
	if (renderData.RenderType != RenderType::Picking){
		glDrawElementsBaseVertex(GL_LINES, this->lineVertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)* this->pointVertexCount), 0);
		glDrawElementsBaseVertex(GL_TRIANGLES, this->triangleVertexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)* (this->pointVertexCount + this->lineVertexCount)), 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

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

	std::vector<unsigned int> ids;
	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<Vector4f> normals;
	std::vector<Vector2f> textures;
	std::vector<float> sizes;

	positions.reserve(vertices->size());
	normals.reserve(vertices->size());
	sizes.reserve(vertices->size());
	for (int x = 0; x < vertices->size(); x += 1){
		const Vertex * vtx = &(*vertices)[x];
		ids.push_back(vtx->Id);
		positions.push_back(vtx->Position);
		colors.push_back(vtx->Color);
		normals.push_back(vtx->Normal);
		textures.push_back(vtx->Texture);
		sizes.push_back((*vertices)[x].Size);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[ID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ids[0]) * ids.size(), &ids[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, 0);

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
	glVertexAttribPointer(5, textures[0].value.size(), GL_FLOAT, GL_FALSE, 0, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->objectBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof((*indices)[0]) * indices->size(), &(*indices)[0], GL_STATIC_DRAW);
}

void MeshObject::UpdateVertexBuffers(const std::vector<Vertex> * const vertices){
	//// Create the VAO
	//glGenVertexArrays(1, &this->objectVAO);
	//glBindVertexArray(this->objectVAO);

	//// Create the buffers for the vertices atttributes
	//glGenBuffers(this->objectBuffers.size(), &this->objectBuffers[0]);

	std::vector<unsigned int> ids;
	std::vector<Vector4f> positions;
	std::vector<Vector4f> colors;
	std::vector<Vector4f> normals;
	std::vector<Vector2f> textures;
	std::vector<float> sizes;

	positions.reserve(vertices->size());
	normals.reserve(vertices->size());
	sizes.reserve(vertices->size());
	for (int x = 0; x < vertices->size(); x += 1){
		const Vertex * vtx = &(*vertices)[x];
		ids.push_back(vtx->Id);
		positions.push_back(vtx->Position);
		colors.push_back(vtx->Color);
		normals.push_back(vtx->Normal);
		textures.push_back(vtx->Texture);
		sizes.push_back((*vertices)[x].Size);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[ID_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ids[0]) * ids.size(), &ids[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[COLOR_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[SIZE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sizes[0]) * sizes.size(), &sizes[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->objectBuffers[TEXTURE_COORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures[0]) * textures.size(), &textures[0], GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureObject);
}

bool MeshObject::KeyCallback(int key, int scancode, int action, int mods){
	return false;
}
bool MeshObject::MouseCallback(int button, int action, int mods){
	Game * game = Game::GetInstance();

	PixelData data = game->GetPixelSelected();
	Vector2d position = game->GetCursorPosition();
	if (data.ObjectId == this->objectId){
		if (button == GLFW_MOUSE_BUTTON_LEFT){
			if (action == GLFW_PRESS){
				printf("(%d, %d) -> <%.3f,%.3f,%.3f>\n", position[0], position[1], data.ObjectId, data.PointId, data.PrimitiveId);
				this->MovingPoints = true;
				this->pickingStartPosition = position;
				Vertex * vertex = &(*this->Vertices)[static_cast<int>(data.PointId)];
				
				PickedVertex pickedVertex;
				pickedVertex.Vertex = vertex;
				pickedVertex.OriginalPosition = vertex->Position;

				if (mods & GLFW_MOD_CONTROL){
					if (!this->IsVertexPicked(vertex->Id)){
						this->PickedVertices.push_back(pickedVertex);
					}
					else{
						this->ClearPickedVertex(pickedVertex.Vertex->Id);
					}
				}
				else{
					this->ClearPickedVertices();
					this->PickedVertices.push_back(pickedVertex);
				}
			}
			
		}
	}

	if (action == GLFW_RELEASE){
		if (!(mods & GLFW_MOD_CONTROL)){
			if (data.ObjectId != this->objectId){
				this->ClearPickedVertices();
				this->MovingPoints = false;
			}
			else{
				if (this->PickedVertices.size() == 1){

					this->ClearPickedVertices();
					this->MovingPoints = false;
				}
			}
		}
	}
	
	return false;
}

void MeshObject::ClearPickedVertices(){
	for (int x = 0; x < this->PickedVertices.size(); x += 1){
		this->PickedVertices[x].Vertex->Color = ColorVectors::GREEN;
		this->PickedVertices[x].Vertex->Size = 5.f;
	}
	this->PickedVertices.clear();
}

void MeshObject::ClearPickedVertex(int id){
	int index = this->GetVertexIndexById(id);

	this->PickedVertices[index].Vertex->Color = ColorVectors::GREEN;
	this->PickedVertices[index].Vertex->Size = 5.f;
	this->PickedVertices.erase(this->PickedVertices.begin() + index);
}

bool MeshObject::IsVertexPicked(int id){
	return (this->GetVertexIndexById(id) != -1);
}

int MeshObject::GetVertexIndexById(int id){
	for (int x = 0; x < this->PickedVertices.size(); x += 1){
		if (this->PickedVertices[x].Vertex->Id == id){
			return x;
		}
	}
	return -1;
}

void MeshObject::CylinderFix(LoadedObject * object, Vector3f origin, float radius, float height, float angleAdjust){
	std::vector<Vertex> * const vertices = object->GetVertices();
	float minDistance = 10000000000.f;
	int best = -1;

	float smallest = 10000000000.f;
	for (int x = 0; x < this->VertexCount; x += 1){
		float circlePart = (x / (this->PointsPerSide)) / (float)(this->PointsPerSide - 1);
		float heightPart = (x % this->PointsPerSide) / (float)this->PointsPerSide;
		float rayRadius = radius;
		int tries = 0;

		while (best == -1 && tries < 15){
			float xpos = std::cos((circlePart * -360.f + angleAdjust) * M_PI / 180.f) * rayRadius;
			float ypos = std::sin((circlePart * -360.f + angleAdjust) * M_PI / 180.f) * rayRadius;
			Vector4f PointPosition = Vector4f(ypos, heightPart * height, xpos, 1.f);

			(*this->Vertices)[x].Position = PointPosition;
			(*this->Vertices)[x + this->VertexCount].Position = PointPosition;

			for (int y = 0; y < vertices->size(); y += 1){
				float distance = PointPosition.DistanceTo((*vertices)[y].Position);
				if (distance < .25f && distance < minDistance){
					best = y;
					minDistance = distance;
				}
				if (distance < smallest){
					smallest = distance;
				}
			}
			if (rayRadius > 0.5){
				if (best == -1){
					rayRadius = rayRadius * .9f;
				}
				else{
					rayRadius = rayRadius * .95f;
				}
			}
			tries += 1;
		}
		if (best != -1){
			(*this->Vertices)[x].Position = (*vertices)[best].Position;
			(*this->Vertices)[x + this->VertexCount].Position = (*vertices)[best].Position;

			printf("[*][%3d/%3d] Using distance of %f, from %d\n", (x + 1), this->VertexCount, smallest, best);
		}
		else{
			printf("[*][%3d/%3d] Gave up, distance to closest was %f\n", (x + 1), this->VertexCount, smallest);
		}
		smallest = minDistance = 10000000000.f;
		best = -1;
	}
	printf("[*] Done!\n");
}