#include "stdafx.h"
#include "app_base.h"

#include <stdexcept>
#include <chrono>
#include <GL/glew.h>

void AppBase::appInit(int width, int height, const char* window_title) {

	glfwInit();
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // OpenGL compatibility profile�� ����Ѵ�.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

	// glfw�� callback ��� ����� �� �ֵ��� UserPointer��� ���� ������ �����Ѵ�.
	// UserPointer�� ��� Ÿ���̵� �����͸� �����ϰ�
	// �ʿ��� ��ġ���� glfwGetWindowUserPointer�� ���� �ٽ� �������� �� �ִ�.
	// �� �Լ��� ���� ���� ȣ��Ǵ� app Ŭ������ �����͸� �����ϰ� �ٽ� ���� �� �ִ� ���̴�.
	// 
	// �� ���п� �ٷ� �Ʒ����� ��ϵǴ� �ݹ� �Լ����� static method �ӿ��� �ұ��ϰ�
	// ���ο��� �ٽ� onWindowSize�� ���� ���� �ݹ� �Լ����� ȣ���� �� �ִ�.
	glfwSetWindowUserPointer(window, this);

	glfwSetWindowSizeCallback(window, AppBase::onWindowSizeCallback);
	glfwSetKeyCallback(window, AppBase::onKeyCallback);
	glfwSetCursorPosCallback(window, AppBase::onCursorPosCallback);
	glfwSetMouseButtonCallback(window, AppBase::onMouseButtonCallback);
	glfwSetScrollCallback(window, AppBase::onScrollCallback);

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("failed to initialize glew!");
	}

}

void AppBase::appCleanup() {

	glfwDestroyWindow(window);
	glfwTerminate();
}

void AppBase::onWindowSizeCallback(GLFWwindow* window, int width, int height) {
	// UserPointer�� ����� �����͸� �޾Ƽ� �θ� Ŭ���� Ÿ���� AppBase�� ĳ���� �Ѵ�.
	AppBase* app = reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window));

	// ���� app�� ����� �����Ͱ� AppBase�� ��ӹ��� Ŭ������ �����Ͷ��
	// onWindowSize �Լ� ���� ������(override)�ߴ� �Լ��� ȣ��� ���̴�.
	app->onWindowSize(window, width, height);

	// �Ʒ� �Լ��鵵 �� �Լ��� ������ �������� �Ǿ��ִ�.
}
void AppBase::onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window))->onKey(window, key, scancode, action, mods); }
void AppBase::onCursorPosCallback(GLFWwindow* window, double x, double y) { reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window))->onCursorPos(window, x, y); }
void AppBase::onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) { reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window))->onMouseButton(window, button, action, mods); }
void AppBase::onScrollCallback(GLFWwindow* window, double xoffset, double yoffset) { reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window))->onScroll(window, xoffset, yoffset); }

void AppBase::Init(int width, int height, const char* window_title) {

	appInit(width, height, window_title);
	userInit(width, height);
}

void AppBase::Run() {

	auto t0 = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		auto t1 = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(t1 - t0).count();
		t0 = t1;
		
		
		update(dt);
		render(dt);

		glfwSwapBuffers(window);
	}

	cleanup();
	appCleanup();
}