#include <iostream>
#include <fstream>
#include <CGAL/boost/graph/IO/PLY.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include "mesh_holes.h"
#include <iterator>

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

	Mesh_holes a;
	a.set_mesh(mesh);

	//a.holefill_benchmark();
	Mesh output_mesh = a.generate_hole_highlight();
	std::vector<Hole> hole_list = a.get_hole_list();
	a.hole_fill(hole_list[1]);
	output_mesh = a.get_mesh();
	//faces not getting merged with mesh...
	//std::cout << output_mesh.number_of_faces() << std::endl;
	//std::cout << output_mesh.number_of_vertices() << std::endl;
	a.~Mesh_holes();

	//Open output stream and write the output file
	std::filebuf fb;
	fb.open("holefill.ply", std::ios::out);
	std::ostream os(&fb);
	CGAL::IO::write_PLY(os, output_mesh);
	fb.close();

    return 0;
}