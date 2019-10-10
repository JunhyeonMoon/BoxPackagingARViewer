#pragma once
#include <map>
#include <string>

struct ShaderSourceStaticInitializer {
	ShaderSourceStaticInitializer();
};

struct ShaderSource {
	static std::map<std::string, const char*> shader_source_map;

	// static member�� shader_source_map�� shader source ���ڿ��� �ִ� �۾���
	// ���α׷� ���ۿ� �ռ� ���� ���� �̷������ �Ѵ�.
	// 
	// ShaderSource Ŭ����(����ü)�� ���ǵǴ� �������� 
	// static member�� static_initializer�� �ʱ�ȭ�ϱ� ���� 
	// static_initializer�� �����ڰ� ȣ��ǹǷ�
	// �� �����ڿ��� shader_source_map�� �ʱ�ȭ�ϸ�
	// �׻� ShaderSource�� �����ϴ� �ڵ庸�� ���� shader_source_map �ʱ�ȭ�� �� �� �ְ� �ȴ�.
	// �ٸ� ������ static constructor�� ���������� C++ �� �׷��� �����Ƿ�
	// ��� ����ϴ� ����� �����̴�.
	static ShaderSourceStaticInitializer static_initializer;

	static const char* GetSource(const char* shader_name);
};

