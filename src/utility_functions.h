#pragma once

#include <GL/glew.h>
#include <map>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

/**********[Data structures]**********/

#define VBO_POS "pos"
#define VBO_COLOR "color"
#define VBO_TEX "texcoord"
#define VBO_NORM "normal"
#define MAX(x, y) x > y ? x : y

/*
GLuint binding_index;
GLenum type;			// GL_FLOAT, GL_UNSIGNED_BYTE, GL_UNSIGNED_INT, GL_INT
GLint channels;			// xyz, rgb 채널 수
GLuint relative_offset; // stride 내에서 해당 데이터가 시작되는 바이트 offset (e.g., offsetof(vertex, pos))
*/
struct attrib_info {
	GLuint binding_index;
	GLenum type;			// GL_FLOAT, GL_UNSIGNED_BYTE, GL_UNSIGNED_INT, GL_INT
	GLint channels;			// xyz, rgb 채널 수
	GLuint relative_offset; // stride 내에서 해당 데이터가 시작되는 바이트 offset (e.g., offsetof(vertex, pos))
};

/*
std::string name;		// buffer name
GLsizeiptr size;		// total byte size of data
GLvoid* pointer;		// data pointer
GLuint offset;			// vbo 버퍼에서 데이터가 시작되는 바이트 offset (거의 항상 0)
GLsizei stride;			// stride (반복되는 데이터 단위의 바이트 크기)
*/
struct VBO_info {
	std::string name;		// buffer name
	GLuint count;			// 버퍼에 들어있는 stride 단위 데이터 개수
	GLvoid* pointer;		// data pointer
	GLuint offset;			// vbo 버퍼에서 데이터가 시작되는 바이트 offset (거의 항상 0)
	GLsizei stride;			// stride (반복되는 데이터 단위의 바이트 크기)
};

struct texture_info {
	int width;
	int height;
	int n; // # 8-bit components per pixel
	unsigned char* data;
	GLuint tex;
};


struct box_specification {
	float l1;
	float l2;
	float l3;
};


struct Program {
	GLuint program;
	std::map<GLenum, GLuint> shaders;

	operator GLuint() { return program; }
};

Program CreateProgram(std::map<GLenum, const char*>& shader_source_map);
void Uniform1f(Program& program, const GLchar* name, GLfloat v0);
void Uniform2f(Program& program, const GLchar* name, GLfloat v0, GLfloat v1);
void Uniform3f(Program& program, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);
void Uniform4f(Program& program, const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
void Uniform1i(Program& program, const GLchar* name, GLint v0);
void Uniform2i(Program& program, const GLchar* name, GLint v0, GLint v1);
void Uniform3i(Program& program, const GLchar* name, GLint v0, GLint v1, GLint v2);
void Uniform4i(Program& program, const GLchar* name, GLint v0, GLint v1, GLint v2, GLint v3);
void Uniform1ui(Program& program, const GLchar* name, GLuint v0);
void Uniform2ui(Program& program, const GLchar* name, GLuint v0, GLuint v1);
void Uniform3ui(Program& program, const GLchar* name, GLuint v0, GLuint v1, GLuint v2);
void Uniform4ui(Program& program, const GLchar* name, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
void Uniform1fv(Program& program, const GLchar* name, GLsizei count, const GLfloat* value);
void Uniform2fv(Program& program, const GLchar* name, GLsizei count, const GLfloat* value);
void Uniform3fv(Program& program, const GLchar* name, GLsizei count, const GLfloat* value);
void Uniform4fv(Program& program, const GLchar* name, GLsizei count, const GLfloat* value);
void Uniform1iv(Program& program, const GLchar* name, GLsizei count, const GLint* value);
void Uniform2iv(Program& program, const GLchar* name, GLsizei count, const GLint* value);
void Uniform3iv(Program& program, const GLchar* name, GLsizei count, const GLint* value);
void Uniform4iv(Program& program, const GLchar* name, GLsizei count, const GLint* value);
void Uniform1uiv(Program& program, const GLchar* name, GLsizei count, const GLuint* value);
void Uniform2uiv(Program& program, const GLchar* name, GLsizei count, const GLuint* value);
void Uniform3uiv(Program& program, const GLchar* name, GLsizei count, const GLuint* value);
void Uniform4uiv(Program& program, const GLchar* name, GLsizei count, const GLuint* value);
void UniformMatrix2fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix3fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix4fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix2x3fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix2x4fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix3x2fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix3x4fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix4x2fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void UniformMatrix4x3fv(Program& program, const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat* value);
void DestroyProgram(Program& p);

struct VertexArray {
	GLuint VAO;
	std::map<std::string, GLuint> VBOs;
	GLuint IBO;
	GLsizei count;

	operator GLuint() { return VAO; }
};

VertexArray CreateVertexArray(const std::vector<VBO_info>& vbo_infos, const std::vector<attrib_info>& attrib_infos, GLsizei index_count = 0, GLuint* indices = nullptr);
void VertexBufferData(VertexArray& vao, const char* buffer_name, GLsizei count, GLsizeiptr size, const GLvoid* data, GLenum usage);
void IndexBufferData(VertexArray& vao, GLsizei count, const GLvoid* data, GLenum usage);
void DestroyVertexArray(VertexArray& v);

struct Texture2D {
	GLuint TEX;
	GLsizei width, height;
	GLsizei levels;

	operator GLuint() { return TEX; }
};

Texture2D CreateTexture2D(GLsizei width = 0, GLsizei height = 0, GLsizei levels = 0, GLenum internal_format = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, const GLvoid* pixels = nullptr);
void TextureImage2D(Texture2D t, GLsizei width, GLsizei height, GLsizei level, GLenum internal_format = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, const GLvoid* pixels = nullptr);
GLsizei GetMipLevels(GLsizei width, GLsizei height);
void DestroyTexture2D(Texture2D& t);

struct Framebuffer {
	GLuint FBO;
	GLuint renderbuffer;
	GLuint rendertexture;

	operator GLuint() { return FBO; }
};

void GenerateMipmap(Framebuffer fbo, Texture2D texture, Program program, VertexArray vao, GLuint level);

Framebuffer CreateFramebuffer(GLsizei width = 1, GLsizei height = 1, bool useRenderbuffer = false);
Framebuffer CreateFramebuffer(Texture2D t, bool useRenderbuffer = false);
void AttachFramebufferTexture(Framebuffer f, Texture2D t, GLsizei level);
void DestroyFramebuffer(Framebuffer& f);

struct ubyte3 { unsigned char r, g, b; };
struct ubyte4 { unsigned char r, g, b, a; };


struct Renderer {

protected:
	GLuint program;
	GLuint vao;
	GLuint vbo;

	void createProgram(const char* vs_src, const char* fs_src);
	void deleteProgram();
};

//////

struct TextRenderer : Renderer {

	void Init(const char* vs_src, const char* fs_src);
	void RenderText(const char* text, float x, float y, float scale_x, float scale_y, glm::mat4 T = { 
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 }
		, int type = 0);
	void Release();

private:
	FT_Library ft;
	FT_Face face;
	glm::ivec2 atlas_size;
	GLuint texture;

	struct glyph_metrics { glm::vec2 advance; glm::vec2 bitmap_size; glm::vec2 bitmap_offset; float tex_offset_x; };
	std::map<char, glyph_metrics> glyph_metrics_map;

	void createVertexArray();
	void createTexture();
	void createGlyphAtlas();
};

struct interface_info {
	Program program;
	VertexArray VAO;
	VertexArray VAO_icon;
	int id;
	int type; //textbox : 0, button : 1
	float width;
	float height;
	texture_info texture;
	texture_info icon_tex;
	glm::vec2 location; //버튼의 중심의 좌표
	glm::vec2 pos[6];
	const char* text;
	bool onCursor;
};