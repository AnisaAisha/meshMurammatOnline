#include "mesh_holes.h"

void Mesh_holes::set_mesh(Mesh input_mesh) {
	mesh = input_mesh;
	highlighted_mesh = input_mesh;
}

Mesh Mesh_holes::generate_hole_highlight() {
	
	Mesh::Property_map<vertex_descriptor, unsigned char> red;
	bool red_created;
	boost::tie(red, red_created) = highlighted_mesh.add_property_map<vertex_descriptor, unsigned char>("v:red", '255');
	assert(red_created);
	Mesh::Property_map<vertex_descriptor, unsigned char> green;
	bool green_created;
	boost::tie(green, green_created) = highlighted_mesh.add_property_map<vertex_descriptor, unsigned char>("v:green", '0');
	assert(green_created);
	Mesh::Property_map<vertex_descriptor, unsigned char> blue;
	bool blue_created;
	boost::tie(blue, blue_created) = highlighted_mesh.add_property_map<vertex_descriptor, unsigned char>("v:blue", '0');
	assert(blue_created);

	/*for (vertex_descriptor vd : mesh.vertices()) {
		if (mesh.is_border(vd)) {
			red[vd] = 255; //highlight with red
			green[vd] = 0;
			blue[vd] = 0;
			border_points.push_back(vd);
		}
		else {
			red[vd] = 211; //gray color by default
			green[vd] = 211;
			blue[vd] = 211;
		}
	}*/

	std::vector<halfedge_descriptor> border_cycles;

	// collect one halfedge per boundary cycle
	CGAL::Polygon_mesh_processing::extract_boundary_cycles(highlighted_mesh, std::back_inserter(border_cycles));
	int num = 0;
	for (halfedge_descriptor h : border_cycles)
	{
		//std::vector <vertex_descriptor> temp;
		//std::vector <face_descriptor> temp2;
		num++;
		Hole hole;
		for (halfedge_descriptor hd : halfedges_around_face(h, highlighted_mesh))
		{
			vertex_descriptor vd = target(hd, highlighted_mesh);
			halfedge_descriptor hi = next(hd, highlighted_mesh);
			face_descriptor fd = face(opposite(hi, highlighted_mesh), highlighted_mesh);
			red[vd] = 255; //highlight with red
			green[vd] = 0;
			blue[vd] = 0;
			//temp.push_back(vd);
			//temp2.push_back(fd);
			hole.vertices.push_back(vd);
			hole.faces.push_back(fd);
			
		}
		//hole_vertices.push_back(temp);
		//hole_faces.push_back(temp2);
		hole_list.push_back(hole);
		
	}
	num_holes = num;
	return highlighted_mesh;
}

void Mesh_holes::calculate_angle(std::vector<vertex_descriptor> border_points) {
	std::cout << "angles function " << std::endl;
	double smallest_angle = 361.0; //keep a large value at start for comparisons
	int smallest_angle_idx = border_points[0];

	for (int i = 0; i < border_points.size(); i++) {
		vertex_descriptor prev_vd;
		vertex_descriptor current_vd = border_points[i];
		vertex_descriptor next_vd = border_points[(i + 1) % static_cast<int>(border_points.size())];

		Point_3 prev_vertex;
		if (i == 0) {
			prev_vd = border_points[border_points.size() - 1];
			prev_vertex = mesh.point(prev_vd);
		}
		else {
			prev_vd = border_points[i - 1];
			prev_vertex = mesh.point(prev_vd);
		}

		Point_3 current_vertex = mesh.point(current_vd);
		Point_3 next_vertex = mesh.point(next_vd);
		//std::cout << "prev vd: " << prev_vertex << " current vd: " << current_vertex << " next vd: " << next_vertex << std::endl;
		
		Angle* angle = new Angle(prev_vertex, current_vertex, next_vertex, prev_vd, current_vd, next_vd);
		angle->degree = angle->compute_angle(prev_vertex, current_vertex, next_vertex);
		//std::cout << angle->degree << std::endl;
		angles.push_back(angle);
		//std::cout << "current degree: " << angle->degree << std::endl;

		if (i == 0) {
			angles[0]->previous = NULL;
		}
		else {
			angles[i]->previous = angles[i - 1];
		}

		if (i > 1) {
			angles[i - 2]->next = angles[i - 1];
		}

		if (smallest_angle > angle->degree) {
			smallest_angle = angle->degree;
			smallest_angle_idx = i;
		}
	}
	angles[0]->previous = angles[angles.size() - 1];
	angles[0]->next = angles[1];
	angles[1]->next = angles[2];
	angles[angles.size() - 2]->next = angles[angles.size() - 1];
	angles[angles.size() - 1]->next = angles[0];
	
	std::sort(angles.begin(), angles.end(), [](Angle* l, Angle* r) { return l->degree < r->degree; });
	//std::sort(angles.begin(), angles.end());
	//std::cout << "next angle degree   " << angles.front().next->next->degree << std::endl;

	/*std::cout << "printing all angles" << std::endl;
	for (int i = 0; i < angles.size(); i++) {
		//Angle angle = angles[i];
		std::cout << i << std::endl;
		std::cout << "previous " << angles[i]->previous->degree << std::endl;
		std::cout << angles[i]->previous->vertex_indices[0] << "  " << angles[i]->previous->vertex_indices[1] << "  " << angles[i]->previous->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->previous->vertices[0] << angles[i]->previous->vertices[1] << angles[i]->previous->vertices[2] << std::endl;
		std::cout << "current " <<  angles[i]->degree << std::endl;
		std::cout << angles[i]->vertex_indices[0] << "  " << angles[i]->vertex_indices[1] << "  " << angles[i]->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->vertices[0] << angles[i]->vertices[1] << angles[i]->vertices[2] << std::endl;
		std::cout << "next " << angles[i]->next->degree << std::endl;
		std::cout << angles[i]->next->vertex_indices[0] << "  " << angles[i]->next->vertex_indices[1] << "  " << angles[i]->next->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->next->vertices[0] << angles[i]->next->vertices[1] << angles[i]->next->vertices[2] << std::endl;
	}*/
}

bool Mesh_holes::hole_fill(Hole hole) {
	bool is_newV = false;
	current_hole = hole;
	//Find all angles for a hole
	std::cout << "hole fill function" << std::endl;
	std::cout << "hole boundary vertices / front size : " << hole.vertices.size() << std::endl;

	calculate_angle(hole.vertices);
	std::cout << "main loop" << std::endl;
	//main loop
	while(true){
		vertex_descriptor new_vd;
		std::cout << "angle size: " << angles.size() << std::endl;
		std::cout << "front vertices size " << current_hole.vertices.size() << std::endl;

		if (current_hole.vertices.size() == 4 || angles.size() == 0) {
			break;
		}
		if (angles.size() > 0) {
			Angle* angle = getAngle();
			std::cout << "current angle degree: " << angle->degree << std::endl;
			//break;
			new_vd = apply_rule(hole.vertices, angle, current_hole);
			std::cout << "neww vertex descriptor: " << new_vd << std::endl;
			std::sort(angles.begin(), angles.end(), [](Angle* l, Angle* r) { return l->degree < r->degree; });
		}

		std::cout << "***rule applied***" << std::endl;
		
		if (new_vd > 10000000 || current_hole.vertices.size() != 3) {
			merge_vertices(new_vd); //front, filling, vnew, hole
		}
		
		/*std::cout << "patch hole size " << current_hole.patch_vertices.size() << std::endl;
		std::cout << "patch faces size " << current_hole.patch_faces.size() << std::endl;
		std::cout << "patch faces custom size " << current_hole.patch_faces_custom.size() << std::endl;
		//std::cout << "mesh vertices " << mesh.number_of_vertices() << std::endl;
		//std::cout << "mesh faces " << mesh.number_of_faces() << std::endl;*/
	}
	
	return true;
}

vertex_descriptor Mesh_holes::apply_rule(std::vector<vertex_descriptor> vertices, Angle* angle, Hole hole) {
	vertex_descriptor vd;
	if (angle->degree <= 75) {
		create_triangles_1(angle);
	}
	else if (angle->degree > 75 && angle->degree <= 135) {
		return create_triangles_2(vertices, angle);
	}
	else {
		return create_triangles_3(vertices, angle, hole);
	}
	return vd;
}

vertex_descriptor Mesh_holes::create_triangles_3(std::vector<vertex_descriptor> vertices, Angle* angle, Hole hole) {
	vertex_descriptor vd;
	Point_3 previous_vertex = angle->vertices[0];
	Point_3 current_vertex = angle->vertices[1];
	Point_3 next_vertex = angle->vertices[2];

	std::cout << "*****create triangle func 3*****" << std::endl;

	//rule3calc
	Vector_3 new_vertex = create_new_vertex3(previous_vertex, current_vertex, next_vertex);
	Point_3 new_v(new_vertex.x(), new_vertex.y(), new_vertex.z());

	bool is_new = true;
	if (face_collision_test(current_hole, new_v, previous_vertex, next_vertex)) {
		//angle->update = true;
		//angles.push_back(angle);
		is_new = false;
		return vd;
	}

	if (is_new){
		vd = mesh.add_vertex(new_v);
		current_hole.patch_vertices.push_back(vd);

		//front indices
		vertex_descriptor prev_vd = angle->vertex_indices[0];
		vertex_descriptor current_vd = angle->vertex_indices[1];
		vertex_descriptor next_vd = angle->vertex_indices[2];

		//filling indices
		//auto filling_next_vertex = std::distance(patch_vertices.begin(), std::find(patch_vertices.begin(), patch_vertices.end(), next_vd));
		//auto filling_current_vertex = std::distance(patch_vertices.begin(), std::find(patch_vertices.begin(), patch_vertices.end(), current_vd));

		vertex_descriptor last_vd = current_hole.patch_vertices[current_hole.patch_vertices.size() - 1];

		face_descriptor new_fd = mesh.add_face(next_vd, current_vd, last_vd);
		current_hole.patch_faces.push_back(new_fd);
		std::cout << "face descriptors: " << new_fd << std::endl;
		std::cout << "number of faces after update: " << mesh.number_of_faces() << std::endl;

		//auto front_current_vertex = std::distance(current_hole.vertices.begin(), std::find(current_hole.vertices.begin(), current_hole.vertices.end(), current_vd));
		//current_hole.vertices.erase(current_hole.vertices.begin() + front_current_vertex);

		std::cout << "works till here" << std::endl;
		unsigned int i = current_vd + 1;
		//Point_3 remove_vertex = mesh.point(current_vd);
		vertex_descriptor remove_vd(i);
		auto front_current_vertex = std::distance(current_hole.vertices.begin(), std::find(begin(current_hole.vertices), end(current_hole.vertices), remove_vd));
		current_hole.vertices.erase(current_hole.vertices.begin() + front_current_vertex);

		std::cout << "works till here" << std::endl;
		auto it = current_hole.vertices.insert(current_hole.vertices.begin(), vd);
		std::cout << "works till here" << std::endl;



		Angle* next_angle = angle->next;
		//Angle* prev_angle = angle->previous;
		Angle* new_angle = new Angle(current_vertex, new_v, next_vertex, current_vd, vd, next_vd);
		//new_angle->set_vertices(current_vertex, new_v, next_vertex);
		std::cout << "works till here " << std::endl;

		//auto it2 = std::distance(angles.begin(), std::find(begin(angles), end(angles), angle->next));
		//angles.erase(angles.begin() + it2);

		new_angle->previous = angle;
		//new_angle->next = next_angle;

		angle->set_vertices(previous_vertex, current_vertex, new_v);
		angle->next = new_angle;

		next_angle->set_vertices(new_v, angle->next->vertices[1], angle->next->vertices[2]);
		next_angle->previous = new_angle;
		
		new_angle->next = next_angle;
		new_angle->degree = new_angle->compute_angle(current_vertex, new_v, next_vertex);

		angles.push_back(new_angle);
		std::cout << "works till here" << std::endl;
		//std::cout << angle->previous->degree << std::endl;
		//std::cout << next_angle->previous->degree << std::endl;
		//std::cout << new_angle->previous->degree << std::endl;
		//angles.push_back(next_angle);
	}
	else {
		angle->update = true;
		
	}
	angles.push_back(angle);
	/*std::cout << "printing all angles" << std::endl;
	for (int i = 0; i < angles.size(); i++) {
		//Angle angle = angles[i];
		std::cout << i << std::endl;
		std::cout << "previous " << angles[i]->previous->degree << std::endl;
		std::cout << angles[i]->previous->vertex_indices[0] << "  " << angles[i]->previous->vertex_indices[1] << "  " << angles[i]->previous->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->previous->vertices[0] << angles[i]->previous->vertices[1] << angles[i]->previous->vertices[2] << std::endl;
		std::cout << "current " << angles[i]->degree << std::endl;
		std::cout << angles[i]->vertex_indices[0] << "  " << angles[i]->vertex_indices[1] << "  " << angles[i]->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->vertices[0] << angles[i]->vertices[1] << angles[i]->vertices[2] << std::endl;
		std::cout << "next " << angles[i]->next->degree << std::endl;
		std::cout << angles[i]->next->vertex_indices[0] << "  " << angles[i]->next->vertex_indices[1] << "  " << angles[i]->next->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->next->vertices[0] << angles[i]->next->vertices[1] << angles[i]->next->vertices[2] << std::endl;
	}*/
	
	return vd;
}

Vector_3 Mesh_holes::create_new_vertex3(Point_3 previous_vertex, Point_3 current_vertex, Point_3 next_vertex) {
	
	Vector_3 next_vector(next_vertex.x() - current_vertex.x(), next_vertex.y() - current_vertex.y(), next_vertex.z() - current_vertex.z());
	Vector_3 prev_vector(previous_vertex.x() - current_vertex.x(), previous_vertex.y() - current_vertex.y(), previous_vertex.z() - current_vertex.z());
	Vector_3 current_vector(current_vertex.x(), current_vertex.y(), current_vertex.z());

	Vector_3 c = CGAL::cross_product(next_vector, prev_vector);
	Vector_3 c2 = CGAL::cross_product(c, next_vector) + current_vector;
	
	int s1 = 1;
	int s2 = 1;
	Point_3 origin(0, 0, 0);
	Vector_3 origin_vector(0, 0, 0);
	Point_3 p(c2.x() - current_vector.x(), c2.y() - current_vector.y(), c2.z() - current_vector.z());
	Vector_3 normal = CGAL::normal(origin, next_vertex, p);
	Vector_3 v1 = (next_vertex - origin) * s1;
	Vector_3 v2 = (Point_3(c2.x(), c2.y(), c2.z()) - origin) * s2;

	//skipping some steps
	Vector_3 result = origin_vector + v1 + v2;
	//skipping keep near plane
	return result;
}


vertex_descriptor Mesh_holes::create_triangles_2(std::vector<vertex_descriptor> vertices, Angle* angle) {
	vertex_descriptor vd;
	Point_3 previous_vertex = angle->vertices[0];
	Point_3 current_vertex = angle->vertices[1];
	Point_3 next_vertex = angle->vertices[2];
	
	std::cout << "*****create triangle func 2*****" << std::endl;
	std::cout << "prev vertex: " << previous_vertex << " current vertex: " << current_vertex << " next vertex: " << next_vertex << std::endl;

	bool is_new = true;
	//rule2calc
	Vector_3 new_vertex = create_new_vertex2(previous_vertex, current_vertex, next_vertex);
	Point_3 new_v(new_vertex.x(), new_vertex.y(), new_vertex.z());
	if (face_collision_test(current_hole, new_v, previous_vertex, next_vertex)) {
		is_new = false;
		return vd;
	}
	//std::cout << "******new vertex create*******" << is_new << std::endl;
	if (is_new) {
		std::cout << "new vertex " << new_v << std::endl;
		vd = mesh.add_vertex(new_v);
		current_hole.patch_vertices.push_back(vd);
		//front indices
		vertex_descriptor prev_vd = angle->vertex_indices[0];
		vertex_descriptor current_vd = angle->vertex_indices[1];
		vertex_descriptor next_vd = angle->vertex_indices[2];
		std::cout << "patch vertics length: " << current_hole.patch_vertices.size() << std::endl;
		vertex_descriptor last_vd = current_hole.patch_vertices[current_hole.patch_vertices.size() - 1];
		Point_3 last_vertex = mesh.point(last_vd);
		std::cout << "prev vd: " << prev_vd << " current vd: " << current_vd << " next vd: " << next_vd << " last vd: " << last_vd << std::endl;

		/* First Face */
		//face_descriptor new_fd1 = mesh.add_face(current_vd, last_vd, prev_vd); //partially works
		face_descriptor new_fd1 = mesh.add_face(prev_vd, current_vd, last_vd); //partially works
		//face_descriptor new_fd1 = mesh.add_face(last_vd, prev_vd, current_vd); //partially works
		
		/* Second Face */
		face_descriptor new_fd2 = mesh.add_face(current_vd, next_vd, last_vd); //partially works
		//face_descriptor new_fd2 = mesh.add_face(last_vd, current_vd, next_vd); //partially works
		//face_descriptor new_fd2 = mesh.add_face(next_vd, last_vd, current_vd); //partially works

		std::cout << "face descriptors: " << new_fd1 << ",   " << new_fd2 << std::endl;
		current_hole.patch_faces.push_back(new_fd1);
		current_hole.patch_faces.push_back(new_fd2);

		Face f1 = Face(current_vd, prev_vd, last_vd);
		f1.set_vertices(current_vertex, previous_vertex, last_vertex);
		current_hole.patch_faces_custom.push_back(f1);
		Face f2 = Face(current_vd, last_vd, next_vd);
		f2.set_vertices(current_vertex, last_vertex, next_vertex);
		current_hole.patch_faces_custom.push_back(f2);
		std::cout << "number of faces after update: " << mesh.number_of_faces() << std::endl;

		auto idx = std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), current_vd));
		vertices[idx] = vd;
		std::cout << "works till here " << std::endl;
		//update angle
		/*Angle* prev_angle = angle->previous;
		Angle* next_angle = angle->next;

		angle->set_vertices(previous_vertex, new_v, next_vertex);

		auto it = std::distance(angles.begin(), std::find(begin(angles), end(angles), angle->next));
		angles.erase(angles.begin() + it);

		auto it2 = std::distance(angles.begin(), std::find(begin(angles), end(angles), angle->previous));
		angles.erase(angles.begin() + it2);

		prev_angle->set_vertices(prev_angle->vertices[0], prev_angle->vertices[1], new_v);
		next_angle->set_vertices(new_v, next_angle->vertices[1], next_angle->vertices[2]);
		
		angles.push_back(prev_angle);
		angles.push_back(next_angle);*/

		angle->set_vertices(previous_vertex, new_v, next_vertex);
		angle->previous->set_vertices(angle->previous->vertices[0], angle->previous->vertices[1], new_v);
		angle->next->set_vertices(new_v, angle->next->vertices[1], angle->next->vertices[2]);

		/*auto it = std::distance(current_hole.vertices.begin(), std::find(begin(current_hole.vertices), end(current_hole.vertices), prev_vd));
		current_hole.vertices.erase(current_hole.vertices.begin() + it);
		std::cout << "works till here " << std::endl;
		auto it2 = std::distance(current_hole.vertices.begin(), std::find(begin(current_hole.vertices), end(current_hole.vertices), next_vd));
		current_hole.vertices.erase(current_hole.vertices.begin() + it2);*/

		//Point_3 new_vp = Point_3(new_vertex.x(), new_vertex.y(), new_vertex.z());
		//angle->previous->set_vertices(angle->previous->vertices[0], angle->previous->vertices[1], new_vp);
		//angle->next->set_vertices(new_vp, angle->next->vertices[1], angle->next->vertices[2]);
		//std::cout << "works till here " << std::endl;
		//prev_angle->set_vertices(prev_angle->vertices[0], prev_angle->vertices[1], new_vp);
		//next_angle->set_vertices(new_vp, next_angle->vertices[1], next_angle->vertices[2]);
		//angles.push_back(prev_angle);
		//angles.push_back(next_angle);
		//std::cout << "works till here " << std::endl;
		//angle->previous = prev_angle;
		//angle->next = next_angle;
		//angle->next->previous = angle->previous;

	}
	else {
		angle->update = true;
		angles.push_back(angle);
	}
	
	/*std::cout << "printing all angles" << std::endl;
	for (int i = 0; i < angles.size(); i++) {
		//Angle angle = angles[i];
		std::cout << i << std::endl;
		std::cout << "previous " << angles[i]->previous->degree << std::endl;
		std::cout << angles[i]->previous->vertex_indices[0] << "  " << angles[i]->previous->vertex_indices[1] << "  " << angles[i]->previous->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->previous->vertices[0] << angles[i]->previous->vertices[1] << angles[i]->previous->vertices[2] << std::endl;
		std::cout << "current " << angles[i]->degree << std::endl;
		std::cout << angles[i]->vertex_indices[0] << "  " << angles[i]->vertex_indices[1] << "  " << angles[i]->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->vertices[0] << angles[i]->vertices[1] << angles[i]->vertices[2] << std::endl;
		std::cout << "next " << angles[i]->next->degree << std::endl;
		std::cout << angles[i]->next->vertex_indices[0] << "  " << angles[i]->next->vertex_indices[1] << "  " << angles[i]->next->vertex_indices[2] << std::endl;
		//std::cout << angles[i]->next->vertices[0] << angles[i]->next->vertices[1] << angles[i]->next->vertices[2] << std::endl;
	}*/
	return vd;
}

Vector_3 Mesh_holes::create_new_vertex2(Point_3 previous_vertex, Point_3 current_vertex, Point_3 next_vertex) {
	int s1 = 1;
	int s2 = 1;
	Point_3 origin(0, 0, 0);
	Vector_3 origin_vector(0, 0, 0);
	//std::cout << "prev vertex: " << previous_vertex << " current vertex: " << current_vertex << " next vertex: " << next_vertex << std::endl;
	//Vector_3 vp = previous_vertex - current_vertex;
	//Vector_3 vn = next_vertex - current_vertex;
	Vector_3 vp = Vector_3(previous_vertex.x(), previous_vertex.y(), previous_vertex.z());
	Vector_3 vn = Vector_3(next_vertex.x(), next_vertex.y(), next_vertex.z());

	//Vector_3 normal = CGAL::normal(vp, origin, vn);
	Vector_3 v1 = (origin_vector - vp) * s1;
	Vector_3 v2 = (origin_vector - vn) * s2;
	//std::cout << "vector vp: "<< vp << std::endl;
	//std::cout << "vector vn: " << vn << std::endl;
	//skipping some steps
	Vector_3 result = origin_vector + v1 + v2;
	return result;
}

void Mesh_holes::create_triangles_1(Angle* angle) {
	Point_3 previous_vertex = angle->vertices[0];
	Point_3 current_vertex = angle->vertices[1];
	Point_3 next_vertex = angle->vertices[2];
	//std::cout << "prev vertex: " << previous_vertex << " current vertex: " << current_vertex << " next vertex: " << next_vertex << std::endl;

	//front vertices
	vertex_descriptor prev_vd = angle->vertex_indices[0];
	vertex_descriptor current_vd = angle->vertex_indices[1];
	vertex_descriptor next_vd = angle->vertex_indices[2];

	std::cout << "*****create triangles func 1*****" << std::endl;
	//std::cout << "prev vd: " << prev_vd << " current vd: " << current_vd << " next vd: " << next_vd << std::endl;
	vertex_descriptor vd;
	bool is_new = true;
	if (face_collision_test(current_hole, previous_vertex, next_vertex, Point_3())) {
		is_new = false;
	}
	if (is_new){
		//face_descriptor new_fd = mesh.add_face(prev_vd, current_vd, next_vd); //one working version
		//face_descriptor new_fd = mesh.add_face(current_vd, next_vd, prev_vd); //another working version
		face_descriptor new_fd = mesh.add_face(next_vd, prev_vd, current_vd);
		//std::cout << current_vertex << next_vertex << previous_vertex << std::endl;
		current_hole.patch_faces.push_back(new_fd);
		Face f = Face(current_vd, prev_vd, next_vd);
		std::cout << "face descriptors: " << new_fd << std::endl;
		std::cout << "number of faces after update: " << mesh.number_of_faces() << std::endl;
		std::cout << "prev vd: " << prev_vd << " current vd: " << current_vd << " next vd: " << next_vd << std::endl;
		
		f.set_vertices(current_vertex, previous_vertex, next_vertex);
		current_hole.patch_faces_custom.push_back(f);
		std::cout << "works till here" << std::endl;
		auto it = std::distance(current_hole.vertices.begin(), std::find(begin(current_hole.vertices), end(current_hole.vertices), current_vd));
		current_hole.vertices.erase(current_hole.vertices.begin() + it);
		std::cout << "works till here" << std::endl;
		//std::cout << "hole vertices after removal: " << current_hole.vertices.size() << std::endl;
		//std::cout << is_new << std::endl;

		/*Angle* prev_angle = angle->previous;
		Angle* next_angle = angle->next;

		auto it2 = std::distance(angles.begin(), std::find(begin(angles), end(angles), angle->previous));
		angles.erase(angles.begin() + it2);
		auto it3 = std::distance(angles.begin(), std::find(begin(angles), end(angles), angle->next));
		angles.erase(angles.begin() + it3);

		prev_angle->set_vertices(prev_angle->vertices[0], prev_angle->vertices[1], next_vertex);
		prev_angle->next = next_angle;

		next_angle->set_vertices(previous_vertex, next_angle->vertices[1], next_angle->vertices[2]);
		next_angle->previous = prev_angle;

		angles.push_back(prev_angle);
		angles.push_back(next_angle);*/

		angle->previous->set_vertices(angle->previous->vertices[0], angle->previous->vertices[1], next_vertex);
		angle->previous->next = angle->next;
		angle->next->set_vertices(previous_vertex, angle->next->vertices[1], angle->next->vertices[2]);
		angle->next->previous = angle->previous;
		std::cout << "works till here" << std::endl;
	}
	else {
		angle->update = true;
		angles.push_back(angle);
	}
	std::cout << "create triangles 1 end" << std::endl;
}

bool Mesh_holes::face_collision_test(Hole hole, Point_3 v, Point_3 fromA, Point_3 fromB) {
	
	std::cout << "face collision func" << std::endl;
	
	//Get the three points of a face
	//std::cout << hole.patch_faces.size() << hole.patch_faces_custom.size() << std::endl;
	for (auto i = 0; i < hole.patch_faces_custom.size(); i++) {
		Face f = hole.patch_faces_custom[i];

		Point_3 a = f.vertices[0];
		Point_3 b = f.vertices[1];
		Point_3 c = f.vertices[2];
		//std::cout << a <<"   " << b << "   " << c << std::endl;
		//Point_3 v2(v.x(), v.y(), v.z());
		if (a == v || b == v || c == v) {
			continue;
		}
		if (a == fromA || b == fromA || c == fromA) {
			continue;
		}
		if (fromB != Point_3()) {
			if (a == fromB || b == fromB || c == fromB) {
				continue;
			}
		}
		Triangle_3 t1(a, b, c);
		Triangle_3 t2(v, fromA, fromB);
		//CGAL::Object result;
		bool result = CGAL::do_intersect(t1, t2);
		//std::cout << "works till here" << std::endl;
		if (result){
			//std::cout << "face func end" << std::endl;
			return true;
		}
	}
	//std::cout << "face func end" << std::endl;
	return false;
}

bool Mesh_holes::merge_vertices(vertex_descriptor new_vd) {
	std::cout << "merge vertices func" << std::endl;
	if (new_vd > 10000000) {
		return true;
	}

	auto v_idx = std::distance(current_hole.vertices.begin(), std::find(current_hole.vertices.begin(), current_hole.vertices.end(), new_vd));
	std::cout << "new vd " << new_vd << std::endl;
	int idx_before = v_idx - 1;
	int idx_after = v_idx + 1;

	if (idx_before < 0) {
		idx_before = current_hole.vertices.size() - 1;
	}
	if (idx_after > current_hole.vertices.size() - 1) {
		idx_after = 0;
	}

	Point_3 a = mesh.point(current_hole.vertices[idx_before]);
	Point_3 b = mesh.point(current_hole.vertices[idx_after]);

	Point_3 v = mesh.point(new_vd);
	//std::cout << "works till here" << std::endl;
	std::vector<Point_3> compare = { a, b };
	std::vector<int> compare_indices = { idx_before, idx_after };
	for (int i = 0; i < 2; i++) {
		Point_3 t = compare[i];
		int dist = sqrt(pow(v.x() - t.x(), 2) + pow(v.y() - t.y(), 2) + pow(v.z() - t.z(), 2));
		if (dist <= merge_threshold) {
			
			//remove c index from filling
			//auto patch_idx = std::distance(hole.patch_vertices.begin(), std::find(hole.patch_vertices.begin(), hole.patch_vertices.end(), new_vd));
			//hole.patch_vertices.erase(hole.patch_vertices.begin() + patch_idx);

			//update_faces(hole, compare_indices[i], new_vd);

			angle_merge_vertex(current_hole, t, v);
		}
	}
	return false;
}

bool Mesh_holes::angle_merge_vertex(Hole hole, Point_3 old_vertex, Point_3 new_vertex) {
	
	for (int i = 0; i < angles.size(); i++) {
		auto angle = angles[i];
		if (angle->vertices[1] == old_vertex) {
			if (angle->previous->vertices[1] == new_vertex) {
				angle->previous->vertices[2] = angle->vertices[2];
				angle->next->vertices[0] = new_vertex;
			}
			else if (angle->next->vertices[1] == new_vertex) {
				angle->previous->vertices[2] = new_vertex;
				angle->next->vertices[0] = angle->vertices[0];
			}
			else {
				return false;
			}
			angle->previous->next = angle->next;
			angle->next->previous = angle->previous;
			double prev_degree = angle->previous->degree;
			double next_degree = angle->next->degree;
			//angles.erase(std::remove_if(angles.begin(), angles.end(), [prev_degree](auto& elem) { return elem.degree == prev_degree;}), angles.end());
			//angles.erase(std::remove_if(angles.begin(), angles.end(), [next_degree](auto& elem) { return elem.degree == next_degree;}), angles.end());

			angle->previous->degree = angle->previous->compute_angle(angle->previous->vertices[0], angle->previous->vertices[1], angle->previous->vertices[2]);
			angle->next->degree = angle->next->compute_angle(angle->next->vertices[0], angle->next->vertices[1], angle->next->vertices[2]);

			return true;
		}
	}
	return false;
}

void Mesh_holes::update_faces(Hole hole, vertex_descriptor old_vertex, vertex_descriptor new_vertex) {
	for (int i = 0; i < hole.patch_faces.size(); i++) {
		face_descriptor fd = hole.patch_faces[i];
		//halfedge_descriptor hd = halfedges_around_target(new_vertex, mesh);
		//for (vertex_descriptor vd : vertices_around_face(hd, mesh))
		//{
		//	std::cout << vd << std::endl;
		//}
	}
}

Angle* Mesh_holes::getAngle() {
	Angle* angle = angles.front();
	angles.erase(angles.begin());
	int count = 0;
	while (angle->update) {
		angles.push_back(angle);
		angle = angles.front();
		angles.erase(angles.begin());

		if (++count >= angles.size()) {
			throw "No more angles available for update";
		}
	}
	return angle;
}

Mesh Mesh_holes::get_mesh() {
	return mesh;
}

std::vector<Hole> Mesh_holes::get_hole_list() {
	return hole_list;
}

void Mesh_holes::holefill_benchmark() {
	int nb_holes = 0;
	for (halfedge_descriptor h : mesh.halfedges())
	{
		if (mesh.is_border(h))
		{
			std::vector<face_descriptor>  patch_facets;
			std::vector<vertex_descriptor> patch_vertices;
			bool success = CGAL::cpp11::get<0>(
				CGAL::Polygon_mesh_processing::triangulate_refine_and_fair_hole(
					mesh,
					h,
					std::back_inserter(patch_facets),
					std::back_inserter(patch_vertices),
					CGAL::Polygon_mesh_processing::parameters::vertex_point_map(get(CGAL::vertex_point, mesh)).
					geom_traits(Kernel())));
			std::cout << " Number of facets in constructed patch: " << patch_facets.size() << std::endl;
			std::cout << " Number of vertices in constructed patch: " << patch_vertices.size() << std::endl;
			std::cout << " Fairing : " << (success ? "succeeded" : "failed") << std::endl;
			++nb_holes;
		}
	}
}

Mesh_holes::~Mesh_holes() {
	angles.clear();
}