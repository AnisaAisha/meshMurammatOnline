#include <iostream>
#include <fstream>
#include <string>
#include "nonmanifold_mesh.h"
#include "pointcloud.h"
#include "polygonal_mesh.h"

int main(int argc, char* argv[])
{
	const char* filename = argv[1];

	std::string fn = filename;
	std::string filetype = fn.substr(fn.length() - 3);
	std::vector<Point> points;
	std::vector<std::vector<std::size_t> > polygons;

	Surface_mesh mesh;
	//MeshType* input_mesh;
	bool is_pointcloud = false;
	bool is_triangular = true;
	bool is_manifold = false;

	std::ifstream input_file(filename);
	if (!input_file) {
		std::cout << "Cannot open file" << std::endl;
	}
	
	if (filetype == "ply"){ //can be any one of the four types 
		std::string line;
		while (std::getline(input_file, line)) {
			if (line != "end_header") {
				if (line.substr(0, 12) == "element face") {
					std::cout << "Not a point cloud" << std::endl;
					break;
				}
			}
			else {
				std::cout << "Given file is a point cloud" << std::endl;
				is_pointcloud = true;
				break;
			}
		}
	}
	input_file.close(); //close the file object.

	std::ifstream mesh_input(filename);
	if (!mesh_input)
	{
		std::cerr << "Cannot open file " << std::endl;
	}
	//definitely not a point cloud, can be of any one of the three types
	if (!is_pointcloud && filetype == "ply") {
		CGAL::IO::read_PLY(mesh_input, points, polygons);
	}
	else if (filetype == "obj") {
		CGAL::IO::read_OBJ(mesh_input, points, polygons);
	}
	else if (filetype == "off") {
		CGAL::IO::read_OFF(mesh_input, points, polygons);
	}
	else if (!is_pointcloud){
		std::cout << "Provided file format is not supported by this application" << std::endl;
		return EXIT_FAILURE;
	}
	mesh_input.close();

	if (is_pointcloud) {
		PointCloud p;
		p.read_input(filename);
		p.removeOutliers();
		p.simplifyPointset();
		p.reconstructSurface();
		p.convertToSoup();
		mesh = p.getMesh();
		//p.write_output();
	}
	else if (!is_pointcloud) {
		Nonmanifold_Mesh m(points, polygons);
		//input_mesh = &m;
		//Check for nonmanifoldness
		//input_mesh->read_input(filename);
		//m.read_input(filename);
		is_manifold = m.check_nonmanifold();
		mesh = m.get_mesh();
		std::cout << "After nonmanifold: " << mesh.number_of_faces() << std::endl;

		if (!is_triangle_mesh(mesh)) {
			is_triangular = false;
			std::cout << "Given mesh is not triangular" << std::endl;
			PolygonMesh polyMesh(mesh);
			std::cout << "After polygonal: " << mesh.number_of_faces() << std::endl;
			polyMesh.triangulateSurface();
			polyMesh.addingFacesVertices();	
			mesh = polyMesh.get_mesh();
		}
		else {
			std::cout << "Given mesh is triangular" << std::endl;
		}
	}
	
	std::cout << "After all preprocessing: " << mesh.number_of_faces() << std::endl;
	std::cout << "manifold: " << is_manifold << " triangular: " << is_triangular << " pointcloud: " << is_pointcloud << std::endl;

	std::string output_fn = fn.erase(fn.length() - 3) + "ply";
	std::cout << "Output filename will be: " << output_fn << std::endl;
	std::filebuf fb;
	fb.open(output_fn, std::ios::out);
	std::ostream os(&fb);
	if (is_manifold && !is_pointcloud) {
		std::cout << "write triangular mesh" << std::endl;
		CGAL::IO::write_PLY(os, points, polygons);
	}
	else {
		CGAL::IO::write_PLY(os, mesh);
	}
	fb.close();

    return 0;
}