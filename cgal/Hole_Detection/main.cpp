#include <iostream>
#include <fstream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/boost/graph/IO/PLY.h>
//#include "mesh_type.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;
typedef CGAL::Surface_mesh<Point_3> Mesh;
typedef Mesh::Edge_index edge_descriptor;
typedef Mesh::Vertex_index vertex_descriptor;
typedef Mesh::Halfedge_index halfedge_descriptor;
typedef Mesh::Face_index face_descriptor;

int main(int argc, char* argv[])
{
	const char* filename = argv[1];
	
	//Get input mesh and load it
	std::ifstream ply_input(filename);
	if (!ply_input)
	{
		std::cerr << "Cannot open file " << std::endl;
		return EXIT_FAILURE;
	}
	Mesh mesh;
	std::vector<Point_3> points;
	std::vector<std::vector<std::size_t> > polygons;
	CGAL::IO::read_PLY(ply_input, points, polygons);
	ply_input.close();

	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);
	
	Mesh::Property_map<vertex_descriptor, unsigned char> red;
	bool red_created;
	boost::tie(red, red_created) = mesh.add_property_map<vertex_descriptor, unsigned char>("v:red", '255');
	assert(red_created);
	Mesh::Property_map<vertex_descriptor, unsigned char> green;
	bool green_created;
	boost::tie(green, green_created) = mesh.add_property_map<vertex_descriptor, unsigned char>("v:green", '0');
	assert(green_created);
	Mesh::Property_map<vertex_descriptor, unsigned char> blue;
	bool blue_created;
	boost::tie(blue, blue_created) = mesh.add_property_map<vertex_descriptor, unsigned char>("v:blue", '0');
	assert(blue_created);
	std::vector<halfedge_descriptor> border_cycles;

	// collect one halfedge per boundary cycle
	/*CGAL::Polygon_mesh_processing::extract_boundary_cycles(mesh, std::back_inserter(border_cycles));
	for (halfedge_descriptor h : border_cycles)
	{
		for (halfedge_descriptor hd : halfedges_around_face(h, mesh))
		{
			vertex_descriptor vd = target(hd, mesh);
			//halfedge_descriptor hi = next(hd, highlighted_mesh);
			//face_descriptor fd = face(opposite(hi, highlighted_mesh), highlighted_mesh);
			red[vd] = 255; //highlight with red
			green[vd] = 0;
			blue[vd] = 0;
		}
	}*/
	for (vertex_descriptor vd : mesh.vertices()) {
		if (mesh.is_border(vd)) {
			red[vd] = 255; //highlight with red
			green[vd] = 0;
			blue[vd] = 0;
		}
		else {
			red[vd] = 211; //gray color by default
			green[vd] = 211;
			blue[vd] = 211;
		}
	}
	//Open output stream and write the output file
	std::filebuf fb;
	fb.open("detected_hole.ply", std::ios::out);
	std::ostream os(&fb);
	CGAL::IO::write_PLY(os, mesh);
	fb.close();

    return 0;
}