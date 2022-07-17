#include "nonmanifold_mesh.h"

Nonmanifold_Mesh::Nonmanifold_Mesh() {
}

Nonmanifold_Mesh::Nonmanifold_Mesh(std::vector<Point> point_list, std::vector<std::vector<std::size_t>> polygon_list) {
	points = point_list;
	polygons = polygon_list;
}

void Nonmanifold_Mesh::set_mesh(Surface_mesh m, std::vector<Point> point_list, std::vector<std::vector<std::size_t>> polygon_list) {
	mesh = m;
	points = point_list;
	polygons = polygon_list;
}

void Nonmanifold_Mesh::read_input(const char* filename){
	std::cout << "reading input" << std::endl;
	std::cout << filename << std::endl;
	std::ifstream ply_input(filename);
	if (!ply_input)
	{
		std::cerr << "Cannot open file " << std::endl;
		//return false;
	}
	read_PLY(ply_input, points, polygons);
	ply_input.close();
	//CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);
	//return true;
}

void Nonmanifold_Mesh::merge_vertices(vertex_descriptor v_keep, vertex_descriptor v_rm, Surface_mesh& mesh)
{
	std::cout << "merging vertices " << v_keep << " and " << v_rm << std::endl;
	for (halfedge_descriptor h : CGAL::halfedges_around_target(v_rm, mesh)) {
		set_target(h, v_keep, mesh); // to ensure that no halfedge points at the deleted vertex
	}
	remove_vertex(v_rm, mesh);
}

void Nonmanifold_Mesh::create_nonmanifold() {
	//introduce non manifold vertex
	vertex_descriptor v0 = *(vertices(mesh).begin());
	vertex_descriptor v1 = *(--(vertices(mesh).end()));
	merge_vertices(v0, v1, mesh);
}

bool Nonmanifold_Mesh::check_nonmanifold() {
	//checks for any nonmanifold vertices
	/*int counter = 0;
	for (vertex_descriptor v : vertices(mesh))
	{
		if (PMP::is_non_manifold_vertex(v, mesh))
		{
			std::cout << "vertex " << v << " is non-manifold" << std::endl;
			++counter;
		}
	}*/

	//checks that each edge has at most two incident faces, and that no polygon has twice the same vertex
	//and the polygon soup describes a manifold surface

	bool valid = PMP::is_polygon_soup_a_polygon_mesh(polygons);
	if (valid) {
		std::cout << "Given mesh is a valid manifold mesh" << std::endl;
		//construct the mesh
		CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);
	}
	else {
		std::cout << "Given mesh is a non-manifold mesh" << std::endl;
		//send mesh for fixing
		fix_nonmanifold1();
	}
	return valid;
}

void Nonmanifold_Mesh::fix_nonmanifold1() {
	std::cout << "fix manifold func" << std::endl;
	CGAL::Polygon_mesh_processing::orient_polygon_soup(points, polygons);
	//Polyhedron mesh;
	std::cout << "oriented faces" << std::endl;
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);
}

void Nonmanifold_Mesh::fix_nonmanifold2() {
	std::vector<std::vector<vertex_descriptor> > duplicated_vertices;
	std::size_t new_vertices_nb = PMP::duplicate_non_manifold_vertices(mesh,
		NP::output_iterator(
			std::back_inserter(duplicated_vertices)));

	std::cout << new_vertices_nb << " vertices have been added to fix mesh manifoldness" << std::endl;
}

Surface_mesh Nonmanifold_Mesh::get_mesh() {
	CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, mesh);
	return mesh;
}