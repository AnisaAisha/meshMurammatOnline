#ifndef _NONMANIFOLD_MESH_H
#define _NONMANIFOLD_MESH_H

#include "mesh_type.h"
//#include <CGAL/IO/OFF_reader.h>
//#include <CGAL/Polyhedron_3.h>

//namespace PMP = CGAL::Polygon_mesh_processing;

//typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
//typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
//typedef Kernel::Point_3 Point_3;
//typedef Kernel::Vector_3 Vector_3;
//typedef CGAL::Surface_mesh<Point_3> Mesh;
//typedef Mesh::Vertex_index vertex_descriptor;
//typedef Mesh::Halfedge_index halfedge_descriptor;
//typedef Mesh::Face_index face_descriptor;

class Nonmanifold_Mesh : public MeshType {
public:
	Nonmanifold_Mesh();
	Nonmanifold_Mesh(std::vector<Point>, std::vector<std::vector<std::size_t>>);
	void set_mesh(Surface_mesh, std::vector<Point>, std::vector<std::vector<std::size_t>>);
	void read_input(const char* filename);
	void merge_vertices(vertex_descriptor, vertex_descriptor, Surface_mesh&);
	void create_nonmanifold();
	bool check_nonmanifold();
	void fix_nonmanifold1();
	void fix_nonmanifold2();
	Surface_mesh get_mesh();

private:
	Surface_mesh mesh;
	std::vector<Point> points;
	std::vector<std::vector<std::size_t>> polygons;
};

#endif