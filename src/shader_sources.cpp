#include "stdafx.h"
#include "shader_sources.h"

std::map<std::string, const char*> ShaderSource::shader_source_map;
ShaderSourceStaticInitializer ShaderSource::static_initializer;

const char* ShaderSource::GetSource(const char* shader_name) {
	// "pointcloud_vs" 같은 이름을 주고 코드를 반환하는 함수인데
	// 이름에 오타를 내거나 존재하지 않는 이름을 입력하면 예외를 반환한다.
	if (shader_source_map.count(shader_name) == 0) {
		std::string message = "shader name[" + std::string(shader_name) + "] does not exists.";
		throw std::runtime_error(message.c_str());
	}

	return shader_source_map[shader_name];
}

// 생성자의 시그니처 부분을 매크로로 만들어 쓰면
// Visual Studio가 코드를 들여쓰기 하지 않는다.
// * 설정에 따라 다를 수 있지만, 거의 먹히는 것 같다.
#define STATIC_INITIALIZE ShaderSourceStaticInitializer::ShaderSourceStaticInitializer()
ShaderSourceStaticInitializer::ShaderSourceStaticInitializer() {

ShaderSource::shader_source_map["pointcloud_vs"] = R"(
#version 430
in vec3 pos;
in uvec3 color;

uniform mat4 MVP;

out vec3 vcolor;

void main() {
	float norm = 1.0/255.0;
	vcolor = vec3(color)*norm;
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["pointcloud_fs"] = R"(
#version 430
in vec3 vcolor;

out vec4 fragcolor;

void main() {
	fragcolor = vec4(vcolor, 1);
}
)";

ShaderSource::shader_source_map["box_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["box_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(0, 0, 0, 0.1);
}
)";

ShaderSource::shader_source_map["box_edge_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["box_edge_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(0, 0, 0, 1);
}
)";


//ShaderSource::shader_source_map["edit_vs"] = R"(
//#version 430
//
//layout(location = 0) in vec3 pos;
//layout(location = 1) in vec2 texcoord;
//
//uniform mat4 MVP;
//
//out vec2 vtexcoord;
//
//void main() {
//	gl_Position = MVP * vec4(pos, 1);
//	vtexcoord = texcoord; 
//}
//)";
//
//ShaderSource::shader_source_map["edit_fs"] = R"(
//#version 430
//
//in vec2 vtexcoord;
//
//uniform sampler2D tex;
//uniform bool isTex;
//
//out vec4 fragcolor;
//
//void main() {
//	if(isTex){
//		fragcolor = vec4(texture(tex, vtexcoord).rgb, 1);
//	}else{
//		fragcolor = vec4(1, 0, 0, 1);
//	}
//}
//)";

ShaderSource::shader_source_map["edit_vs"] = R"(
#version 430 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main(){
		// Output position of the vertex, in clip space : MVP * position
		gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
		
		// Position of the vertex, in worldspace : M * position
		Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
		
		// Vector that goes from the vertex to the camera, in camera space.
		// In camera space, the camera is at the origin (0,0,0).
		vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
		EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

		// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
		vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
		LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
		
		// Normal of the the vertex, in camera space
		Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
		
		// UV of the vertex. No special space for this one.
		UV = vertexUV;
}
)";


ShaderSource::shader_source_map["edit_fs"] = R"(
#version 430 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform bool isTex;

void main(){
	if(isTex){
		// Light emission properties
		// You probably want to put them as uniforms
		vec3 LightColor = vec3(1,1,1);
		float LightPower = 0.2f;
		
		// Material properties
		vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
		vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

		// Distance to the light
		float distance = length( LightPosition_worldspace - Position_worldspace );

		// Normal of the computed fragment, in camera space
		vec3 n = normalize( Normal_cameraspace );
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize( LightDirection_cameraspace );
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,l ), 0,1 );
		
		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( E,R ), 0,1 );
		
		color = 
			// Ambient : simulates indirect lighting
			MaterialAmbientColor +
			// Diffuse : "color" of the object
			MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
			// Specular : reflective highlight, like a mirror
			MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,10) / (distance*distance);
		
		//color = (LightPower * cosTheta / (distance*distance)).xxx;
			
				
		
	}else{
		color = vec3(1, 0, 0);
	}
}
)";



ShaderSource::shader_source_map["select_vs"] = R"(
#version 430

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;

out vec2 vtexcoord;

void main() {
	gl_Position = vec4(pos, 0, 1);
	vtexcoord = texcoord;
}
)";

ShaderSource::shader_source_map["select_fs"] = R"(
#version 430

in vec2 vtexcoord;
out vec4 fragcolor;

uniform sampler2D tex;
void main() {
	fragcolor = vec4(texture(tex, vtexcoord).rgb, 1);
}
)";

ShaderSource::shader_source_map["str_render_text_vs"] = R"(
#version 430

in vec2 pos;
in vec2 tex;

out vec2 texcoord;

uniform mat4 T;

void main() {
        gl_Position = T*vec4(pos, 0, 1);
        texcoord = tex;
}
)";

ShaderSource::shader_source_map["str_render_text_fs"] = R"(
#version 430

in vec2 texcoord;

uniform sampler2D tex;
uniform vec4 color;

out vec4 fragcolor;

void main() {
        fragcolor = vec4(1, 1, 1, texture(tex, texcoord).r) * color;
}
)";

ShaderSource::shader_source_map["interface_vs"] = R"(
#version 430

in vec2 pos;
in vec2 texcoord;

uniform bool isMain;
uniform mat4 T;

out vec2 vtexcoord;
void main() {
	if(isMain){
		gl_Position = vec4(pos, 0, 1);
		vtexcoord = texcoord;
	}
	else{
		gl_Position = T * vec4(pos, 0, 1);
		vtexcoord = texcoord;
	}
}
)";

ShaderSource::shader_source_map["interface_fs"] = R"(
#version 430

in vec2 vtexcoord;
uniform sampler2D tex;
uniform bool onCursor;
uniform bool rbox;

out vec4 fragcolor;
void main() {
	vec4 c = texture(tex, vtexcoord);
	float w = 0.05;
	
	if(c.a == 0)
		discard;
	else if(c.r == 1 && c.g == 1 && c.b == 1)
		discard;

	if(rbox){
		if(vtexcoord.x <= w || vtexcoord.x >= 1-w || vtexcoord.y <= w || vtexcoord.y >= 1-w){
		//fragcolor = vec4(1, 0, 0, 1);
			fragcolor = vec4(1, 0, 0, 1);
		}else{
			fragcolor = vec4(texture(tex, vtexcoord).rgb, 1);
		}
	}else{
		
		if(onCursor){
			if(vtexcoord.x <= w || vtexcoord.x >= 1-w || vtexcoord.y <= w || vtexcoord.y >= 1-w){
				fragcolor = vec4(0.9, 0.9, 0.9, 1);
			}else{
				fragcolor = vec4(texture(tex, vtexcoord).rgb, 1);
			}
		}else{
			fragcolor = vec4(texture(tex, vtexcoord).rgb, 1);
		}
	}
}
)";



ShaderSource::shader_source_map["side_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["side_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(1, 0, 0, 0.1);
}
)";

ShaderSource::shader_source_map["arrow_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["arrow_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(1, 1, 1, 1);
}
)";

ShaderSource::shader_source_map["arrow_line_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["arrow_line_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(0, 0, 0, 1);
}
)";

ShaderSource::shader_source_map["direction_vs"] = R"(
#version 430
in vec3 pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(pos, 1);
} 
)";

ShaderSource::shader_source_map["direction_fs"] = R"(
#version 430
out vec4 fragcolor;

void main() {
	fragcolor = vec4(1, 0, 0, 1);
}
)";

}
