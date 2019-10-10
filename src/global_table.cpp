#include "stdafx.h"
#include "global_table.h"
#include "shader_sources.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// 보조로 사용될 함수의 정의를 이곳에 작성한다.

void UserData::Init_OpenGL(int width, int height) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//////pointcloud
	std::map<GLenum, const char*> pointcloud_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("pointcloud_vs") },
		{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("pointcloud_fs") }
	};
	program_pointcloud = CreateProgram(pointcloud_shader_map);

	std::vector<VBO_info> VBO_info_pointcloud = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) },
		{ VBO_COLOR, 0, nullptr, 0, sizeof(ubyte3) }
	};
	std::vector<attrib_info> attrib_info_pointcloud = {
		{ 0, GL_FLOAT, 3, 0 },
		{ 1, GL_UNSIGNED_BYTE, 3, 0 }
	};
	VAO_pointcloud = CreateVertexArray(VBO_info_pointcloud, attrib_info_pointcloud);

	/////outlier
	std::map<GLenum, const char*> outlier_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("pointcloud_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("pointcloud_fs") }
	};
	program_outlier = CreateProgram(outlier_shader_map);

	std::vector<VBO_info> VBO_info_outlier = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) },
	{ VBO_COLOR, 0, nullptr, 0, sizeof(ubyte3) }
	};
	std::vector<attrib_info> attrib_info_outlier = {
		{ 0, GL_FLOAT, 3, 0 },
	{ 1, GL_UNSIGNED_BYTE, 3, 0 }
	};
	VAO_outlier = CreateVertexArray(VBO_info_outlier, attrib_info_outlier);


	//////box
	std::map<GLenum, const char*> box_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("box_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("box_fs") }
	};
	program_box = CreateProgram(box_shader_map);

	std::vector<VBO_info> VBO_info_box = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_box = {
		{ 0, GL_FLOAT, 3, 0 }
	};

	VAO_box = CreateVertexArray(VBO_info_box, attrib_info_box);

	///////box edge
	std::map<GLenum, const char*> box_edge_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("box_edge_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("box_edge_fs") }
	};
	program_box_edge = CreateProgram(box_edge_shader_map);

	std::vector<VBO_info> VBO_info_box_edge = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_box_edge = {
		{ 0, GL_FLOAT, 3, 0 }
	};

	GLuint edge_indices[24] = { 
	0, 1, 1, 3, 3, 2, 2, 0,//밑면
	4, 5, 5, 7, 7, 6, 6, 4,//윗면
	4, 0, 5, 1, 7, 3, 6, 2 //옆면
	};

	VAO_box_edge = CreateVertexArray(VBO_info_box_edge, attrib_info_box_edge, std::size(edge_indices), edge_indices);

	/////edit box
	Load_texture();
	//뒤집어짐
	glm::vec2 quad_texcoord[36]{
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f }
	};
	
	std::map<GLenum, const char*> edit_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("edit_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("edit_fs") }
	};
	program_edit = CreateProgram(edit_shader_map);
	
	std::vector<VBO_info> VBO_info_edit = {
		{VBO_POS, 0, nullptr, 0, sizeof(glm::vec3)},
		{VBO_TEX, 36, quad_texcoord, 0, sizeof(glm::vec2)},
		{VBO_NORM, 0, nullptr, 0, sizeof(glm::vec3)}
	};
	std::vector<attrib_info> attrib_info_edit = {
		{0, GL_FLOAT, 3, 0},
		{1, GL_FLOAT, 2, 0},
		{2, GL_FLOAT, 3, 0 }
	};
	
	VAO_edit = CreateVertexArray(VBO_info_edit, attrib_info_edit);

	/////side
	std::map<GLenum, const char*> side_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("side_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("side_fs") }
	};
	program_side = CreateProgram(side_shader_map);

	std::vector<VBO_info> VBO_info_side = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_side = {
		{ 0, GL_FLOAT, 3, 0 }
	};

	VAO_side = CreateVertexArray(VBO_info_side, attrib_info_side);


	/////arrow
	//std::map<GLenum, const char*> arrow_shader_map = {
	//	{ GL_VERTEX_SHADER, ShaderSource::GetSource("arrow_vs") },
	//{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("arrow_fs") }
	//};
	//program_arrow = CreateProgram(arrow_shader_map);
	//
	//std::vector<VBO_info> VBO_info_arrow = {
	//	{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	//};
	//std::vector<attrib_info> attrib_info_arrow = {
	//	{ 0, GL_FLOAT, 3, 0 }
	//};
	//
	//VAO_arrow = CreateVertexArray(VBO_info_arrow, attrib_info_arrow);

	/////arrow_line
	std::map<GLenum, const char*> arrow_line_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("arrow_line_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("arrow_line_fs") }
	};
	program_arrow_line = CreateProgram(arrow_line_shader_map);

	std::vector<VBO_info> VBO_info_arrow_line = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_arrow_line = {
		{ 0, GL_FLOAT, 3, 0 }
	};

	VAO_arrow_line = CreateVertexArray(VBO_info_arrow_line, attrib_info_arrow_line);

	/////direction
	std::map<GLenum, const char*> direction_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("direction_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("direction_fs") }
	};
	program_direction = CreateProgram(direction_shader_map);

	std::vector<VBO_info> VBO_info_direction = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_direction = {
		{ 0, GL_FLOAT, 3, 0 }
	};

	VAO_direction = CreateVertexArray(VBO_info_direction, attrib_info_direction);


	/////select wrapping paper button
	glm::vec2 select_posArray[30]{
	{ -0.9f, 0.9f },{ -0.9f, 0.8f },{ -0.8f, 0.8f },
	{ -0.8f, 0.8f },{ -0.8f, 0.9f },{ -0.9f, 0.9f },
	{ -0.6f, 0.9f },{ -0.6f, 0.8f },{ -0.5f, 0.8f },
	{ -0.5f, 0.8f },{ -0.5f, 0.9f },{ -0.6f, 0.9f },
	{ -0.3f, 0.9f },{ -0.3f, 0.8f },{ -0.2f, 0.8f },
	{ -0.2f, 0.8f },{ -0.2f, 0.9f },{ -0.3f, 0.9f },
	{ 0.f, 0.9f },{ 0.f, 0.8f },{ 0.1f, 0.8f },
	{ 0.1f, 0.8f },{ 0.1f, 0.9f },{ 0.f, 0.9f },
	{ 0.3f, 0.9f },{ 0.3f, 0.8f },{ 0.4f, 0.8f },
	{ 0.4f, 0.8f },{ 0.4f, 0.9f },{ 0.3f, 0.9f },
	};

	glm::vec2 select_texcoord[30]{
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f },
	{ 0.f, 1.f },{ 0.f, 0.f },{ 1.f, 0.f },
	{ 1.f, 0.f },{ 1.f, 1.f },{ 0.f, 1.f }
	};
	
	select_pos.clear(); select_pos.reserve(std::size(select_posArray));
	for (int i = 0; i < std::size(select_posArray); i++) {
		select_pos.push_back(select_posArray[i]);
	}

	std::map<GLenum, const char*> select_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("select_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("select_fs") }
	};

	program_select = CreateProgram(select_shader_map);
	
	std::vector<VBO_info> VBO_info_select = {
		{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec2) },
		{ VBO_TEX, std::size(select_texcoord), select_texcoord, 0, sizeof(glm::vec2) }
	};
	std::vector<attrib_info> attrib_info_select = {
		{ 0, GL_FLOAT, 2, 0 },
		{ 1, GL_FLOAT, 2, 0 }
	};

	VAO_select = CreateVertexArray(VBO_info_select, attrib_info_select);
	VertexBufferData(VAO_select, VBO_POS, select_pos.size(), sizeof(glm::vec2), &select_pos.at(0), GL_STATIC_DRAW);

	/////////////
	/////text renderer
	tr.Init(ShaderSource::GetSource("str_render_text_vs"), ShaderSource::GetSource("str_render_text_fs"));	

	//////OpenGL state
	glPointSize(5.0f);
	glEnable(GL_CULL_FACE); // 삼각형이 뒷면이 보이는 경우 그리지 않는다.
	glEnable(GL_DEPTH_TEST); // 뒷면에 가려지는 도형은 그리지 않는다.
	
	// 나중에 pixel unpack 연산 (pixel 데이터를 GPU로 보내는 작업)을 할 때 주석을 풀자.
	
	//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	//glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	//glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
}

void UserData::Load_texture() {
	// Basic usage (see HDR discussion below for HDR usage):
	//    int x,y,n;
	//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	//    stbi_image_free(data)

	// Standard parameters:
	//    int *x                 -- outputs image width in pixels
	//    int *y                 -- outputs image height in pixels
	//    int *channels_in_file  -- outputs # of image components in image file
	//    int desired_channels   -- if non-zero, # of image components requested in result
	wrapping_texture.clear();
	wrapping_texture.resize(5);
	for (int i = 0; i < 5; i++) {
		switch (i) {
		case 0:
			wrapping_texture[i].data = stbi_load("WrappingPaper\\1.jpg", &wrapping_texture[i].width, &wrapping_texture[i].height, &wrapping_texture[i].n, 0);
			break;
		case 1:
			wrapping_texture[i].data = stbi_load("WrappingPaper\\2.jpg", &wrapping_texture[i].width, &wrapping_texture[i].height, &wrapping_texture[i].n, 0);
			break;
		case 2:
			wrapping_texture[i].data = stbi_load("WrappingPaper\\3.jpg", &wrapping_texture[i].width, &wrapping_texture[i].height, &wrapping_texture[i].n, 0);
			break;
		case 3:
			wrapping_texture[i].data = stbi_load("WrappingPaper\\4.jpg", &wrapping_texture[i].width, &wrapping_texture[i].height, &wrapping_texture[i].n, 0);
			break;
		case 4:
			wrapping_texture[i].data = stbi_load("WrappingPaper\\5.jpg", &wrapping_texture[i].width, &wrapping_texture[i].height, &wrapping_texture[i].n, 0);
			break;
		default:
			printf("Load_texture error!\n"); return; break;
		}
		
		if (wrapping_texture[i].data == NULL) {
			printf("Load_texture failed!!\n");
			return;
		}
		wrapping_texture[i].tex = CreateTexture2D(wrapping_texture[i].width, wrapping_texture[i].height, 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, wrapping_texture[i].data);
		stbi_image_free(wrapping_texture[i].data);
	}
}

void UserData::Load_texture(const char* path, int channel, texture_info& tex) {
	tex.data = stbi_load(path, &tex.width, &tex.height, &tex.n, channel);

	if (tex.data == NULL) {
		printf("Load inteface texture error!!\n");
		return;
	}
	
	if (channel == 4) {
		tex.tex = CreateTexture2D(tex.width, tex.height, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
	}
	else {
		tex.tex = CreateTexture2D(tex.width, tex.height, 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, tex.data);
	}
	stbi_image_free(tex.data);
}


void UserData::Load_texture(int type, texture_info& tex) {
	
	switch (type) {
	case 0:
		tex.data = stbi_load("Interface\\background.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	case 1: 
		tex.data = stbi_load("Interface\\button1.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	case 2:
		tex.data = stbi_load("Interface\\button2.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	case 3:
		tex.data = stbi_load("Interface\\main_button.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	case 4:
		tex.data = stbi_load("Interface\\textground.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	case 5:
		tex.data = stbi_load("Interface\\textgroundb.png", &tex.width, &tex.height, &tex.n, 4);
		break;


	default:
		tex.data = stbi_load("Interface\\button1.png", &tex.width, &tex.height, &tex.n, 4);
		break;
	}
	
	if (tex.data == NULL) {
		printf("Load inteface texture error!!\n");
		return;
	}
	tex.tex = CreateTexture2D(tex.width, tex.height, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
	stbi_image_free(tex.data);
}

void UserData::Init_FindSurface() {
	switch (createFindSurface(&fs)) {
	case FS_OUT_OF_MEMORY:		throw std::runtime_error("FindSurface: failed to create a context (out of memory).\n");
	case FS_LICENSE_EXPIRED:	throw std::runtime_error("FindSurface: failed to create a context (license expired).\n");
	case FS_LICENSE_UNKNOWN:	throw std::runtime_error("FindSurface: failed to create a context (license unknown).\n");
	}

	// initialize parameters (not mandatory, but recommended)
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_ACCURACY, 0.003f);
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_MEAN_DIST, 0.01f);
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_TOUCH_R, 0.045f);
}

void UserData::Init_RealSense() {
	rs::log_to_console(rs::log_severity::warn);

	if (ctx.get_device_count() == 0) {
		throw std::runtime_error("RealSense: there is no RealSense device connected.\n");
	}

	dev = ctx.get_device(0);
	dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
	dev->enable_stream(rs::stream::color, rs::preset::best_quality);
	dev->set_option(rs::option::fisheye_strobe, 1);
	dev->set_option(rs::option::fisheye_external_trigger, 1);
	dev->enable_stream(rs::stream::fisheye, rs::preset::best_quality);
	dev->start();

	depth_intrin = dev->get_stream_intrinsics(rs::stream::depth);
	depth_to_color = dev->get_extrinsics(rs::stream::depth, rs::stream::rectified_color);
	color_to_depth = dev->get_extrinsics(rs::stream::rectified_color, rs::stream::depth);
	color_intrin = dev->get_stream_intrinsics(rs::stream::rectified_color);
	scale = dev->get_depth_scale();
}

void UserData::Init_Trackball(int width, int height) {
	trackball.curr.eye = glm::vec3{};
	trackball.curr.at = glm::vec3{ 0, 0, 1 };
	trackball.curr.up = glm::vec3{ 0, -1, 0 };
	trackball.curr.width = float(width) / 1000;
	trackball.curr.height = float(height) / 1000;
	trackball.curr.fovy = glm::radians<float>(46.f);
	trackball.curr.dnear = 0.001f;
	trackball.curr.dfar = 20.0f;
	trackball.prev = trackball.home = trackball.curr;
}

void UserData::Init_Interface() {
	////Init cursor
	//tex.data = stbi_load("Interface\\button1.png", &tex.width, &tex.height, &tex.n, 4);
	//
	//if (tex.data == NULL) {
	//	printf("Load inteface texture error!!\n");
	//	return;
	//}
	//tex.tex = CreateTexture2D(tex.width, tex.height, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
	//stbi_image_free(tex.data);

 	GLFWimage cursor_image;
	std_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	panning_cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

	//zoom cursor
	cursor_image.pixels = stbi_load("Interface\\zoom_cursor.png", &cursor_image.width, &cursor_image.height, nullptr, 4);
	for (int i = 0; i < cursor_image.width * cursor_image.height * 4; i += 4) {
		cursor_image.pixels[i] = 255;
	}
	zoom_cursor = glfwCreateCursor(&cursor_image, 0, 0);
	stbi_image_free(cursor_image.pixels);

	//rotate cursor
	cursor_image.pixels = stbi_load("Interface\\rotate_cursor.png", &cursor_image.width, &cursor_image.height, nullptr, 4);
	for (int i = 0; i < cursor_image.width * cursor_image.height * 4; i += 4) {
		cursor_image.pixels[i] = 255;
	}
	rotate_cursor = glfwCreateCursor(&cursor_image, 0, 0);
	stbi_image_free(cursor_image.pixels);

	//plane cursor
	cursor_image.pixels = stbi_load("Interface\\plane_cursor.png", &cursor_image.width, &cursor_image.height, nullptr, 4);
	for (int i = 0; i < cursor_image.width * cursor_image.height * 4; i += 4) {
		cursor_image.pixels[i] = 255;
	}
	plane_cursor = glfwCreateCursor(&cursor_image, 0, 0);
	stbi_image_free(cursor_image.pixels);



	///////////////////
	main_interface.clear();
	main_interface.reserve(20);

	edit_interface.clear();
	edit_interface.reserve(20);

	texture_interface.clear();
	texture_interface.reserve(20);

	unit_interface.clear();
	unit_interface.reserve(20);

	camera_interface.clear();
	camera_interface.reserve(20);

	glm::vec2 lo;
	glm::ivec2 ilo;

	//////1024x768
	//////main interface
	/////type 0 : background  ///type 1, 2 : button
	lo = { -0.5f, 0.4f };
	CreateButton(main_interface, 4, lo, 0.5, 0.5, "Box Packaging AR Viewer");
	lo = { 0.f, -0.1f };
	CreateButton(main_interface, 3, lo, 0.3, 0.3, "Camera");
	Load_texture("Interface\\icon\\1.png",
		4, main_interface.back().icon_tex);
	lo = { 0.f, -0.5f };
	CreateButton(main_interface, 3, lo, 0.3, 0.3, "Packaging");
	Load_texture("Interface\\icon\\3d-modeling_ Flaticon Basic License._Smashicons.png",
		4, main_interface.back().icon_tex);

	//////camera interface
	ilo = { 854, 384 };
	CreateButton(camera_interface, 0, ilo, 340, 768, ""); //background

	ilo = { 764, 650 };
	CreateButton(camera_interface, 1, ilo, 150, 100, "Find Box!");
	Load_texture("Interface\\icon\\photo-camera_Daniel Bruce.png",
		4, camera_interface.back().icon_tex);

	ilo = { 919, 650 };
	CreateButton(camera_interface, 1, ilo, 150, 100, "resume");
	Load_texture("Interface\\icon\\undo_google.png",
		4, camera_interface.back().icon_tex);

	ilo = { 764, 540 };
	CreateButton(camera_interface, 1, ilo, 150, 100, "main");
	Load_texture("Interface\\icon\\home_Flaticon Basic License freepik.png",
		4, camera_interface.back().icon_tex);

	ilo = { 764, 400 };
	CreateButton(camera_interface, 2, ilo, 150, 100, "rotation");
	Load_texture("Interface\\icon\\rotate_freepik.png",
		4, camera_interface.back().icon_tex);

	ilo = { 919, 400 };
	CreateButton(camera_interface, 2, ilo, 150, 100, "zoom");
	Load_texture("Interface\\icon\\zoom_By Egor Rumyantsev.png",
		4, camera_interface.back().icon_tex);

	ilo = { 764, 290 };
	CreateButton(camera_interface, 2, ilo, 150, 100, "panning");
	Load_texture("Interface\\icon\\panning_google.png",
		4, camera_interface.back().icon_tex);
	
	lo = { -0.2f, -0.7f };//7
	CreateButton(camera_interface, 4, lo, 0.f, 0.f, "");
	lo = { -0.2f, -0.8f };//8
	CreateButton(camera_interface, 4, lo, 0.f, 0.f, "");
	lo = { -0.2f, -0.9f };//9
	CreateButton(camera_interface, 4, lo, 0.f, 0.f, "");


	/////edit interface
	ilo = { 854, 384 };
	CreateButton(edit_interface, 0, ilo, 340, 768, ""); //background
	
	ilo = { 919, 650 };
	CreateButton(edit_interface, 1, ilo, 150, 100, "wrapping"); 
	Load_texture("Interface\\icon\\texture_google.png",
		4, edit_interface.back().icon_tex);

	ilo = { 764, 650 };
	CreateButton(edit_interface, 1, ilo, 150, 100, "size control");
	Load_texture("Interface\\icon\\expand-button_Flaticon Basic License_Freepik.png",
		4, edit_interface.back().icon_tex);

	ilo = { 919, 540 };
	CreateButton(edit_interface, 1, ilo, 150, 100, "resume");
	Load_texture("Interface\\icon\\undo_google.png",
		4, edit_interface.back().icon_tex);
	
	ilo = { 764, 540 };
	CreateButton(edit_interface, 1, ilo, 150, 100, "main");
	Load_texture("Interface\\icon\\home_Flaticon Basic License freepik.png",
		4, edit_interface.back().icon_tex);

	ilo = { 764, 400 };
	CreateButton(edit_interface, 2, ilo, 150, 100, "rotation");
	Load_texture("Interface\\icon\\rotate_freepik.png",
		4, edit_interface.back().icon_tex);
	
	ilo = { 919, 400 };
	CreateButton(edit_interface, 2, ilo, 150, 100, "zoom");
	Load_texture("Interface\\icon\\zoom_By Egor Rumyantsev.png",
		4, edit_interface.back().icon_tex);

	ilo = { 764, 290 };
	CreateButton(edit_interface, 2, ilo, 150, 100, "panning");
	Load_texture("Interface\\icon\\panning_google.png",
		4, edit_interface.back().icon_tex);

	lo = { -0.2f, -0.7f };//8
	CreateButton(edit_interface, 4, lo, 0.f, 0.f, "");
	lo = { -0.2f, -0.8f };//9
	CreateButton(edit_interface, 4, lo, 0.f, 0.f, "");
	lo = { -0.2f, -0.9f };//10
	CreateButton(edit_interface, 4, lo, 0.f, 0.f, "");

	ilo = { 854, 120 };
	CreateButton(edit_interface, 0, ilo, 330, 230, "", "Interface\\box_size_table.png");
	edit_interface.back().id = -2;

	//unit interface
	lo = { -0.8f, -0.6875f };
	const char* txt = "";
	CreateButton(unit_interface, 4, lo, 0.1, 0.125, txt);
	
	lo = { -0.8f, -0.5625f };
	txt = "";
	CreateButton(unit_interface, 4, lo, 0.1, 0.125, txt);
	
	lo = { -0.9f, -0.525f };
	txt = "";
	CreateButton(unit_interface, 1, lo, 0.15, 0.15, txt, "Interface\\arrow11.png");

	lo = { -0.9f, -0.875f };
	txt = "";
	CreateButton(unit_interface, 1, lo, 0.15, 0.15, txt, "Interface\\arrow12.png");

	lo = { -0.9f, -0.7f };
	txt = "";
	CreateButton(unit_interface, 2, lo, 0.15, 0.1, txt, "Interface\\changePlane.png");

	lo = { -0.8f, 0.8f };//5
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\1box.png");
	lo = { -0.65f, 0.8f };
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\2box.png");
	lo = { -0.5f, 0.8f };
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\3box.png");
	lo = { -0.35f, 0.8f };
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\4box.png");
	lo = { -0.2f, 0.8f };
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\5box.png");
	lo = { -0.05f, 0.8f };
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\6box.png");
	lo = { 0.1f, 0.8f };//11
	txt = "";
	CreateButton(unit_interface, 5, lo, 0.15, 0.15, txt, "Interface\\7box.png");


	//texture interface
	lo = { -0.75f, 0.75f };
	txt = "";
	CreateButton(texture_interface, 1, lo, 0.25, 0.25, txt, "WrappingPaper\\1.jpg");

	lo = { -0.5f, 0.75f };
	txt = "";
	CreateButton(texture_interface, 1, lo, 0.25, 0.25, txt, "WrappingPaper\\2.jpg");

	lo = { -0.25f, 0.75f };
	txt = "";
	CreateButton(texture_interface, 1, lo, 0.25, 0.25, txt, "WrappingPaper\\3.jpg");

	lo = { 0.f, 0.75f };
	txt = "Delete";
	CreateButton(texture_interface, 2, lo, 0.25, 0.25, txt);

}


void UserData::Init_BoxSpecification() {
	box_ho.clear(); box_ho.resize(7);
	box_ho_ems.clear(); box_ho_ems.resize(7);

	box_ho.at(0).l1 = 0.09f;
	box_ho.at(0).l2 = 0.19f;
	box_ho.at(0).l3 = 0.22f; 

	box_ho.at(1).l1 = 0.15f;
	box_ho.at(1).l2 = 0.18f;
	box_ho.at(1).l3 = 0.27f; 

	box_ho.at(2).l1 = 0.21f;
	box_ho.at(2).l2 = 0.25f;
	box_ho.at(2).l3 = 0.34f; 

	box_ho.at(3).l1 = 0.28f;
	box_ho.at(3).l2 = 0.31f;
	box_ho.at(3).l3 = 0.41f; 

	box_ho.at(4).l1 = 0.34f;
	box_ho.at(4).l2 = 0.38f;
	box_ho.at(4).l3 = 0.48f; 

	box_ho.at(5).l1 = 0.40f;
	box_ho.at(5).l2 = 0.48f;
	box_ho.at(5).l3 = 0.52f; 

	box_ho.at(6).l1 = 0.60f;
	box_ho.at(6).l2 = 0.60f;
	box_ho.at(6).l3 = 0.60f;

//////////////////////////////////////////////

	box_ho_ems.at(0).l1 = 0.13f;
	box_ho_ems.at(0).l2 = 0.16f;
	box_ho_ems.at(0).l3 = 0.26f; 
		  
	box_ho_ems.at(1).l1 = 0.13f;
	box_ho_ems.at(1).l2 = 0.24f;
	box_ho_ems.at(1).l3 = 0.33f; 
		  
	box_ho_ems.at(2).l1 = 0.20f;
	box_ho_ems.at(2).l2 = 0.30f;
	box_ho_ems.at(2).l3 = 0.40f; 
		  
	box_ho_ems.at(3).l1 = 0.30f;
	box_ho_ems.at(3).l2 = 0.34f;
	box_ho_ems.at(3).l3 = 0.45f; 
		  
	box_ho_ems.at(4).l1 = 0.35f;
	box_ho_ems.at(4).l2 = 0.40f;
	box_ho_ems.at(4).l3 = 0.50f; 
		  
	box_ho_ems.at(5).l1 = 0.50f;
	box_ho_ems.at(5).l2 = 0.60f;
	box_ho_ems.at(5).l3 = 0.70f; 
		  
	box_ho_ems.at(6).l1 = 0.80f;
	box_ho_ems.at(6).l2 = 0.80f;
	box_ho_ems.at(6).l3 = 0.80f;
}

void UserData::print_explane() {
	//printf("%s\n%s\n%s\n%s\n",
	//	"Q : rotating",
	//	"W : padding",
	//	"E : zoom",
	//	"R : reset camera"
	//);
	//printf("%s\n%s\n%s\n%s\n%s\n",
	//	"click mouse right button to floor",
	//	"press 1 to change render",
	//	"press space bar to find a box",
	//	"H : box recommendation",
	//	"press 2 to edit mode"
	//);
	printf("%s\n",
		"click mouse right button to show interface"
	);
}

#include <stdlib.h>

void UserData::Update_RealSense() {
	depth_points.clear();
	depth_colors.clear();

	dev->wait_for_frames();

	// 1. fetch point clouds from the Intel RealSense device.
	const uint16_t* depth_image = (const uint16_t*)dev->get_frame_data(rs::stream::depth);
	const uint8_t* color_image = (uint8_t*)dev->get_frame_data(rs::stream::rectified_color);

	// 2. We have to filter out *dead* pixels that do not have depth values due to measurement errors,
	//	  such as obsorbing IR of black surfaces or too much far distant surfaces.
	int depth_width = depth_intrin.width;
	int depth_height = depth_intrin.height;
	for (int dy = 0; dy < depth_height; dy++) {
		for (int dx = 0; dx < depth_width; dx++) {
			uint16_t depth_value = depth_image[dy*depth_width + dx];
			float depth_in_meters = depth_value * scale;

			if (depth_value == 0) continue; // zero depth means it is one of the dead pixels.

			rs::float2 depth_pixel = { float(dx), float(dy) };
			rs::float3 depth_point = depth_intrin.deproject(depth_pixel, depth_in_meters);
			rs::float3 color_point = depth_to_color.transform(depth_point);
			rs::float2 color_pixel = color_intrin.project(color_point);

			ubyte3 depth_color = {};
			const int cx = int(std::round(color_pixel.x)), cy = int(std::round(color_pixel.y));
			if (cx >= 0 && cx < color_intrin.width && cy >= 0 && cy < color_intrin.height) {
				depth_color = *((ubyte3*)(color_image + 3 * (cy*color_intrin.width + cx)));
			}

			depth_points.push_back(reinterpret_cast<glm::vec3&>(depth_point));
			depth_colors.push_back(depth_color);
		}
	}
}

void UserData::Update_FindSurface() {
	cleanUpFindSurface(fs);
	setPointCloudFloat(fs, depth_points.data(), static_cast<unsigned int>(depth_points.size()), sizeof(depth_points[0]));
}


void UserData::Cleanup_OpenGL() {
	DestroyProgram(program_pointcloud);
	DestroyVertexArray(VAO_pointcloud);

	DestroyProgram(program_outlier);
	DestroyVertexArray(VAO_outlier);

	DestroyProgram(program_box);
	DestroyVertexArray(VAO_box);
	
	DestroyProgram(program_edit);
	DestroyVertexArray(VAO_edit);

	DestroyProgram(program_select);
	DestroyVertexArray(VAO_select);

	DestroyProgram(program_box_edge);
	DestroyVertexArray(VAO_box_edge);

	DestroyProgram(program_side);
	DestroyVertexArray(VAO_side);


	tr.Release();
}

void UserData::Cleanup_FindSurface() {
	cleanUpFindSurface(fs);
	releaseFindSurface(fs);
}

void UserData::Cleanup_RealSense() {
	dev->stop();
}

void UserData::Cleanup_Interface() {
	for (int i = 0; i < main_interface.size(); i++) {
		DestroyProgram(main_interface[i].program);
		DestroyVertexArray(main_interface[i].VAO);
	}
	for (int i = 0; i < edit_interface.size(); i++) {
		DestroyProgram(edit_interface[i].program);
		DestroyVertexArray(edit_interface[i].VAO);
	}
	for (int i = 0; i < camera_interface.size(); i++) {
		DestroyProgram(camera_interface[i].program);
		DestroyVertexArray(camera_interface[i].VAO);
	}

	for (int i = 0; i < unit_interface.size(); i++) {
		DestroyProgram(unit_interface[i].program);
		DestroyVertexArray(unit_interface[i].VAO);
	}
	for (int i = 0; i < texture_interface.size(); i++) {
		DestroyProgram(texture_interface[i].program);
		DestroyVertexArray(texture_interface[i].VAO);
	}

	glfwDestroyCursor(std_cursor);
	glfwDestroyCursor(zoom_cursor);
	glfwDestroyCursor(rotate_cursor);
	glfwDestroyCursor(panning_cursor);
	glfwDestroyCursor(plane_cursor);
}

void UserData::Run_FindSurface(unsigned int index, float depth) {
	// point clouds tends to have measurement errors propositional to distance.
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_ACCURACY, MAX(0.008f + 0.002f*(depth - 1.f), 0.005f));
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_TOUCH_R, 1.f);
	setFindSurfaceParamFloat(fs, FS_PARAM_MEAN_DIST, 0.1f);
	setFindSurfaceParamInt(fs, FS_PARAMS::FS_PARAM_RAD_EXP, 10);
	setFindSurfaceParamInt(fs, FS_PARAMS::FS_PARAM_LAT_EXT, 10);
	
	
	// if succeeds, the struct "result" will be filled with data.
	int res = findSurface(fs, type, index, &result);

	switch (res) {
	case FS_NO_ERROR: printf("FindSurface: find! \n"); break;
	case FS_NOT_FOUND:
	case FS_UNACCEPTABLE_RESULT: fprintf(stderr, "FindSurface: failed to find (%d).\n", res); return;
	case FS_INVALID_OPERATION: fprintf(stderr, "FindSurface: INVALID_OPERATION error.\n"); return;
	case FS_INVALID_ENUM: fprintf(stderr, "FindSurface: INVALID_ENUM error.\n"); return;
	case FS_LICENSE_EXPIRED: fprintf(stderr, "FindSurface: license error occurred (FS_LICENSE_EXPIRED).\n"); return;
	case FS_LICENSE_UNKNOWN: fprintf(stderr, "FindSurface: license error occurred (FS_LICENSE_UNKNOWN).\n"); return;
	}

	const unsigned char* flags = getInOutlierFlags(fs);
	int count_out = getOutliersFloat(fs, nullptr, 0); // retrieve the number of outlier points;
	int count_in = getInliersFloat(fs, nullptr, 0); // retrieve the number of inlier points;
	float sumOfz = 0;

	outlier_points.clear(); outlier_points.reserve(count_out);
	outlier_colors.clear(); outlier_colors.reserve(count_out);
	
	inlier_points.clear(); inlier_points.reserve(count_in);
	inlier_colors.clear(); inlier_colors.reserve(count_in);

	for (int k = 0; k<int(getPointCloudCount(fs)); k++) {
		if (flags[k]) {
			outlier_points.push_back(reinterpret_cast<glm::vec3&>(depth_points[k]));
			outlier_colors.push_back(depth_colors[k]);
		}
		else {
			inlier_points.push_back(reinterpret_cast<glm::vec3&>(depth_points[k]));
			inlier_colors.push_back(depth_colors[k]);
			sumOfz += depth_points[k].z;
		}
	}

	avgz = sumOfz / count_in;
	//float error = MAX(0.008f + 0.002f*(depth - 1.f), 0.005f);
	
	//float error_param = depth;
	//1m일때는 5
	//최소( 2
	//

	//float error = result.rms * 4.0f;
	//printf("rms : %f\n", result.rms);
	
	printf("%f\n", depth);
	float error = 0;
	if (depth > 1) {
		error = 0.03f;
	}
	else {
		error = result.rms * 3.0f;
	}

	Cutblob(error);

	VertexBufferData(VAO_outlier, VBO_POS, foutlier_points.size(), sizeof(glm::vec3), foutlier_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_outlier, VBO_COLOR, foutlier_colors.size(), sizeof(ubyte3), foutlier_colors.data(), GL_STATIC_DRAW);

	//VertexBufferData(VAO_pointcloud, VBO_POS, outlier_points.size(), sizeof(glm::vec3), outlier_points.data(), GL_STATIC_DRAW);
	//VertexBufferData(VAO_pointcloud, VBO_COLOR, outlier_colors.size(), sizeof(ubyte3), outlier_colors.data(), GL_STATIC_DRAW);

	float avgheight = 0;
	for (int i = 0; i < foutlier_points.size(); i++) {
		avgheight += avgz - foutlier_points[i].z;
	}
	avgheight /= foutlier_points.size();
	
	//표준편차를 이용해서 박스높이 에러를 수정
	float variance = 0; //분산
	float min_differ = fabs(avgheight - (avgz - foutlier_points.begin()->z));
	float differ;
	avg_index = 0;
	for (int i = 0; i < foutlier_points.size(); i++) {
		variance += pow(avgz - foutlier_points[i].z - avgheight, 2);
		differ = fabs(avgheight - (avgz - foutlier_points[i].z));
		if (min_differ > differ) {
			min_differ = differ;
			avg_index = i;
		}
	}
	variance = variance / foutlier_points.size();
	float std_deviation = sqrt(variance); //표준편차

	box_height = avgheight + std_deviation * 2.2f;
	
	printf("variance = %f, avgheight = %f, std_d2eviation = %f\n", variance, avgheight, std_deviation);


	/*
	int count = getInliersFloat(fs, nullptr, 0); // retrieve the number of inlier points;
	
	for (int k = 0; k<int(getPointCloudCount(fs)); k++) {
		if (!flags[k]) {
			inlier_points.push_back(reinterpret_cast<glm::vec3&>(depth_points[k]));
			inlier_colors.push_back(depth_colors[k]);
		}
	}
	VertexBufferData(VAO_pointcloud, VBO_POS, inlier_points.size(), sizeof(glm::vec3), inlier_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_pointcloud, VBO_COLOR, inlier_colors.size(), sizeof(ubyte3), inlier_colors.data(), GL_STATIC_DRAW);
	*/
}

void UserData::Cutblob(float error) {
	size_t outlierSize = outlier_points.size();
	foutlier_points.clear();
	foutlier_colors.clear();
	foutlier_points.reserve(outlierSize);
	foutlier_colors.reserve(outlierSize);

	for (int i = 0; i < outlierSize; i++) {
		if (outlier_points[i].z < avgz - error) {
			foutlier_points.push_back(outlier_points[i]);
			foutlier_colors.push_back(outlier_colors[i]);
		}
	}
}

int UserData::Find_Floor() {
	/////표준편차를 이용해 카메라에서 가장 먼 거리에 있는 점 하나를 바닥의 인덱스로 리턴
	float avgheight = 0;
	for (int i = 0; i < depth_points.size(); i++) {
		avgheight += depth_points[i].z;
	}
	avgheight /= depth_points.size();

	float variance = 0; //분산

	for (int i = 0; i < depth_points.size(); i++) {
		variance += pow(depth_points[i].z - avgheight, 2);
	}
	variance = variance / depth_points.size();
	float std_deviation = sqrt(variance); //표준편차
	
	float floorz = avgheight + std_deviation * 5.f;
	

	int idx = 0;
	float min = fabs(depth_points[0].z - floorz);
	for (int i = 1; i < depth_points.size(); i++) {
		if (min > fabs(depth_points[i].z - floorz)) {
			idx = i;
		}
	}
	return idx;
}

void UserData::Run_FindBox() {
	box_points.clear();
	box_points.reserve(36);
	box_points_org.clear();
	box_points_org.resize(36);


	//강제적으로 노이즈를 부여
	//outlier값들을 같은 평면으로 누를 때, 한치의 오차도 없이 값을 전달하게 되면 findsurface가 평면을 잘 찾지 못한다.
	for (int i = 0; i < foutlier_points.size(); i++) {
		foutlier_points[i].z = avgz + glm::sin(i)*0.001;
	}


	cleanUpFindSurface(fs);
	setPointCloudFloat(fs, foutlier_points.data(), static_cast<unsigned int>(foutlier_points.size()), sizeof(foutlier_points[0]));
	// point clouds tends to have measurement errors propositional to distance.
	setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_ACCURACY, MAX(0.008f + 0.002f*(avgz - 1.f), 0.005f));
	setFindSurfaceParamFloat(fs, FS_PARAM_TOUCH_R, 0.5f);
	setFindSurfaceParamFloat(fs, FS_PARAM_MEAN_DIST, 0.5f);
	//setFindSurfaceParamFloat(fs, FS_PARAMS::FS_PARAM_ACCURACY, 0.03f);
	setFindSurfaceParamInt(fs, FS_PARAMS::FS_PARAM_RAD_EXP, 10);
	setFindSurfaceParamInt(fs, FS_PARAMS::FS_PARAM_LAT_EXT, 10);

	// if succeeds, the struct "result" will be filled with data.
	
	int res = findSurface(fs, type, avg_index, &result);
	switch (res) {
	case FS_NO_ERROR: printf("FindSurface: find! \n"); break;
	case FS_NOT_FOUND:
	case FS_UNACCEPTABLE_RESULT: fprintf(stderr, "FindSurface: failed to find (%d).\n", res); return;
	case FS_INVALID_OPERATION: fprintf(stderr, "FindSurface: INVALID_OPERATION error.\n"); return;
	case FS_INVALID_ENUM: fprintf(stderr, "FindSurface: INVALID_ENUM error.\n"); return;
	case FS_LICENSE_EXPIRED: fprintf(stderr, "FindSurface: license error occurred (FS_LICENSE_EXPIRED).\n"); return;
	case FS_LICENSE_UNKNOWN: fprintf(stderr, "FindSurface: license error occurred (FS_LICENSE_UNKNOWN).\n"); return;
	}

	Findbox = true;
	edit_enable = true;

	glm::vec3 bll = reinterpret_cast<glm::vec3&>(result.plane_param.ll);
	glm::vec3 blr = reinterpret_cast<glm::vec3&>(result.plane_param.lr);
	glm::vec3 bul = reinterpret_cast<glm::vec3&>(result.plane_param.ul);
	glm::vec3 bur = reinterpret_cast<glm::vec3&>(result.plane_param.ur);

	glm::vec3 v = glm::normalize(bul - bll);
	glm::vec3 h = glm::normalize(blr - bll);

	if (glm::cross(h, v).z > 0) {
		std::swap(bll, blr);
		std::swap(bul, bur);
	}
	
	glm::vec3 tll = bll;
	glm::vec3 tlr = blr;
	glm::vec3 tul = bul;
	glm::vec3 tur = bur;

	tll.z -= box_height;
	tlr.z -= box_height;
	tul.z -= box_height;
	tur.z -= box_height;

	//옆면 4개
	box_points.push_back(tll); box_points.push_back(bll); box_points.push_back(blr);
	box_points.push_back(blr); box_points.push_back(tlr); box_points.push_back(tll);

	box_points.push_back(tlr); box_points.push_back(blr); box_points.push_back(bur);
	box_points.push_back(bur); box_points.push_back(tur); box_points.push_back(tlr);

	box_points.push_back(tur); box_points.push_back(bur); box_points.push_back(bul);
	box_points.push_back(bul); box_points.push_back(tul); box_points.push_back(tur);

	box_points.push_back(tul); box_points.push_back(bul); box_points.push_back(bll);
	box_points.push_back(bll); box_points.push_back(tll); box_points.push_back(tul);

	//밑면
	box_points.push_back(bll); box_points.push_back(bul); box_points.push_back(bur);
	box_points.push_back(bur); box_points.push_back(blr); box_points.push_back(bll);

	//윗면
	box_points.push_back(tul); box_points.push_back(tll); box_points.push_back(tlr);
	box_points.push_back(tlr); box_points.push_back(tur); box_points.push_back(tul);


	////////박스 좌표 초기값 저장
	box_points_org = box_points;

	VertexBufferData(VAO_box, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_edit, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);


	std::vector<glm::vec3> edge_pos;
	edge_pos.clear(); edge_pos.reserve(8);
	edge_pos.push_back(bll); edge_pos.push_back(blr); edge_pos.push_back(bul); edge_pos.push_back(bur); 
	edge_pos.push_back(tll); edge_pos.push_back(tlr); edge_pos.push_back(tul); edge_pos.push_back(tur);
	
	VertexBufferData(VAO_box_edge, VBO_POS, edge_pos.size(), sizeof(glm::vec3), edge_pos.data(), GL_STATIC_DRAW);
	
	//Set_arrows();


	//calculate normal vector and bind buffer to VAO_edit normal attr
	std::vector<glm::vec3> box_norm;
	box_norm.clear(); box_norm.reserve(36);
	
	for (int i = 0; i < 36; i += 6){
		if (i < 24) {
			glm::vec3 v1, v2;
			v1 = glm::normalize(box_points[i] - box_points[i + 1]);
			v2 = glm::normalize(box_points[i + 4] - box_points[i]);
			glm::vec3 nv = glm::normalize(glm::cross(v1, v2));
			for (int j = 0; j < 6; j++) {
				box_norm.push_back(nv);
			}
		}
		else {
			glm::vec3 v1, v2;
			v1 = glm::normalize(box_points[i + 1] - box_points[i]);
			v2 = glm::normalize(box_points[i + 4] - box_points[i]);
			glm::vec3 nv = glm::normalize(glm::cross(v1, v2));
			for (int j = 0; j < 6; j++) {
				box_norm.push_back(nv);
			}
		}
	}
	VertexBufferData(VAO_edit, VBO_NORM, box_norm.size(), sizeof(glm::vec3), box_norm.data(), GL_STATIC_DRAW);

	//printf("ll = %f %f %f, lr = %f %f %f, ul = %f %f %f, ur = %f %f %f\n", result.plane_param.ll[0], result.plane_param.ll[1], result.plane_param.ll[2],
	//	result.plane_param.lr[0], result.plane_param.lr[1], result.plane_param.lr[2],
	//	result.plane_param.ul[0], result.plane_param.ul[1], result.plane_param.ul[2],
	//	result.plane_param.ur[0], result.plane_param.ur[1], result.plane_param.ur[2]
	//);


	//box_vert = glm::distance(reinterpret_cast<glm::vec3&>(result.plane_param.ll), reinterpret_cast<glm::vec3&>(result.plane_param.ul));
	//box_hori = glm::distance(reinterpret_cast<glm::vec3&>(result.plane_param.ll), reinterpret_cast<glm::vec3&>(result.plane_param.lr));

	box_vert = glm::distance(box_points[1], box_points[2]);
	box_hori = glm::distance(box_points[2], box_points[8]);
	box_height = glm::distance(box_points[1], box_points[0]);

	printf("Find Box!\n");
	//printf("Box vertical = %f, Box horizontal = %f, Box height = %f\n", box_vert, box_hori, box_height);

	s1 = "width : " + std::to_string((int)(box_vert*100)) + "cm";
	camera_interface[7].text = s1.c_str();
	edit_interface[8].text = s1.c_str();
	
	s2 = "length : " + std::to_string((int)(box_hori*100)) + "cm";
	camera_interface[8].text = s2.c_str();
	edit_interface[9].text = s2.c_str();

	s3 = "height : " + std::to_string((int)(box_height*100)) + "cm";
	camera_interface[9].text = s3.c_str();
	edit_interface[10].text = s3.c_str();

	rbox = Recommend_Box();
	for (int i = 5; i < 12; i++) {
		if (rbox + 5 == i)
			unit_interface[i].id = 2;
		else
			unit_interface[i].id = 0;
	}
	if (rbox == -1)
		unit_interface[11].id = 2;
}

void UserData::Run_edit() {

}

void UserData::Run_selectPaper(int index) {
	printf("Run_selectPaper index = %d\n", index);
	isSelectWrapper = true;
	WrapperIdx = index-1;
}


int UserData::Recommend_Box() {
	box_specification box_found;

	float temp[3] = { 0, };
	temp[0] = box_height;
	temp[1] = box_vert;
	temp[2] = box_hori;

	std::sort(temp, temp + 3);
	box_found.l1 = temp[0];
	box_found.l2 = temp[1];
	box_found.l3 = temp[2];

	for (int i = 0; i < 6; i++) {
		if (box_found.l1 < box_ho.at(i).l1 && box_found.l2 < box_ho.at(i).l2 && box_found.l3 < box_ho.at(i).l3) {
			//printf("추천 국내박스 규격 : %d호\n", i + 1);
			return i;
			break;
		}
		else if (i >= 5) {
			return -1;
			//printf("추천 국내박스 규격이 없습니다\n");
		}
	}
	for (int i = 0; i < 7; i++) {
		if (box_found.l1 < box_ho_ems.at(i).l1 && box_found.l2 < box_ho_ems.at(i).l2 && box_found.l3 < box_ho_ems.at(i).l3) {
			printf("추천 국제박스(EMS) 규격 : %d호\n", i + 1);
			break;
		}
		else if (i >= 6) {
			printf("추천 국제박스 규격이 없습니다\n");
		}
	}
}

void UserData::Size_up() {
	if (side_count == 0) {

		float x1 = box_points.at(0).x;
		float x2 = box_points.at(2).x;
		float x3 = box_points.at(18).x;

		if (box_points.at(0).x != box_points.at(18).x) {
			float giulgi = (box_points.at(0).y - box_points.at(18).y) / (box_points.at(0).x - box_points.at(18).x);
			float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
			float dy = giulgi * dx;

			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += dx;
						box_points.at(i).y += dy;
					}
					else {
						box_points.at(i).x -= dx;
						box_points.at(i).y -= dy;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += unit_cm;
					}
					else {
						box_points.at(i).x -= unit_cm;
					}
				}
			}
		}
	}
	else if (side_count == 6) {

		float x1 = box_points.at(6).x;
		float x2 = box_points.at(8).x;
		float x3 = box_points.at(0).x;

		if (box_points.at(6).x != box_points.at(0).x) {
			float giulgi = (box_points.at(6).y - box_points.at(0).y) / (box_points.at(6).x - box_points.at(0).x);
			float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
			float dy = giulgi * dx;

			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += dx;
						box_points.at(i).y += dy;
					}
					else {
						box_points.at(i).x -= dx;
						box_points.at(i).y -= dy;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += unit_cm;
					}
					else {
						box_points.at(i).x -= unit_cm;
					}
				}
			}
		}
	}
	else if (side_count == 12) {

		float x1 = box_points.at(12).x;
		float x2 = box_points.at(14).x;
		float x3 = box_points.at(6).x;

		if (box_points.at(12).x != box_points.at(6).x) {
			float giulgi = (box_points.at(12).y - box_points.at(6).y) / (box_points.at(12).x - box_points.at(6).x);
			float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
			float dy = giulgi * dx;

			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += dx;
						box_points.at(i).y += dy;
					}
					else {
						box_points.at(i).x -= dx;
						box_points.at(i).y -= dy;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += unit_cm;
					}
					else {
						box_points.at(i).x -= unit_cm;
					}
				}
			}
		}
	}
	else if (side_count == 18) {

		float x1 = box_points.at(18).x;
		float x2 = box_points.at(20).x;
		float x3 = box_points.at(12).x;

		if (box_points.at(18).x != box_points.at(12).x) {
			float giulgi = (box_points.at(18).y - box_points.at(12).y) / (box_points.at(18).x - box_points.at(12).x);
			float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
			float dy = giulgi * dx;

			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += dx;
						box_points.at(i).y += dy;
					}
					else {
						box_points.at(i).x -= dx;
						box_points.at(i).y -= dy;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 36; i++) {
				if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
					if (x1 > x3) {
						box_points.at(i).x += unit_cm;
					}
					else {
						box_points.at(i).x -= unit_cm;
					}
				}
			}
		}
	}
	else if (side_count == 24) {
		for (int i = 24; i < 30; i++) {
			box_points.at(i).z += unit_cm;
		}
		for (int i = 0; i != 24 && i != 30 && i < 36; i += 6) {
			box_points.at(i + 1).z += unit_cm;
			box_points.at(i + 2).z += unit_cm;
			box_points.at(i + 3).z += unit_cm;
		}
	}
	else if (side_count == 30) {
		for (int i = 30; i < 36; i++) {
			box_points.at(i).z -= unit_cm;
		}
		for (int i = 0; i != 24 && i != 30 && i < 36; i += 6) {
			box_points.at(i).z -= unit_cm;
			box_points.at(i + 4).z -= unit_cm;
			box_points.at(i + 5).z -= unit_cm;
		}
	}

	/*DestroyProgram(program_edit2);
	DestroyVertexArray(VAO_edit2);

	std::map<GLenum, const char*> edit2_shader_map = {
	{ GL_VERTEX_SHADER, ShaderSource::GetSource("box_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("box_fs") }
	};
	program_edit2 = CreateProgram(edit2_shader_map);

	std::vector<VBO_info> VBO_info_edit2 = {
	{ VBO_POS, 0, nullptr, 0, sizeof(glm::vec3) }
	};
	std::vector<attrib_info> attrib_info_edit2 = {
	{ 0, GL_FLOAT, 3, 0 }
	};

	VAO_edit2 = CreateVertexArray(VBO_info_edit2, attrib_info_edit2);*/
	VertexBufferData(VAO_box, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_edit, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);


	std::vector<glm::vec3> box_edge_pos;
	box_edge_pos.clear(); box_edge_pos.reserve(8);
	box_edge_pos.push_back(box_points[1]);
	box_edge_pos.push_back(box_points[2]);
	box_edge_pos.push_back(box_points[14]);
	box_edge_pos.push_back(box_points[8]);
	box_edge_pos.push_back(box_points[0]);
	box_edge_pos.push_back(box_points[4]);
	box_edge_pos.push_back(box_points[16]);
	box_edge_pos.push_back(box_points[10]);
	VertexBufferData(VAO_box_edge, VBO_POS, box_edge_pos.size(), sizeof(glm::vec3), box_edge_pos.data(), GL_STATIC_DRAW);

	//VertexBufferData(VAO_edit2, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	//VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);

	//Set_arrows();
}


void UserData::Size_down() {
	if (side_count == 0) {

		float x1 = box_points.at(0).x;
		float x2 = box_points.at(2).x;
		float x3 = box_points.at(18).x;

		float dx_l = box_points.at(0).x - box_points.at(18).x;
		float dy_l = box_points.at(0).y - box_points.at(18).y;
		float length = sqrt(dx_l * dx_l + dy_l * dy_l);

		if (length > 2 * unit_cm) {
			if (box_points.at(0).x != box_points.at(18).x) {
				float giulgi = (box_points.at(0).y - box_points.at(18).y) / (box_points.at(0).x - box_points.at(18).x);
				float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
				float dy = giulgi * dx;

				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= dx;
							box_points.at(i).y -= dy;
						}
						else {
							box_points.at(i).x += dx;
							box_points.at(i).y += dy;
						}
					}
				}
			}
			else {
				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= unit_cm;
						}
						else {
							box_points.at(i).x += unit_cm;
						}
					}
				}
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}
	else if (side_count == 6) {

		float x1 = box_points.at(6).x;
		float x2 = box_points.at(8).x;
		float x3 = box_points.at(0).x;

		float dx_l = box_points.at(6).x - box_points.at(0).x;
		float dy_l = box_points.at(6).y - box_points.at(0).y;
		float length = sqrt(dx_l * dx_l + dy_l * dy_l);

		if (length > 2 * unit_cm) {
			if (box_points.at(6).x != box_points.at(0).x) {
				float giulgi = (box_points.at(6).y - box_points.at(0).y) / (box_points.at(6).x - box_points.at(0).x);
				float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
				float dy = giulgi * dx;

				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= dx;
							box_points.at(i).y -= dy;
						}
						else {
							box_points.at(i).x += dx;
							box_points.at(i).y += dy;
						}
					}
				}
			}
			else {
				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= unit_cm;
						}
						else {
							box_points.at(i).x += unit_cm;
						}
					}
				}
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}
	else if (side_count == 12) {

		float x1 = box_points.at(12).x;
		float x2 = box_points.at(14).x;
		float x3 = box_points.at(6).x;

		float dx_l = box_points.at(12).x - box_points.at(6).x;
		float dy_l = box_points.at(12).y - box_points.at(6).y;
		float length = sqrt(dx_l * dx_l + dy_l * dy_l);

		if (length > 2 * unit_cm) {
			if (box_points.at(12).x != box_points.at(6).x) {
				float giulgi = (box_points.at(12).y - box_points.at(6).y) / (box_points.at(12).x - box_points.at(6).x);
				float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
				float dy = giulgi * dx;

				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= dx;
							box_points.at(i).y -= dy;
						}
						else {
							box_points.at(i).x += dx;
							box_points.at(i).y += dy;
						}
					}
				}
			}
			else {
				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= unit_cm;
						}
						else {
							box_points.at(i).x += unit_cm;
						}
					}
				}
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}
	else if (side_count == 18) {

		float x1 = box_points.at(18).x;
		float x2 = box_points.at(20).x;
		float x3 = box_points.at(12).x;

		float dx_l = box_points.at(18).x - box_points.at(12).x;
		float dy_l = box_points.at(18).y - box_points.at(12).y;
		float length = sqrt(dx_l * dx_l + dy_l * dy_l);

		if (length > 2 * unit_cm) {
			if (box_points.at(18).x != box_points.at(12).x) {
				float giulgi = (box_points.at(18).y - box_points.at(12).y) / (box_points.at(18).x - box_points.at(12).x);
				float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * (1 / unit_cm));
				float dy = giulgi * dx;

				for (int i = 0; i < 36; i++) {
					if (box_points.at(i).x == x1 || box_points.at(i).x == x2) {
						if (x1 > x3) {
							box_points.at(i).x -= dx;
							box_points.at(i).y -= dy;
						}
						else {
							box_points.at(i).x += dx;
							box_points.at(i).y += dy;
						}
					}
				}
			}
			else {
				for (int i = 0; i < 36; i++) {
					if ((box_points.at(i).x == x1 || box_points.at(i).x == x2) && length > 0.05) {
						if (x1 > x3) {
							box_points.at(i).x -= unit_cm;
						}
						else {
							box_points.at(i).x += unit_cm;
						}
					}
				}
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}
	else if (side_count == 24) {
		if (box_points.at(1).z - box_points.at(0).z > 2 * unit_cm) {
			for (int i = 24; i < 30; i++) {
				box_points.at(i).z -= unit_cm;
			}
			for (int i = 0; i != 24 && i != 30 && i < 36; i += 6) {
				box_points.at(i + 1).z -= unit_cm;
				box_points.at(i + 2).z -= unit_cm;
				box_points.at(i + 3).z -= unit_cm;
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}
	else if (side_count == 30) {
		if (box_points.at(1).z - box_points.at(0).z > 2 * unit_cm) {
			for (int i = 30; i < 36; i++) {
				box_points.at(i).z += unit_cm;
			}
			for (int i = 0; i != 24 && i != 30 && i < 36; i += 6) {
				box_points.at(i).z += unit_cm;
				box_points.at(i + 4).z += unit_cm;
				box_points.at(i + 5).z += unit_cm;
			}
		}
		else {
			printf("더 이상 원하는 방향으로 상자를 줄일 수 없습니다.\n");
		}
	}

	VertexBufferData(VAO_box, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_edit, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);

	
	std::vector<glm::vec3> box_edge_pos;
	box_edge_pos.clear(); box_edge_pos.reserve(8);
	box_edge_pos.push_back(box_points[1]);
	box_edge_pos.push_back(box_points[2]);
	box_edge_pos.push_back(box_points[14]);
	box_edge_pos.push_back(box_points[8]);
	box_edge_pos.push_back(box_points[0]);
	box_edge_pos.push_back(box_points[4]);
	box_edge_pos.push_back(box_points[16]);
	box_edge_pos.push_back(box_points[10]);
	VertexBufferData(VAO_box_edge, VBO_POS, box_edge_pos.size(), sizeof(glm::vec3), box_edge_pos.data(), GL_STATIC_DRAW);

	//VertexBufferData(VAO_edit2, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
	//VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);


	//화살표 위치 조정
	//Set_arrows();
}


void UserData::Set_arrows() {
	arrow_points.clear();
	arrow_points.reserve(108);

	arrow_line_points.clear();
	arrow_line_points.reserve(72);

	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 v4;
	glm::vec3 v5;

	//옆면1

	float x1 = box_points.at(0).x;
	float x2 = box_points.at(2).x;
	float x3 = box_points.at(18).x;

	v1.x = box_points.at(0).x * 2 / 9 + box_points.at(1).x * 5 / 9 + box_points.at(2).x * 2 / 9;
	v1.y = box_points.at(0).y * 2 / 9 + box_points.at(1).y * 5 / 9 + box_points.at(2).y * 2 / 9;
	v1.z = box_points.at(0).z * 2 / 9 + box_points.at(1).z * 5 / 9 + box_points.at(2).z * 2 / 9;

	v2.x = box_points.at(1).x * 2 / 9 + box_points.at(2).x * 5 / 9 + box_points.at(4).x * 2 / 9;
	v2.y = box_points.at(1).y * 2 / 9 + box_points.at(2).y * 5 / 9 + box_points.at(4).y * 2 / 9;
	v2.z = box_points.at(1).z * 2 / 9 + box_points.at(2).z * 5 / 9 + box_points.at(4).z * 2 / 9;

	v3.x = box_points.at(2).x * 2 / 9 + box_points.at(4).x * 5 / 9 + box_points.at(5).x * 2 / 9;
	v3.y = box_points.at(2).y * 2 / 9 + box_points.at(4).y * 5 / 9 + box_points.at(5).y * 2 / 9;
	v3.z = box_points.at(2).z * 2 / 9 + box_points.at(4).z * 5 / 9 + box_points.at(5).z * 2 / 9;

	v4.x = box_points.at(4).x * 2 / 9 + box_points.at(0).x * 5 / 9 + box_points.at(1).x * 2 / 9;
	v4.y = box_points.at(4).y * 2 / 9 + box_points.at(0).y * 5 / 9 + box_points.at(1).y * 2 / 9;
	v4.z = box_points.at(4).z * 2 / 9 + box_points.at(0).z * 5 / 9 + box_points.at(1).z * 2 / 9;


	if (box_points.at(0).x != box_points.at(18).x) {
		float giulgi = (box_points.at(0).y - box_points.at(18).y) / (box_points.at(0).x - box_points.at(18).x);
		float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * 100) * 9;
		float dy = giulgi * dx;

		if (x1 > x3) {
			v1.x += dx;
			v1.y += dy;
			v2.x += dx;
			v2.y += dy;
			v3.x += dx;
			v3.y += dy;
			v4.x += dx;
			v4.y += dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += dx;
			v5.y += dy;
		}
		else {
			v1.x -= dx;
			v1.y -= dy;
			v2.x -= dx;
			v2.y -= dy;
			v3.x -= dx;
			v3.y -= dy;
			v4.x -= dx;
			v4.y -= dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= dx;
			v5.y -= dy;
		}
	}
	else {
		if (x1 > x3) {
			v1.x += (float)0.09;
			v2.x += (float)0.09;
			v3.x += (float)0.09;
			v4.x += (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += (float)0.09;
		}
		else {
			v1.x -= (float)0.09;
			v2.x -= (float)0.09;
			v3.x -= (float)0.09;
			v4.x -= (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= (float)0.09;
		}
	}

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	//옆면2

	x1 = box_points.at(6).x;
	x2 = box_points.at(8).x;
	x3 = box_points.at(0).x;

	v1.x = box_points.at(6).x * 2 / 9 + box_points.at(7).x * 5 / 9 + box_points.at(8).x * 2 / 9;
	v1.y = box_points.at(6).y * 2 / 9 + box_points.at(7).y * 5 / 9 + box_points.at(8).y * 2 / 9;
	v1.z = box_points.at(6).z * 2 / 9 + box_points.at(7).z * 5 / 9 + box_points.at(8).z * 2 / 9;

	v2.x = box_points.at(7).x * 2 / 9 + box_points.at(8).x * 5 / 9 + box_points.at(10).x * 2 / 9;
	v2.y = box_points.at(7).y * 2 / 9 + box_points.at(8).y * 5 / 9 + box_points.at(10).y * 2 / 9;
	v2.z = box_points.at(7).z * 2 / 9 + box_points.at(8).z * 5 / 9 + box_points.at(10).z * 2 / 9;

	v3.x = box_points.at(9).x * 2 / 9 + box_points.at(10).x * 5 / 9 + box_points.at(11).x * 2 / 9;
	v3.y = box_points.at(9).y * 2 / 9 + box_points.at(10).y * 5 / 9 + box_points.at(11).y * 2 / 9;
	v3.z = box_points.at(9).z * 2 / 9 + box_points.at(10).z * 5 / 9 + box_points.at(11).z * 2 / 9;

	v4.x = box_points.at(10).x * 2 / 9 + box_points.at(6).x * 5 / 9 + box_points.at(7).x * 2 / 9;
	v4.y = box_points.at(10).y * 2 / 9 + box_points.at(6).y * 5 / 9 + box_points.at(7).y * 2 / 9;
	v4.z = box_points.at(10).z * 2 / 9 + box_points.at(6).z * 5 / 9 + box_points.at(7).z * 2 / 9;


	if (box_points.at(6).x != box_points.at(0).x) {
		float giulgi = (box_points.at(6).y - box_points.at(0).y) / (box_points.at(6).x - box_points.at(0).x);
		float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * 100) * 9;
		float dy = giulgi * dx;

		if (x1 > x3) {
			v1.x += dx;
			v1.y += dy;
			v2.x += dx;
			v2.y += dy;
			v3.x += dx;
			v3.y += dy;
			v4.x += dx;
			v4.y += dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += dx;
			v5.y += dy;
		}
		else {
			v1.x -= dx;
			v1.y -= dy;
			v2.x -= dx;
			v2.y -= dy;
			v3.x -= dx;
			v3.y -= dy;
			v4.x -= dx;
			v4.y -= dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= dx;
			v5.y -= dy;
		}
	}
	else {
		if (x1 > x3) {
			v1.x += (float)0.09;
			v2.x += (float)0.09;
			v3.x += (float)0.09;
			v4.x += (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += (float)0.09;
		}
		else {
			v1.x -= (float)0.09;
			v2.x -= (float)0.09;
			v3.x -= (float)0.09;
			v4.x -= (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= (float)0.09;
		}
	}

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	//옆면3

	x1 = box_points.at(12).x;
	x2 = box_points.at(14).x;
	x3 = box_points.at(6).x;

	v1.x = box_points.at(12).x * 2 / 9 + box_points.at(13).x * 5 / 9 + box_points.at(14).x * 2 / 9;
	v1.y = box_points.at(12).y * 2 / 9 + box_points.at(13).y * 5 / 9 + box_points.at(14).y * 2 / 9;
	v1.z = box_points.at(12).z * 2 / 9 + box_points.at(13).z * 5 / 9 + box_points.at(14).z * 2 / 9;

	v2.x = box_points.at(13).x * 2 / 9 + box_points.at(14).x * 5 / 9 + box_points.at(16).x * 2 / 9;
	v2.y = box_points.at(13).y * 2 / 9 + box_points.at(14).y * 5 / 9 + box_points.at(16).y * 2 / 9;
	v2.z = box_points.at(13).z * 2 / 9 + box_points.at(14).z * 5 / 9 + box_points.at(16).z * 2 / 9;

	v3.x = box_points.at(14).x * 2 / 9 + box_points.at(16).x * 5 / 9 + box_points.at(17).x * 2 / 9;
	v3.y = box_points.at(14).y * 2 / 9 + box_points.at(16).y * 5 / 9 + box_points.at(17).y * 2 / 9;
	v3.z = box_points.at(14).z * 2 / 9 + box_points.at(16).z * 5 / 9 + box_points.at(17).z * 2 / 9;

	v4.x = box_points.at(16).x * 2 / 9 + box_points.at(12).x * 5 / 9 + box_points.at(13).x * 2 / 9;
	v4.y = box_points.at(16).y * 2 / 9 + box_points.at(12).y * 5 / 9 + box_points.at(13).y * 2 / 9;
	v4.z = box_points.at(16).z * 2 / 9 + box_points.at(12).z * 5 / 9 + box_points.at(13).z * 2 / 9;


	if (box_points.at(12).x != box_points.at(6).x) {
		float giulgi = (box_points.at(12).y - box_points.at(6).y) / (box_points.at(12).x - box_points.at(6).x);
		float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * 100) * 9;
		float dy = giulgi * dx;

		if (x1 > x3) {
			v1.x += dx;
			v1.y += dy;
			v2.x += dx;
			v2.y += dy;
			v3.x += dx;
			v3.y += dy;
			v4.x += dx;
			v4.y += dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += dx;
			v5.y += dy;
		}
		else {
			v1.x -= dx;
			v1.y -= dy;
			v2.x -= dx;
			v2.y -= dy;
			v3.x -= dx;
			v3.y -= dy;
			v4.x -= dx;
			v4.y -= dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= dx;
			v5.y -= dy;
		}
	}
	else {
		if (x1 > x3) {
			v1.x += (float)0.09;
			v2.x += (float)0.09;
			v3.x += (float)0.09;
			v4.x += (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += (float)0.09;
		}
		else {
			v1.x -= (float)0.09;
			v2.x -= (float)0.09;
			v3.x -= (float)0.09;
			v4.x -= (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= (float)0.09;
		}
	}

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	//옆면4

	x1 = box_points.at(18).x;
	x2 = box_points.at(20).x;
	x3 = box_points.at(12).x;

	v1.x = box_points.at(18).x * 2 / 9 + box_points.at(19).x * 5 / 9 + box_points.at(20).x * 2 / 9;
	v1.y = box_points.at(18).y * 2 / 9 + box_points.at(19).y * 5 / 9 + box_points.at(20).y * 2 / 9;
	v1.z = box_points.at(18).z * 2 / 9 + box_points.at(19).z * 5 / 9 + box_points.at(20).z * 2 / 9;

	v2.x = box_points.at(19).x * 2 / 9 + box_points.at(20).x * 5 / 9 + box_points.at(22).x * 2 / 9;
	v2.y = box_points.at(19).y * 2 / 9 + box_points.at(20).y * 5 / 9 + box_points.at(22).y * 2 / 9;
	v2.z = box_points.at(19).z * 2 / 9 + box_points.at(20).z * 5 / 9 + box_points.at(22).z * 2 / 9;

	v3.x = box_points.at(20).x * 2 / 9 + box_points.at(22).x * 5 / 9 + box_points.at(23).x * 2 / 9;
	v3.y = box_points.at(20).y * 2 / 9 + box_points.at(22).y * 5 / 9 + box_points.at(23).y * 2 / 9;
	v3.z = box_points.at(20).z * 2 / 9 + box_points.at(22).z * 5 / 9 + box_points.at(23).z * 2 / 9;

	v4.x = box_points.at(22).x * 2 / 9 + box_points.at(18).x * 5 / 9 + box_points.at(19).x * 2 / 9;
	v4.y = box_points.at(22).y * 2 / 9 + box_points.at(18).y * 5 / 9 + box_points.at(19).y * 2 / 9;
	v4.z = box_points.at(22).z * 2 / 9 + box_points.at(18).z * 5 / 9 + box_points.at(19).z * 2 / 9;


	if (box_points.at(18).x != box_points.at(12).x) {
		float giulgi = (box_points.at(18).y - box_points.at(12).y) / (box_points.at(18).x - box_points.at(12).x);
		float dx = sqrt(giulgi * giulgi + 1) / ((giulgi * giulgi + 1) * 100) * 9;
		float dy = giulgi * dx;

		if (x1 > x3) {
			v1.x += dx;
			v1.y += dy;
			v2.x += dx;
			v2.y += dy;
			v3.x += dx;
			v3.y += dy;
			v4.x += dx;
			v4.y += dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += dx;
			v5.y += dy;
		}
		else {
			v1.x -= dx;
			v1.y -= dy;
			v2.x -= dx;
			v2.y -= dy;
			v3.x -= dx;
			v3.y -= dy;
			v4.x -= dx;
			v4.y -= dy;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= dx;
			v5.y -= dy;
		}
	}
	else {
		if (x1 > x3) {
			v1.x += (float)0.09;
			v2.x += (float)0.09;
			v3.x += (float)0.09;
			v4.x += (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x += (float)0.09;
		}
		else {
			v1.x -= (float)0.09;
			v2.x -= (float)0.09;
			v3.x -= (float)0.09;
			v4.x -= (float)0.09;

			v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
			v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
			v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
			v5.x -= (float)0.09;
		}
	}

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	//밑면

	v1.x = box_points.at(24).x * 2 / 9 + box_points.at(25).x * 5 / 9 + box_points.at(26).x * 2 / 9;
	v1.y = box_points.at(24).y * 2 / 9 + box_points.at(25).y * 5 / 9 + box_points.at(26).y * 2 / 9;
	v1.z = box_points.at(24).z * 2 / 9 + box_points.at(25).z * 5 / 9 + box_points.at(26).z * 2 / 9;

	v2.x = box_points.at(25).x * 2 / 9 + box_points.at(26).x * 5 / 9 + box_points.at(28).x * 2 / 9;
	v2.y = box_points.at(25).y * 2 / 9 + box_points.at(26).y * 5 / 9 + box_points.at(28).y * 2 / 9;
	v2.z = box_points.at(25).z * 2 / 9 + box_points.at(26).z * 5 / 9 + box_points.at(28).z * 2 / 9;

	v3.x = box_points.at(26).x * 2 / 9 + box_points.at(28).x * 5 / 9 + box_points.at(29).x * 2 / 9;
	v3.y = box_points.at(26).y * 2 / 9 + box_points.at(28).y * 5 / 9 + box_points.at(29).y * 2 / 9;
	v3.z = box_points.at(26).z * 2 / 9 + box_points.at(28).z * 5 / 9 + box_points.at(29).z * 2 / 9;

	v4.x = box_points.at(28).x * 2 / 9 + box_points.at(24).x * 5 / 9 + box_points.at(25).x * 2 / 9;
	v4.y = box_points.at(28).y * 2 / 9 + box_points.at(24).y * 5 / 9 + box_points.at(25).y * 2 / 9;
	v4.z = box_points.at(28).z * 2 / 9 + box_points.at(24).z * 5 / 9 + box_points.at(25).z * 2 / 9;


	v1.z += (float)0.09;
	v2.z += (float)0.09;
	v3.z += (float)0.09;
	v4.z += (float)0.09;

	v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
	v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
	v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
	v5.z += (float)0.09;

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	//윗면

	v1.x = box_points.at(30).x * 2 / 9 + box_points.at(31).x * 5 / 9 + box_points.at(32).x * 2 / 9;
	v1.y = box_points.at(30).y * 2 / 9 + box_points.at(31).y * 5 / 9 + box_points.at(32).y * 2 / 9;
	v1.z = box_points.at(30).z * 2 / 9 + box_points.at(31).z * 5 / 9 + box_points.at(32).z * 2 / 9;

	v2.x = box_points.at(31).x * 2 / 9 + box_points.at(32).x * 5 / 9 + box_points.at(34).x * 2 / 9;
	v2.y = box_points.at(31).y * 2 / 9 + box_points.at(32).y * 5 / 9 + box_points.at(34).y * 2 / 9;
	v2.z = box_points.at(31).z * 2 / 9 + box_points.at(32).z * 5 / 9 + box_points.at(34).z * 2 / 9;

	v3.x = box_points.at(32).x * 2 / 9 + box_points.at(34).x * 5 / 9 + box_points.at(35).x * 2 / 9;
	v3.y = box_points.at(32).y * 2 / 9 + box_points.at(34).y * 5 / 9 + box_points.at(35).y * 2 / 9;
	v3.z = box_points.at(32).z * 2 / 9 + box_points.at(34).z * 5 / 9 + box_points.at(35).z * 2 / 9;

	v4.x = box_points.at(34).x * 2 / 9 + box_points.at(30).x * 5 / 9 + box_points.at(31).x * 2 / 9;
	v4.y = box_points.at(34).y * 2 / 9 + box_points.at(30).y * 5 / 9 + box_points.at(31).y * 2 / 9;
	v4.z = box_points.at(34).z * 2 / 9 + box_points.at(30).z * 5 / 9 + box_points.at(31).z * 2 / 9;


	v1.z -= (float)0.09;
	v2.z -= (float)0.09;
	v3.z -= (float)0.09;
	v4.z -= (float)0.09;

	v5.x = (v1.x + v2.x + v3.x + v4.x) / 4;
	v5.y = (v1.y + v2.y + v3.y + v4.y) / 4;
	v5.z = (v1.z + v2.z + v3.z + v4.z) / 4;
	v5.z -= (float)0.09;

	arrow_points.push_back(v1); arrow_points.push_back(v2); arrow_points.push_back(v3);
	arrow_points.push_back(v3); arrow_points.push_back(v4); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v1); arrow_points.push_back(v4);
	arrow_points.push_back(v5); arrow_points.push_back(v2); arrow_points.push_back(v1);
	arrow_points.push_back(v5); arrow_points.push_back(v3); arrow_points.push_back(v2);
	arrow_points.push_back(v5); arrow_points.push_back(v4); arrow_points.push_back(v3);

	arrow_line_points.push_back(v5); arrow_line_points.push_back(v4); arrow_line_points.push_back(v1);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v1); arrow_line_points.push_back(v2);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v2); arrow_line_points.push_back(v3);
	arrow_line_points.push_back(v5); arrow_line_points.push_back(v3); arrow_line_points.push_back(v4);

	VertexBufferData(VAO_arrow, VBO_POS, arrow_points.size(), sizeof(glm::vec3), arrow_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_arrow_line, VBO_POS, arrow_line_points.size(), sizeof(glm::vec3), arrow_line_points.data(), GL_STATIC_DRAW);
	VertexBufferData(VAO_direction, VBO_POS, arrow_points.size(), sizeof(glm::vec3), arrow_points.data(), GL_STATIC_DRAW);
}

void UserData::CreateButton(std::vector<interface_info>& interfaces, int type, glm::vec2 location, float width, float height, const char* text, const char* path, int channel) {
	interface_info itf;

	itf.id = 0;
	itf.location = location;
	itf.width = width;
	itf.height = height;
	itf.text = text;
	itf.type = type;
	itf.onCursor = false;
	itf.icon_tex.tex = NULL;
	itf.icon_tex.data = NULL;
	itf.icon_tex.width = 0;
	itf.icon_tex.height = 0;
	itf.icon_tex.n = 0;
	
	if (path != NULL) {
		Load_texture(path, channel, itf.texture);
	}
	else {
		Load_texture(itf.type, itf.texture);
	}
	
	float w = width / 2;
	float h = height / 2;
	float x = location.x;
	float y = location.y;

	glm::vec2 pos[6] = {
	{ x - w, y + h },{ x - w, y - h}, {x + w, y - h},
	{ x + w, y - h },{ x + w, y + h },{ x - w, y + h }
	};

	for (int i = 0; i < 6; i++)
		itf.pos[i] = pos[i];

	glm::vec2 quad_texcoord[6] = {	
	{ 0.f, 0.f },{ 0.f, 1.f },{ 1.f, 1.f },
	{ 1.f, 1.f },{ 1.f, 0.f },{ 0.f, 0.f }
	};

	std::map<GLenum, const char*> interface_shader_map = {
		{ GL_VERTEX_SHADER, ShaderSource::GetSource("interface_vs") },
	{ GL_FRAGMENT_SHADER, ShaderSource::GetSource("interface_fs") }
	};
	itf.program = CreateProgram(interface_shader_map);

	std::vector<VBO_info> VBO_info_interface = {
		{ VBO_POS, 6, pos, 0, sizeof(glm::vec2) },
	{ VBO_TEX, 6, quad_texcoord, 0, sizeof(glm::vec2) }
	};

	float wh = 0.05f;
	glm::vec2 icon_pos[6] = {
		{ x - wh, y + wh },{ x - wh, y - wh },{ x + wh, y - wh },
	{ x + wh, y - wh },{ x + wh, y + wh },{ x - wh, y + wh }
	};
	
	std::vector<VBO_info> VBO_info_icon = {
		{ VBO_POS, 6, icon_pos, 0, sizeof(glm::vec2) },
	{ VBO_TEX, 6, quad_texcoord, 0, sizeof(glm::vec2) }
	};

	std::vector<attrib_info> attrib_info_interface = {
		{ 0, GL_FLOAT, 2, 0 },
	{ 1, GL_FLOAT, 2, 0 }
	};


	itf.VAO = CreateVertexArray(VBO_info_interface, attrib_info_interface);
	itf.VAO_icon = CreateVertexArray(VBO_info_icon, attrib_info_interface);

	interfaces.push_back(itf);
}

/////CreateButton의 중심좌표와 가로세로 길이를 픽셀로 받는다.
void UserData::CreateButton(std::vector<interface_info>& interfaces, int type, glm::ivec2 location, int width, int height, const char* text, const char* path) {
	float w, h;
	w = 2 * (float)width / this->width;
	h = 2 * (float)height / this->height;
	glm::vec2 lo;
	lo.x = 2 * (float)location.x / this->width - 1;
	lo.y = 2 * (float)location.y / this->height - 1;

	CreateButton(interfaces, type, lo, w, h, text, path);
}

int UserData::checkClickButton(std::vector<interface_info>& interfaces, float x, float y) {
	for (int i = interfaces.size()-1; i >= 0; i--) {
		if (interfaces[i].pos[0].x <= x && x <= interfaces[i].pos[2].x &&
			interfaces[i].pos[1].y <= y && y <= interfaces[i].pos[0].y) {
			//printf("interface button %d clicked!!\n", i);
			return i;
		}
	}
	return -1;
}


void UserData::Save_box() {
	FILE* fp = fopen("save.txt", "a");
	fprintf(fp, "box\n");
	for (int i = 0; i < box_points.size(); i++) {
		fprintf(fp, "%g %g %g\n", box_points[i].x, box_points[i].y, box_points[i].z);
	}

	fprintf(fp, "outlier\n");
	for(int i = 0; i < foutlier_points.size(); i++){
		fprintf(fp, "%g %g %g\n", foutlier_points[i].x, foutlier_points[i].y, foutlier_points[i].z);
	}
	fclose(fp);
}


void UserData::Load_box() {
	FILE* fp = fopen("save.txt", "r");
	std::string temp;
	int index = 0;
	std::vector<std::string> save_data;
	save_data.clear(); save_data.reserve(10);
	while (!feof(fp)) {
		while (1) {
			if (feof(fp))
				break;

			fgets(&temp[0], 1024, fp);
			if (temp.compare("box") && index != 0) {
				break;
			}
			
			save_data[index].append(temp);
		}
		index++;
	}
	fclose(fp);
}

void UserData::cleanMouseBehavior() {
	behavior = camera::Trackball::Behavior::NOTHING;
	rotation_mode = false;
	zoom_mode = false;
	panning_mode = false;
	glfwSetCursor(window, std_cursor);
}