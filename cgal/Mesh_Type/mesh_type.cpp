#include "mesh_type.h"

//MeshType::MeshType() {
//
//}


void MeshType::read_input(const char* filename) {
	std::cout << "I want to die";
}

void MeshType::write_output() {
	std::cout << "Same";
}

void MeshType::checkType(const char* filename){
	//type = "point cloud";
	type = "polygon";
}

std::string MeshType::get_type(){
	return type;
}

//Surface_mesh MeshType::get_mesh() {
//	return mesh;
//}