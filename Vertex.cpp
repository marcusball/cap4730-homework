#include "Vertex.h"


Vertex::Vertex() : Position(0,0,0,1), Normal(0,0,0,1){
}

Vertex::Vertex(Vector4f pos, Vector4f norm) : Position(pos), Normal(norm){

}


Vertex::~Vertex()
{
}
