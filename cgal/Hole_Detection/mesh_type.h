#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <algorithm>
#include <CGAL/Triangle_3.h>
#include <CGAL/intersections.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;
typedef Kernel::Triangle_3 Triangle_3;
typedef CGAL::Surface_mesh<Point_3> Mesh;
typedef Mesh::Edge_index edge_descriptor;
typedef Mesh::Vertex_index vertex_descriptor;
typedef Mesh::Halfedge_index halfedge_descriptor;
typedef Mesh::Face_index face_descriptor;


struct Angle {
	std::vector<Point_3> vertices; //vertices that make up the angle
	std::vector<vertex_descriptor> vertex_indices;
	Angle* next;
	Angle* previous;
	double degree;
	bool update = false;

	Angle() {
		degree = 0;
		next = NULL;
		previous = NULL;
		vertices.push_back(Point_3(0, 0, 0));
		vertices.push_back(Point_3(0, 0, 0));
		vertices.push_back(Point_3(0, 0, 0));
	}

	Angle(Point_3 a, Point_3 b, Point_3 c, vertex_descriptor v1, vertex_descriptor v2, vertex_descriptor v3) {
		degree = 0;
		next = NULL;
		previous = NULL;
		vertices.push_back(a);
		vertices.push_back(b);
		vertices.push_back(c);
		vertex_indices.push_back(v1);
		vertex_indices.push_back(v2);
		vertex_indices.push_back(v3);
	}

	void set_vertices(Point_3 a, Point_3 b, Point_3 c) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}

	double compute_angle(Point_3 a, Point_3 b, Point_3 c) {
		Vector_3 v1 = a - b;
		v1 = v1 / std::sqrt(v1 * v1);
		Vector_3 v2 = c - b;
		v2 = v2 / std::sqrt(v2 * v2);

		double degree = (std::acos(((v1 * v2) / (std::sqrt(v1.squared_length() * v2.squared_length()))))) /CGAL_PI * 180;
		//std::cout << "initial degree " << degree << std::endl;

		Vector_3 b_vec = Vector_3(b.x(), b.y(), b.z());
		Vector_3 c2 = CGAL::cross_product(Vector_3(a.x(), a.y(), a.z()), Vector_3(c.x(), c.y(), c.z())) + b_vec;
		c2 = c2/std::sqrt(c2.squared_length());
		//std::cout << std::sqrt(c2.squared_length()) << "   " <<  std::sqrt(b_vec.squared_length()) << std::endl;

		if (std::sqrt(c2.squared_length()) < std::sqrt(b_vec.squared_length())) {
			degree = 360 - degree;
		}
		//std::cout << "final degree " << degree << std::endl;
		return degree;
	}

	bool operator< (const Angle& other) const {
		return degree < other.degree;
	}


	bool operator== (const Angle& a1) const{
		//return vertex_indices[0] == a1.vertex_indices[0] && vertex_indices[1] == a1.vertex_indices[1] && vertex_indices[2] == a1.vertex_indices[2];
		std::cout << a1.degree << degree << std::endl;
		return a1.degree == degree;
	}
};

/*bool operator==(const Angle& l, const Angle& r) {
	return l.degree == r.degree;
}

bool operator<(const Angle& l, const Angle& r) {
	return l.degree < r.degree;
}*/

struct Face {
	std::vector<Point_3> vertices;
	std::vector<vertex_descriptor> vertex_indices;

	Face(vertex_descriptor a, vertex_descriptor b, vertex_descriptor c) {
		vertex_indices.push_back(a);
		vertex_indices.push_back(b);
		vertex_indices.push_back(c);
	}

	void set_vertices(Point_3 a, Point_3 b, Point_3 c) {
		vertices.push_back(a);
		vertices.push_back(b);
		vertices.push_back(c);
	}
};


struct Hole {
	std::vector<vertex_descriptor> vertices; //front
	std::vector<vertex_descriptor> patch_vertices; //filling
	std::vector<face_descriptor> faces;
	std::vector<face_descriptor> patch_faces;
	std::vector<Face> patch_faces_custom;
	std::vector<Angle> angles;
};



class Mesh_holes {
public:
	void set_mesh(Mesh input_mesh);
	Mesh generate_hole_highlight();
	void calculate_angle(std::vector<vertex_descriptor>);
	vertex_descriptor apply_rule(std::vector<vertex_descriptor>, Angle*, Hole);
	bool hole_fill(Hole hole);
	void create_triangles_1(Angle* angle); //create triangles acc to rule 1
	vertex_descriptor create_triangles_2(std::vector<vertex_descriptor>, Angle*); //create triangles acc to rule 2
	vertex_descriptor create_triangles_3(std::vector<vertex_descriptor>, Angle*, Hole); //create triangles acc to rule 3
	Angle* getAngle();
	Vector_3 create_new_vertex2(Point_3, Point_3, Point_3);
	Vector_3 create_new_vertex3(Point_3, Point_3, Point_3);
	bool face_collision_test(Hole, Point_3, Point_3, Point_3);
	bool merge_vertices(vertex_descriptor);
	Mesh get_mesh();
	std::vector<Hole> get_hole_list();
	void update_faces(Hole, vertex_descriptor, vertex_descriptor);
	bool angle_merge_vertex(Hole, Point_3, Point_3);
	~Mesh_holes();
	void holefill_benchmark();
	Mesh get_highlighted_mesh();
	
private:
	Mesh mesh;
	Mesh highlighted_mesh;
	//Hole hole_info;

	//list of holes (where each hole is a list of boundary vertices)
	std::vector < std::vector<vertex_descriptor> > holes;
	std::vector < std::vector<vertex_descriptor> > hole_vertices; //front
	std::vector < std::vector<face_descriptor> > hole_faces;
	std::vector<Hole> hole_list;

	std::vector <Angle*> angles;
	std::vector <vertex_descriptor> patch_vertices; //filling
	std::vector <face_descriptor> patch_faces;
	Hole current_hole;
	bool intersection = true;
	int merge_threshold = 0.001;
	int num_holes;
};

//sort(angles.begin(), angles.end(), [](const Angle& a, const Angle& b)) {
//	return a.degree < b.degree;
//}