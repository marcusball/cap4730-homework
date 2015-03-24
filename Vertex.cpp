#include "Vertex.h"


Vertex::Vertex() : Position(0,0,0,1), Normal(0,0,0), Color(ColorVectors::WHITE){
}

Vertex::Vertex(Vector4f pos, Vector3f norm) : Position(pos), Normal(norm), Color(ColorVectors::WHITE){

}

Vertex::Vertex(Vector4f pos, Vector3f norm, Vector4f color) : Position(pos), Normal(norm), Color(color){

}

Vertex::~Vertex()
{
}
