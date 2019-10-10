#pragma once
#include <GLFW/glfw3.h>

// application �� ���¿� ������ ��� ����� Ŭ�����̴�.
// ����: �� Ŭ������ ��ӹ޴� ���ο� Ŭ������ ����
// virtual�� ����� ����Լ����� ������(override) �Ѵ�.
class AppBase {
	
protected:
	GLFWwindow * window;

private:
	
	// �� �Լ����� glew�� glfw�� �ʱ�ȭ�� �����Ѵ�. 
	void appInit(int width, int height, const char* window_title);

	// ����ڰ� ��ӹ޾� ���� ������ �Լ����̴�.
	// render�� pure virtual function �̹Ƿ� �ݵ�� �������ؾ� �Ѵ�.
	// ������ �Լ��� ����� �����Լ� �̹Ƿ� ������ ���� ������ �� �Լ��� ȣ��ȴ�. (�ƹ��͵� ���� �ʴ´�.)
	virtual void userInit(int width, int height) {}	// ������ init�� �ش��Ѵ�. �Ű������� �������� ũ��.
	virtual void update(float time_elapsed) {}	// update
	virtual void render(float time_elapsed) = 0;	// render
	virtual void cleanup() {}	// userInit���� �Ҵ��� �ڿ��� �����Ѵ�.
	
	// �� �Լ��� appInit���� �Ҵ��� �ڿ��� �����Ѵ�.
	void appCleanup();

	// �� �Լ��� ������(override)�Ͽ� �ݹ� �Լ����� ó���� ������ �ۼ��Ѵ�.
	virtual void onWindowSize(GLFWwindow* window, int width, int height) {}
	virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {}
	virtual void onCursorPos(GLFWwindow* window, double x, double y) {}
	virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) {}
	virtual void onScroll(GLFWwindow* window, double xoffset, double yoffset) {}

	// �� static �Լ����� ���� �ݹ� �Լ����� ȣ�����ִ� �븮(delegate) �Լ����̴�.
	// ����Լ��� �Ϲ����� ������δ� �Լ� �����͸� �̿��� �ݹ� ����� �� �� ����.
	// �̴� ����Լ��� �ش� Ŭ������ �ν��Ͻ� �����͸� �޴� ���� �Ű������� �ֱ� �����̴�.
	// ��)
	//	class Foo {
	//		void bar(int x);
	//	}
	//
	//  ���� �Լ� bar�� ��ǻ� ������ ���� �Լ��� ����.
	//	void bar(Foo* this, int x);
	//
	// ���� �Լ� �����ͷ� �����Ϸ��� �ϸ� Ÿ���� �ùٸ��� �����Ƿ� ������ ���� �ʴ´�.
	// �ڼ��� ������ AppBase.cpp ������ ��������.
	static void onWindowSizeCallback(GLFWwindow* window, int width, int height);
	static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onCursorPosCallback(GLFWwindow* window, double x, double y);
	static void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void onScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	
public:

	// �Ʒ� �� �Լ��� ���� �Լ����� ȣ���Ѵ�.
	void Init(int width, int height, const char* window_title);
	void Run();
};