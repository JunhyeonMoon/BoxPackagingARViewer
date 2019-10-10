#pragma once
#include "app_base.h"
#include "utility_functions.h"
#include "camera.h"
#include <FindSurface.h>
#include <librealsense/rs.hpp>

struct UserData : public AppBase {
	
	//////전역변수 대신 이곳에 변수를 선언한다.
	// Application states
	int width, height;
	glm::mat4 VP;
	glm::mat4 T;

	std::string s1;
	std::string s2;
	std::string s3;
	std::string s_unit;

	std::vector<texture_info> wrapping_texture;
	bool select_mode = false;
	std::vector<glm::vec2> select_pos;
	bool isSelectWrapper = false;
	int WrapperIdx = 0;
	int floor_idx = -1;
	int rbox = 0;

	bool main_mode = true;
	bool camera_mode = false;
	bool edit_mode = false;
	bool size_mode = false;
	bool find_plane = false;
	bool rotation_mode = false;
	bool zoom_mode = false;
	bool panning_mode = false;

	enum class InterfaceState : char {
		Hide = 0,
		Entering = 1,
		Entered = 2,
		Exiting = 3
	};
	typedef InterfaceState IS;

	InterfaceState state = InterfaceState::Hide;

	TextRenderer tr;

	GLFWcursor* std_cursor;
	GLFWcursor* zoom_cursor;
	GLFWcursor* rotate_cursor;
	GLFWcursor* panning_cursor;
	GLFWcursor* plane_cursor;



	// OpenGL Objects
	Program program_pointcloud;
	VertexArray VAO_pointcloud;
	
	Program program_outlier;
	VertexArray VAO_outlier;

	Program program_box;
	VertexArray VAO_box;

	Program program_box_edge;
	VertexArray VAO_box_edge;

	Program program_edit;
	VertexArray VAO_edit;

	Program program_select;
	VertexArray VAO_select;

	Program program_arrow;
	VertexArray VAO_arrow;

	Program program_arrow_line;
	VertexArray VAO_arrow_line;

	Program program_direction;
	VertexArray VAO_direction;

	Program program_side;
	VertexArray VAO_side;

	// FindSurface
	FIND_SURFACE_CONTEXT fs;
	FS_FEATURE_RESULT result = {};
	FS_FEATURE_TYPE type = FS_FEATURE_TYPE::FS_TYPE_PLANE;

	// RealSense
	rs::context ctx;
	rs::device* dev = nullptr;
	rs::intrinsics depth_intrin;
	rs::intrinsics color_intrin;
	rs::extrinsics depth_to_color;
	rs::extrinsics color_to_depth;
	float scale = 0.f;
	bool Findplane = false;
	bool Findbox = false;
	bool edit_enable = false;
	bool texture_change_mode = false;
	bool editmode = false;
	bool transform_side_mode = false;
	float avgz = 0;
	int avg_index = -1;
	float box_height = 0;
	float box_vert = 0;
	float box_hori = 0;
	int side_count = 0;
	float unit_cm = (float)0.02;
	int selected_texture = -1;
	int selected_arrow = -1;


	// Trackball
	camera::Trackball trackball;
	camera::Trackball::Behavior behavior;

	// Pointcloud Data
	std::vector<glm::vec3> depth_points;
	std::vector<ubyte3> depth_colors;

	std::vector<glm::vec3> inlier_points;
	std::vector<ubyte3> inlier_colors;

	std::vector<glm::vec3> outlier_points;
	std::vector<ubyte3> outlier_colors;
	std::vector<glm::vec3> foutlier_points;
	std::vector<ubyte3> foutlier_colors;
	
	std::vector<glm::vec3> box_points;
	std::vector<unsigned int> box_indices;

	std::vector<glm::vec3> box_points_org;

	std::vector<glm::vec3> arrow_points;
	std::vector<glm::vec3> arrow_line_points;

	//////보조로 사용될 함수도 이곳에 선언한다.
	void Init_OpenGL(int width, int height);
	void Init_FindSurface();
	void Init_RealSense();
	void Init_Trackball(int width, int height);
	void Init_BoxSpecification();
	void Init_Interface();
	void print_explane();

	void Update_RealSense();
	void Update_FindSurface();
	void Run_FindSurface(unsigned int index, float depth);
	int Find_Floor(); //depth_points중 바닥에 해당하는 point의 index를 리턴
	void Cutblob(float error);
	void Run_FindBox();
	int Recommend_Box();
	void Run_edit();
	void Load_texture();
	void Load_texture(const char* path, int channel, texture_info& tex);
	void Load_texture(int type, texture_info& tex);
	void Run_selectPaper(int index);
	void Size_up();
	void Size_down();
	void Set_arrows();


	void Cleanup_OpenGL();
	void Cleanup_FindSurface();
	void Cleanup_RealSense();
	void Cleanup_Interface();

	void Save_box();
	void Load_box();

	/////////interface 관련 멤버
	std::vector<interface_info> main_interface;
	std::vector<interface_info> edit_interface;
	std::vector<interface_info> camera_interface;
	std::vector<interface_info> unit_interface;
	std::vector<interface_info> texture_interface;


	void CreateButton(std::vector<interface_info>& interfaces, int type, glm::vec2 location, float width, float height, const char* text, const char* path = NULL, int channel = 3);
	void CreateButton(std::vector<interface_info>& interfaces, int type, glm::ivec2 location, int width, int height, const char* text, const char* path = NULL);
	void cleanMouseBehavior();
	int checkClickButton(std::vector<interface_info>& interfaces, float x, float y); //어느버튼이 클릭됬는지 리턴
	

	//box 규격
	std::vector<box_specification> box_ho; //국내 박스 규격
	std::vector<box_specification> box_ho_ems; //국제 박스 규격
};

