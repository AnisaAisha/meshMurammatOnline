#ifndef _MESH_TYPE_H
#define _MESH_TYPE_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/boost/graph/IO/PLY.h>
#include <CGAL/remove_outliers.h>
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_set_3/IO.h>
#include <fstream>
#include <iostream>
#include <vector> 
#include <CGAL/compute_average_spacing.h>

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector_3;
typedef CGAL::Surface_mesh<Point> Surface_mesh;
typedef boost::graph_traits<Surface_mesh>::face_descriptor face_descriptor;
typedef boost::graph_traits<Surface_mesh>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Surface_mesh>::halfedge_descriptor halfedge_descriptor;
typedef CGAL::Point_set_3<Point, Vector_3> Point_set;
namespace PMP = CGAL::Polygon_mesh_processing;
namespace NP = CGAL::parameters;

class MeshType{
	public:
		//MeshType();
		void checkType(const char* filename);
		std::string get_type();
		virtual void read_input(const char* filename);
		virtual void write_output(); //Pure virtual function we dont want to define it here
		//Surface_mesh get_mesh();
		Surface_mesh mesh;
		std::string filename;
		Surface_mesh output_mesh;
		
	private:
		std::string type; 
		
    	//Surface_mesh output_mesh;
};

#endif