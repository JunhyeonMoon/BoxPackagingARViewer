#pragma once
#include <map>
#include <string>

struct ShaderSourceStaticInitializer {
	ShaderSourceStaticInitializer();
};

struct ShaderSource {
	static std::map<std::string, const char*> shader_source_map;

	// static member인 shader_source_map에 shader source 문자열을 넣는 작업이
	// 프로그램 시작에 앞서 가장 먼저 이루어져야 한다.
	// 
	// ShaderSource 클래스(구조체)가 정의되는 시점에서 
	// static member인 static_initializer를 초기화하기 위해 
	// static_initializer의 생성자가 호출되므로
	// 그 생성자에서 shader_source_map을 초기화하면
	// 항상 ShaderSource에 접근하는 코드보다 먼저 shader_source_map 초기화를 할 수 있게 된다.
	// 다른 언어에서는 static constructor를 지원하지만 C++ 은 그렇지 않으므로
	// 대신 사용하는 편법의 일종이다.
	static ShaderSourceStaticInitializer static_initializer;

	static const char* GetSource(const char* shader_name);
};

