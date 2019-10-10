#pragma once
#include <GLFW/glfw3.h>

// application 의 상태와 동작을 모두 담당할 클래스이다.
// 사용법: 이 클래스를 상속받는 새로운 클래스를 만들어서
// virtual로 선언된 멤버함수들을 재정의(override) 한다.
class AppBase {
	
protected:
	GLFWwindow * window;

private:
	
	// 이 함수에서 glew와 glfw의 초기화를 수행한다. 
	void appInit(int width, int height, const char* window_title);

	// 사용자가 상속받아 새로 정의할 함수들이다.
	// render는 pure virtual function 이므로 반드시 재정의해야 한다.
	// 나머지 함수는 평범한 가상함수 이므로 재정의 하지 않으면 빈 함수가 호출된다. (아무것도 하지 않는다.)
	virtual void userInit(int width, int height) {}	// 기존의 init에 해당한다. 매개변수는 윈도우의 크기.
	virtual void update(float time_elapsed) {}	// update
	virtual void render(float time_elapsed) = 0;	// render
	virtual void cleanup() {}	// userInit에서 할당한 자원을 해제한다.
	
	// 이 함수는 appInit에서 할당한 자원을 해제한다.
	void appCleanup();

	// 이 함수를 재정의(override)하여 콜백 함수에서 처리할 내용을 작성한다.
	virtual void onWindowSize(GLFWwindow* window, int width, int height) {}
	virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {}
	virtual void onCursorPos(GLFWwindow* window, double x, double y) {}
	virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) {}
	virtual void onScroll(GLFWwindow* window, double xoffset, double yoffset) {}

	// 이 static 함수들은 위의 콜백 함수들을 호출해주는 대리(delegate) 함수들이다.
	// 멤버함수는 일반적인 방법으로는 함수 포인터를 이용해 콜백 등록을 할 수 없다.
	// 이는 멤버함수에 해당 클래스의 인스턴스 포인터를 받는 숨은 매개변수가 있기 때문이다.
	// 예)
	//	class Foo {
	//		void bar(int x);
	//	}
	//
	//  위의 함수 bar는 사실상 다음과 같은 함수와 같다.
	//	void bar(Foo* this, int x);
	//
	// 따라서 함수 포인터로 전달하려고 하면 타입이 올바르지 않으므로 전달이 되지 않는다.
	// 자세한 내용은 AppBase.cpp 파일을 참조하자.
	static void onWindowSizeCallback(GLFWwindow* window, int width, int height);
	static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onCursorPosCallback(GLFWwindow* window, double x, double y);
	static void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void onScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	
public:

	// 아래 두 함수를 메인 함수에서 호출한다.
	void Init(int width, int height, const char* window_title);
	void Run();
};