# Created by the script cgal_create_cmake_script
# This is the CMake script for compiling a CGAL application.


cmake_minimum_required(VERSION 3.1...3.15)
project( Mesh_Type )


find_package(CGAL QUIET)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(Eigen3 3.2.0) #(requires 3.2.0 or greater)
include(CGAL_Eigen_support)

if ( CGAL_FOUND )
	if (TARGET CGAL::Eigen_support)
		add_executable(meshtype main.cpp mesh_type.h nonmanifold_mesh.h pointcloud.h polygonal_mesh.h mesh_type.cpp nonmanifold_mesh.cpp pointcloud.cpp polygonal_mesh.cpp)
		target_link_libraries(meshtype CGAL::CGAL CGAL::CGAL_Core CGAL::Eigen_support)
	endif()
else()

    message(STATUS "This program requires the CGAL library, and will not be compiled.")

endif()

