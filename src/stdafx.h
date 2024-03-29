#define _CRT_SECURE_NO_WARNINGS
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// stdafx.h 파일은 미리 컴파일 된 헤더 (precompiled header) 파일이다.
// 프로그램의 규모가 커지면 include 하는 헤더파일과 그에 따라 빌드해야 할 소스파일이 많아진다.
// 수 많은 파일들 중 겨우 몇 개의 파일에서 약간만 고쳐도 많은 파일을 새로 빌드하느라 오랜 시간이 걸릴 것이다.
// 
// C++ standard library나 3rdparty 라이브러리는 코드가 수정될 일이 없다.
// 따라서 매번 해당 파일을 빌드하는데 걸리는 시간이 낭비될 것이다.
// 그러한 파일들을 미리 컴파일 된 헤더에서 include함으로써,
// 다시 빌드될 필요 없는 것들은 생략함으로써 시간을 절약할 수 있다.
// 
// 사용법:
//		내가 작성하고 있는 파일이 아닌 모든 헤더파일의 include를 이곳으로 복사해온다.
//		그리고 해당 include를 필요로 하는 모든 소스파일에서 stdafx.h를 include 한다.
//		include의 dependancy tree에서 stdafx.h가 모든 소스파일에서 가장 먼저 include되므로
//		stdafx.h 에서 include 하는 모든 헤더들은 한번 컴파일 된 뒤 재사용된다.
//		이후 다른 파일에서 include 될 때는 재사용되는 바이너리를 사용하게 된다.
//
// 자세한 내용은 precompiled header 에 대해 검색해보는 것을 추천한다.
// 이를 사용하지 않으려면, stdafx.h와 stdafx.cpp를 제거하고,
// 프로젝트 설정에서 [C++]-[Precompiled Header] 항목에 가서 사용하지 않음으로 설정하면 된다.

// pragma region 은 코드의 구획(영역)을 구분하는 매크로로 아마도 Visual Studio에서만 사용가능한 것 같다.
// 단지 영역을 구분하기 위함일 뿐이므로 앞뒤로 주석을 다는 것과 크게 다를 것 없다.
// 유일한 차이가 있다면, Visual Studio 에서는 pragma region 표시 왼쪽의 [-]를 눌러서 접었다 폈다 할 수 있다는 점 정도?

#pragma region app_base.cpp
#include <stdexcept>
#include <chrono>
#include <GL/glew.h>
#pragma endregion

#pragma region app_base.h
#include <GLFW/glfw3.h>
#pragma endregion

#pragma region camera.h
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#pragma endregion

#pragma region global_table.h
#include <FindSurface.h>
#include <librealsense/rs.hpp>
#include <algorithm>
#include <math.h>
#pragma endregion

#pragma region shader_sources.h
#include <map>
#include <string>
#pragma endregion

#pragma region utility_functions.h
#include <GL/glew.h>
#include <map>
#include <vector>
#include <stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#pragma endregion

// TODO: reference additional headers your program requires here
