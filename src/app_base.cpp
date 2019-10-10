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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // OpenGL compatibility profile을 사용한다.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

	// glfw는 callback 등에서 사용할 수 있도록 UserPointer라는 내부 변수를 지원한다.
	// UserPointer에 어떠한 타입이든 포인터를 전달하고
	// 필요한 위치에서 glfwGetWindowUserPointer를 통해 다시 돌려받을 수 있다.
	// 이 함수를 통해 현재 호출되는 app 클래스의 포인터를 저장하고 다시 얻을 수 있는 것이다.
	// 
	// 이 덕분에 바로 아래에서 등록되는 콜백 함수들이 static method 임에도 불구하고
	// 내부에서 다시 onWindowSize와 같은 실제 콜백 함수들을 호출할 수 있다.
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
	// UserPointer에 저장된 포인터를 받아서 부모 클래스 타입인 AppBase로 캐스팅 한다.
	AppBase* app = reinterpret_cast<AppBase*>(glfwGetWindowUserPointer(window));

	// 만약 app에 저장된 포인터가 AppBase를 상속받은 클래스의 포인터라면
	// onWindowSize 함수 역시 재정의(override)했던 함수가 호출될 것이다.
	app->onWindowSize(window, width, height);

	// 아래 함수들도 이 함수와 동일한 과정으로 되어있다.
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