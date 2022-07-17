#include "polygonal_mesh.h"

PolygonMesh::PolygonMesh(){

}

PolygonMesh::PolygonMesh(Surface_mesh m) {
    mesh = m;
}


void PolygonMesh::read_input(const char* filename){
    if(!PMP::read_polygon_mesh(filename, mesh))
    {
        std::cerr << "Invalid input." << std::endl;
    }
    //std::cout << mesh.number_of_faces() << st;
}

void PolygonMesh::triangulateSurface(){
    copiedMesh = mesh;
    std::cout << copiedMesh.number_of_vertices() << std::endl;
    PMP::triangulate_faces(mesh);
}

void PolygonMesh::addingFacesVertices() {
    vertex_descriptor u = mesh.add_vertex(Point(0, 1, 0));
    vertex_descriptor v = mesh.add_vertex(Point(0, 0, 0));
    vertex_descriptor w = mesh.add_vertex(Point(1, 1, 0));
    face_descriptor f = mesh.add_face(u, v, w);
    newVertices.push_back(u);
    newVertices.push_back(v);
    newVertices.push_back(w);
    newFaces.push_back(f);
}
void PolygonMesh::write_output_poly(){

//This is just for testing we will iterate these lists with the new vertices/facesadded using holefilling
    for (auto i = newVertices.begin(); i != newVertices.end(); ++i) {
        copiedMesh.add_vertex(mesh.point(*i));
    }
    std::vector<vertex_descriptor> indFace;
    for (auto i = newFaces.begin(); i != newFaces.end(); ++i) {
        face_descriptor f;
        BOOST_FOREACH(vertex_descriptor vd, vertices_around_face(mesh.halfedge(*i), mesh)) {
            indFace.push_back(vd);
        }
        copiedMesh.add_face(indFace[0], indFace[1], indFace[2]);
        indFace.clear();
    }
    //    copiedMesh.add_face(*i);

    //for (auto i = newVertices.cbegin(); i != newVertices.cend(); ++i)
    //    copiedMesh.add_vertex(*i);
    //
    std::ofstream output("polygonOutputClass.ply");
    CGAL::write_PLY(output, copiedMesh);

}


void PolygonMesh::write_output_triangle(){
    std::ofstream output("triangularOutput.ply");
    CGAL::write_PLY(output, mesh);
    output.close();
}

Surface_mesh PolygonMesh::get_mesh() {
    return mesh;
}