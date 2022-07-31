#ifndef _POLYGONAL_MESH_H
#define _POLYGONAL_MESH_H

#include "mesh_type.h"
class PolygonMesh:public MeshType {
public:
	PolygonMesh();
	PolygonMesh(Surface_mesh);
	void read_input(const char* filename);
	void triangulateSurface();
	void addingFacesVertices();
	void write_output_triangle();
	void write_output_poly();
	Surface_mesh get_mesh();

private:
	std::vector<face_descriptor> newFaces;
	std::vector<vertex_descriptor> newVertices;
	Surface_mesh copiedMesh;
};

#endif