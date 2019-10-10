#pragma once
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

struct ray { glm::vec3 org, dir; };
struct isect { glm::vec3 hit_pos; glm::vec2 hit_tc; glm::vec3 hit_norm; glm::vec3 bc; };

namespace camera {

	struct Camera {
		glm::vec3 eye, at, up;
		glm::vec3 dir() { return eye - at; }
		float width, height;
		float fovy;
		float dnear, dfar;
		glm::mat4 view_matrix;
		glm::mat4 projection_matrix;

		void updateViewMatrix();
		void updateProjectionMatrix();
	};

	struct Trackball {

		static float zoom_speed;
		static float pan_speed;
		static float rotate_speed;
		static float translate_speed;

		enum class Behavior {
			NOTHING,
			ROTATING,
			PANNING,
			ZOOMING,
			ROLLING
		};

		Behavior behavior;

		struct {
			bool up, down, left, right, forward, backward;
			void reset() { up = down = left = right = forward = backward = false; }
			bool any() { return up || down || left || right || forward || backward; }
		} movement = {};

		Camera curr, prev, home;
		float x, y;

		void reset();
		void mouse(float x, float y, Behavior behavior);
		void motion(float x, float y);
		void update(double elapsed_time);

		void zoom(float sign);

		glm::mat4 view_matrix() { return curr.view_matrix; }
		glm::mat4 projection_matrix() { return curr.projection_matrix; }
		void set_eye(glm::vec3 v) { curr.eye = v; }
		glm::vec3 get_eye() { return curr.eye; }

		ray generate_ray(float x, float y);

	private:
		void pan(float x, float y);
		void roll(float x, float y);
		void rotate(float x, float y);
		void zoom(float x, float y);
	};
};

bool intersect_triangle(
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
	ray r, isect& intersection);

bool intersect_triangle(
	glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
	glm::vec2 t0, glm::vec2 t1, glm::vec2 t2,
	ray r, isect& intersection);

bool intersect_points(
	std::vector<glm::vec3>& points, 
	ray r, unsigned int& index, 
	float radius = 0.f);
