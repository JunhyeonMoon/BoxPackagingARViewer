#include "stdafx.h"
#include "camera.h"

namespace camera {

	float Trackball::zoom_speed = 0.05f;
	float Trackball::pan_speed = 0.1f;
	float Trackball::rotate_speed = 0.75f;
	float Trackball::translate_speed = 0.0005f;

	void Camera::updateViewMatrix() { view_matrix = glm::lookAt(eye, at, up); }
	void Camera::updateProjectionMatrix() {
		//projection_matrix = glm::ortho(-0.5f*width, 0.5f*width, -0.5f*height, 0.5f*height, dnear, dfar); 
		projection_matrix = glm::perspective(fovy, width / height, dnear, dfar);
	}


	void Trackball::reset() {
		curr = prev = home;
	}

	void Trackball::mouse(float x, float y, Trackball::Behavior behavior) {
		prev = curr;
		x = 2 * x - 1;
		y = 1 - 2 * y;
		this->x = glm::clamp(x, -1.f, 1.f);
		this->y = glm::clamp(y, -1.f, 1.f);
		this->behavior = behavior;
	}

	void Trackball::motion(float x, float y) {
		x = 2 * x - 1;
		y = 1 - 2 * y;
		x = glm::clamp(x, -1.f, 1.f);
		y = glm::clamp(y, -1.f, 1.f);

		switch (behavior) {
		case Trackball::Behavior::PANNING: pan(x, y); break;
		case Trackball::Behavior::ROLLING: roll(x, y); break;
		case Trackball::Behavior::ROTATING: rotate(x, y); break;
		case Trackball::Behavior::ZOOMING: zoom(x, y); break;
		}
	}

	void Trackball::update(double elapsed_time) {
		if (movement.any()) {
			float distance = float(glm::length(home.dir())*elapsed_time*translate_speed);
			glm::vec3 n = -glm::normalize(curr.dir());
			glm::vec3 u = glm::normalize(glm::cross(curr.up, n));
			glm::vec3 v = glm::normalize(glm::cross(n, u));

			if (movement.forward) { curr.eye = curr.eye + n * distance; }
			if (movement.backward) { curr.eye = curr.eye - n * distance; }
			if (movement.left) { curr.eye = curr.eye - u * distance; }
			if (movement.right) { curr.eye = curr.eye + u * distance; }
			if (movement.up) { curr.eye = curr.eye + v * distance; }
			if (movement.down) { curr.eye = curr.eye - v * distance; }
		}

		curr.updateViewMatrix();
		curr.updateProjectionMatrix();
	}

	void Trackball::pan(float x, float y) {
		float scale = glm::length(curr.dir())*pan_speed;
		float dx = (x - this->x)*scale;
		float dy = (y - this->y)*scale;

		glm::vec3 n = glm::normalize(prev.dir());
		glm::vec3 u = glm::normalize(glm::cross(prev.up, n));
		glm::vec3 v = glm::normalize(glm::cross(n, u));
		glm::vec3 d = u * dx + v * dy;

		curr.eye = prev.eye - d;
		curr.at = prev.at - d;

		curr.updateViewMatrix();
	}

	void Trackball::roll(float x, float y) {
		float dx = (x - this->x)*rotate_speed;
		float dy = (y - this->y)*rotate_speed;

		glm::vec3 n = glm::normalize(prev.dir());
		glm::vec3 u = glm::normalize(glm::cross(prev.up, n));
		glm::vec3 v = glm::normalize(glm::cross(n, u));

		curr.up = glm::rotate(dy*100.f*glm::pi<float>() / 180.f, n) * glm::vec4(v, 0);
		curr.updateViewMatrix();
	}

	void Trackball::rotate(float x, float y) {
		x = (x - this->x)*rotate_speed;
		y = (y - this->y)*rotate_speed;
		float z = 1 - (x*x + y * y);
		glm::vec3 p1 = { x, y, sqrtf(glm::max(0.f, z)) };
		glm::vec3 n = glm::cross(glm::vec3{ 0, 0, 1 }, p1)*glm::mat3(prev.view_matrix);
		float len = glm::length(n);
		float angle = asinf(glm::min(len, 0.999f));

		glm::mat4 T = glm::translate(prev.at);
		glm::mat4 R = glm::rotate(-angle, glm::normalize(n));

		curr.eye = (T*R*glm::vec4(prev.dir(), 1));
		curr.at = prev.at;
		curr.up = (R*glm::vec4(prev.up, 0));
		curr.updateViewMatrix();
	}

	void Trackball::zoom(float x, float y) {
		//float sign = y < this->y ? 1.f : -1.f;
		//this->y = y;
		//this->x = x;
		//float dt = 1.f+sign*zoom_speed;
		//curr.width *= dt;
		//curr.height *= dt;
		//curr.updateProjectionMatrix();

		float t = glm::min(0.7f, y - this->y);
		curr.eye = glm::mix(prev.eye, prev.at, t);

		curr.updateViewMatrix();
	}

	void Trackball::zoom(float sign) {
		float dt = 1.f + sign * zoom_speed;
		curr.width *= dt;
		curr.height *= dt;
		curr.updateProjectionMatrix();
	}

	ray Trackball::generate_ray(float x, float y) {
		x = 2 * x - 1;
		y = 1 - 2 * y;
		x = glm::clamp(x, -1.f, 1.f);
		y = glm::clamp(y, -1.f, 1.f);

		ray r;
		r.org = curr.eye;
		float near_plane_height = curr.dnear * glm::tan(curr.fovy*0.5f);
		float near_plane_width = near_plane_height * curr.width / curr.height;
		glm::vec3 ray_at = glm::normalize(-curr.dir())*curr.dnear;
		glm::vec3 ray_right = glm::normalize(glm::cross(ray_at, curr.up));
		glm::vec3 ray_up = glm::normalize(glm::cross(ray_right, ray_at));

		r.dir = glm::normalize(ray_at + near_plane_height * y*ray_up + near_plane_width * x * ray_right);

		return r;
	}
}

/* https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates */
glm::vec3 barycentric(glm::vec3 p, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
	glm::vec3 e0 = v1 - v0;
	glm::vec3 e1 = v2 - v0;
	glm::vec3 e2 = p - v0;
	float d00 = glm::dot(e0, e0);
	float d01 = glm::dot(e0, e1);
	float d11 = glm::dot(e1, e1);
	float d20 = glm::dot(e2, e0);
	float d21 = glm::dot(e2, e1);
	float denom = d00 * d11 - d01 * d01;
	float b0 = (d11*d20 - d01 * d21) / denom;
	float b1 = (d00*d21 - d01 * d20) / denom;
	float b2 = 1.f - (b0 + b1);
	return glm::vec3(b0, b1, b2);
}

bool intersect_triangle(
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
	ray r, isect& intersection) {

	glm::vec3 e0 = v1 - v0;
	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v0 - v2;

	glm::vec3 norm = glm::normalize(glm::cross(e0, -e1));

	float D = -glm::dot(norm, v0);
	float t = -(D + glm::dot(norm, r.org)) / glm::dot(norm, r.dir);

	glm::vec3 P = r.org + t * r.dir;

	glm::vec3 d0 = P - v0;
	glm::vec3 d1 = P - v1;
	glm::vec3 d2 = P - v2;

	glm::vec3 n0 = glm::normalize(glm::cross(e0, d0));
	glm::vec3 n1 = glm::normalize(glm::cross(e1, d1));
	glm::vec3 n2 = glm::normalize(glm::cross(e2, d2));

	float d01 = glm::dot(n0, n1);
	float d12 = glm::dot(n1, n2);
	float d20 = glm::dot(n2, n0);

	intersection.hit_pos = P;
	intersection.hit_tc = {};
	intersection.hit_norm = norm;
	intersection.bc = {};

	bool isFront = glm::angle(intersection.hit_norm, r.dir) > glm::pi<float>() / 2;

	return d01 > 0.f && d12 > 0.f && d20 > 0.f && isFront;
}

bool intersect_triangle(
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
	glm::vec2 t0, glm::vec2 t1, glm::vec2 t2,
	ray r, isect& intersection) {

	glm::vec3 e0 = v1 - v0;
	glm::vec3 e1 = v2 - v1;
	glm::vec3 e2 = v0 - v2;

	glm::vec3 norm = glm::normalize(glm::cross(e0, -e1));

	float D = -glm::dot(norm, v0);
	float t = -(D + glm::dot(norm, r.org)) / glm::dot(norm, r.dir);

	glm::vec3 P = r.org + t * r.dir;

	glm::vec3 d0 = P - v0;
	glm::vec3 d1 = P - v1;
	glm::vec3 d2 = P - v2;

	glm::vec3 n0 = glm::normalize(glm::cross(e0, d0));
	glm::vec3 n1 = glm::normalize(glm::cross(e1, d1));
	glm::vec3 n2 = glm::normalize(glm::cross(e2, d2));

	float d01 = glm::dot(n0, n1);
	float d12 = glm::dot(n1, n2);
	float d20 = glm::dot(n2, n0);

	glm::vec3 bc = barycentric(P, v0, v1, v2);

	intersection.hit_pos = P;
	intersection.hit_tc = bc.x*t0 + bc.y*t1 + bc.z*t2;
	intersection.hit_norm = norm;
	intersection.bc = bc;

	bool isFront = glm::angle(intersection.hit_norm, r.dir) > glm::pi<float>() / 2;

	return d01 > 0.f && d12 > 0.f && d20 > 0.f && isFront;

}

// 점과 ray 의 수직거리
float get_distance_between_ray_and_point(const ray& r, const glm::vec3& p) {
	glm::vec3 po = p - r.org;
	return glm::length(po - r.dir * glm::dot(r.dir, po));
}

// ray로부터 radius 반경에 들어오는 점(의 index)을 일단 (candidate에) 모은다.
// candidate에 모인 점들의 index를 다시 검토하여 화면(ray origin)에서 가장 가까운 점을 선택.
// radius가 0 이면 candidate 에 아무것도 들어갈 수 없으므로 그냥 무조건 ray로부터 수직거리가 가까운 점을 선택.
bool intersect_points(
	std::vector<glm::vec3>& points,
	ray ray, unsigned int& index, float radius) {

	// ray 로부터 radius 거리안에 들어오는 점의 인덱스를 넣을 공간.
	std::vector<unsigned int> candidate; // 후보

	int index_min_dist = 0; // index of the point having minimum distance to the ray.
	float min_dist = FLT_MAX; // minimum distance;
	for (int k = 0; k < static_cast<int>(points.size()); k++) {
		const glm::vec3& pt = reinterpret_cast<glm::vec3&>(points[k]);
		float dist = get_distance_between_ray_and_point(ray, pt);
		if (dist < radius) {	// 점이 반경안에 들어오면
			candidate.push_back(k);  // 인덱스를 넣고
		}
		if (dist < min_dist) {  // 혹시 candidate에 아무것도 안들어갈 것을 대비해서
			index_min_dist = k; // ray 에서 가장 가까운 점도 찾아놓는다.
			min_dist = dist;
		}
	}

	if (candidate.size() == 0) { // candidate 에 아무것도 없으면
		index = index_min_dist;   // 뽑아놓은 가장 가까운 것을 선택
		return (radius == 0.f) ? true : false;	// radius 가 0이었으면 의도한 것이므로 true, 아니라면 반경안에 아무것도 없으니 false
	}

	for (unsigned int k : candidate) { // candidate 에 뭔가 있으면
		glm::vec3& pt = reinterpret_cast<glm::vec3&>(points[k]);
		float dist_to_screen = glm::distance(ray.org, pt);
		if (dist_to_screen < min_dist) { // 화면에서 가장 가까운 점을 찾는다.
			index_min_dist = k;
			min_dist = dist_to_screen;
		}
	}

	return true;
}