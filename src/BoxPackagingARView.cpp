// BoxPackagingARView.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "global_table.h"
#include "interface.h"

struct App : public UserData {
	
	// override 키워드는 이 함수가 부모 클래스에서 선언한 virtual 함수를 재정의하겠다는 것을 의미한다.
	// AppBase 클래스에서 선언하지 않는 함수를 override하려 하거나,
	// 상속받은 가상함수의 이름이나 타입이 달라질 경우 override 키워드에서 컴파일 에러가 발생한다.
	void userInit(int width, int height) override {
		this->width = width;
		this->height = height;

		print_explane();
		Init_OpenGL(width, height);
		Init_RealSense();
		Init_FindSurface();
		Init_Trackball(width, height);
		Init_Interface();
		Init_BoxSpecification();
	}

	void update(float time_elapsed) override {
		Update_RealSense();
		Update_FindSurface();
		
		static int frame = 0;
		frame++;

		if (!Findplane) {
			VertexBufferData(VAO_pointcloud, VBO_POS, depth_points.size(), sizeof(glm::vec3), depth_points.data(), GL_STREAM_DRAW);
			VertexBufferData(VAO_pointcloud, VBO_COLOR, depth_colors.size(), sizeof(ubyte3), depth_colors.data(), GL_STREAM_DRAW);
		}
		
		trackball.update(time_elapsed);
		VP = trackball.projection_matrix()*trackball.view_matrix();

	}

	void render(float time_elapsed) override {

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (main_mode) {
			render_main(main_interface);
		}
		else if (camera_mode) {
			if (Findbox) {
				render_outlier();
				render_box();
			}else{
				render_pointcloud();
			}
			render_interface(camera_interface);
		}
		else if (edit_mode && edit_enable) {
			if (transform_side_mode) {
				render_side();
				//render_arrow();
				//render_direction();
				//render_arrow_line();
				render_interface(unit_interface);
			}
			if (texture_change_mode) {
				render_interface(texture_interface);
			}
			render_edit();
			render_outlier();

			render_interface(edit_interface);
		}
		


		//if (!editmode) {
		//	if (!Findplane) {
		//		render_pointcloud();
		//	}
		//	else {
		//		render_outlier();
		//	}
		//
		//	if (Findbox) {
		//		render_box();
		//	}
		//}
		//else {
		//	render_select();
		//	render_edit();
		//	if (transform_side_mode) {
		//		render_arrow();
		//		render_direction();
		//		render_arrow_line();
		//	}
		//}

		/////////////
	}

	// 이 함수는 상속받은 함수가 아니며 새로 추가된 함수이므로 override를 붙이지 않는다.
	// 또한, 클래스 선언 내에 있기 때문에 render보다 먼저 선언될 필요가 없다.
	// 간단한 함수는 이곳에서 선언하고 내용이 복잡한 함수는 UserData (global_table.h)에서
	// 선언하여 사용하는 것이 좋다.
	void render_pointcloud() {
		glUseProgram(program_pointcloud);
		glBindVertexArray(VAO_pointcloud);

		UniformMatrix4fv(program_pointcloud, "MVP", 1, GL_FALSE, &VP[0][0]);
		glDrawArrays(GL_POINTS, 0, VAO_pointcloud.count);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_outlier() {
		glUseProgram(program_outlier);
		glBindVertexArray(VAO_outlier);

		UniformMatrix4fv(program_outlier, "MVP", 1, GL_FALSE, &VP[0][0]);
		glDrawArrays(GL_POINTS, 0, VAO_outlier.count);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_box() {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(program_box);
		glBindVertexArray(VAO_box);

		UniformMatrix4fv(program_box, "MVP", 1, GL_FALSE, &VP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, VAO_box.count);
		
		glBindVertexArray(0);
		glUseProgram(0);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//box edge
		glUseProgram(program_box_edge);
		glBindVertexArray(VAO_box_edge);
		
		UniformMatrix4fv(program_box_edge, "MVP", 1, GL_FALSE, &VP[0][0]);
		VAO_box_edge.count = 24;
		glDrawElements(GL_LINES, VAO_box_edge.count, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_edit() {
		glUseProgram(program_edit);
		glBindVertexArray(VAO_edit);
		glm::vec3 lightpos = { 9.f, 5.f, 5.f };
		glm::mat4 V = trackball.view_matrix();
		glm::mat4 M = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		
		if (isSelectWrapper) {
			Uniform1i(program_edit, "isTex", true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wrapping_texture[WrapperIdx].tex);
			UniformMatrix4fv(program_edit, "MVP", 1, GL_FALSE, &VP[0][0]);
			UniformMatrix4fv(program_edit, "V", 1, GL_FALSE, &V[0][0]);
			UniformMatrix4fv(program_edit, "M", 1, GL_FALSE, &M[0][0]);
			Uniform3fv(program_edit, "LightPosition_worldspace", 3, &lightpos.x);

			glDrawArrays(GL_TRIANGLES, 0, VAO_edit.count);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else {
			glBindVertexArray(0);
			glUseProgram(0);
			render_box();
			//Uniform1i(program_edit, "isTex", false);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//UniformMatrix4fv(program_edit, "MVP", 1, GL_FALSE, &VP[0][0]);
			//glDrawArrays(GL_TRIANGLES, 0, VAO_edit.count);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}
	
	void render_select() {
		glUseProgram(program_select);
		glBindVertexArray(VAO_select);
		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < 5; i++) {
			glBindTexture(GL_TEXTURE_2D, wrapping_texture[i].tex);
			Uniform1i(program_select, "tex", 0);
			glDrawArrays(GL_TRIANGLES, i*6, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_side() {
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(program_side);
		glBindVertexArray(VAO_side);

		UniformMatrix4fv(program_side, "MVP", 1, GL_FALSE, &VP[0][0]);

		for (int i = 0; i < 36; i += 6) {
			if (i % 36 == side_count) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i, 6);
			}
		}

		glBindVertexArray(0);
		glUseProgram(0);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void render_arrow() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(program_arrow);
		glBindVertexArray(VAO_arrow);

		UniformMatrix4fv(program_arrow, "MVP", 1, GL_FALSE, &VP[0][0]);

		for (int i = 0; i < 108; i += 18) {
			if ((i / 3) % 36 != side_count) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i, 18);
			}
		}
		
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_direction() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(program_direction);
		glBindVertexArray(VAO_direction);

		UniformMatrix4fv(program_direction, "MVP", 1, GL_FALSE, &VP[0][0]);

		for (int i = 0; i < 108; i += 18) {
			if ((i / 3) % 36 == side_count) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawArrays(GL_TRIANGLES, i, 18);
			}
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void render_arrow_line() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(program_arrow_line);
		glBindVertexArray(VAO_arrow_line);

		UniformMatrix4fv(program_arrow_line, "MVP", 1, GL_FALSE, &VP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 72);

		glBindVertexArray(0);
		glUseProgram(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void render_main(std::vector<interface_info>& interfaces) {
		glDisable(GL_DEPTH_TEST);
		glm::mat4 T = { 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		for (int i = 0; i < interfaces.size(); i++) {
			if (interfaces[i].icon_tex.tex != NULL) {
				glUseProgram(interfaces[i].program);
				glBindVertexArray(interfaces[i].VAO_icon);
				Uniform1i(interfaces[i].program, "isMain", true);
				Uniform1i(interfaces[i].program, "onCursor", false);

				glBindTexture(GL_TEXTURE_2D, interfaces[i].icon_tex.tex);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindVertexArray(0);
				glUseProgram(0);
			}

			if (i == 0) {
				float sx, sy;
				sx = 3.5 / float(width); sy = 3.5 / float(height);
				tr.RenderText(interfaces[i].text, interfaces[i].location.x, interfaces[i].location.y, sx, sy, T, interfaces[i].type);
			}
			else {
				glUseProgram(interfaces[i].program);
				glBindVertexArray(interfaces[i].VAO);
				glActiveTexture(GL_TEXTURE0);

				Uniform1i(interfaces[i].program, "isMain", true);
				Uniform1i(interfaces[i].program, "onCursor", interfaces[i].onCursor);

				glBindTexture(GL_TEXTURE_2D, interfaces[i].texture.tex);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindTexture(GL_TEXTURE_2D, 0);

				glBindVertexArray(0);
				glUseProgram(0);

				float sx, sy;
				sx = 1 / float(width); sy = 1 / float(height);

				tr.RenderText(interfaces[i].text, interfaces[i].location.x - interfaces[i].width / 2 + 0.02, 
					interfaces[i].location.y - interfaces[i].height / 2 + 0.02, sx, sy, T, interfaces[i].type);
			}

			
		}
		glEnable(GL_DEPTH_TEST);
	}


	void render_interface(std::vector<interface_info>& interfaces) {
		glDisable(GL_DEPTH_TEST);
		static int f = 0;
		for (int i = 0; i < interfaces.size(); i++) {
			if (interfaces[i].id == -2) {
				if (!transform_side_mode) {
					continue;
				}
			}
			
			
			glm::vec3 temp;
			switch (state)
			{
			case UserData::InterfaceState::Hide:
				f = 0;
				if(interfaces[i].type != 4)
					continue;
				break;
			case UserData::InterfaceState::Entering:
				f++;
				if (f >= 20)
					state = IS::Entered;
				break;
			case UserData::InterfaceState::Entered:
				f = 20;
				break;
			case UserData::InterfaceState::Exiting:
				f--;
				if (f <= 0)
					state = IS::Hide;
				break;
			default:
				break;
			}
			temp = { 0.4f - 0.4f * f / 20.f, 0.f, 0.f };
			T = glm::translate(temp);
			glm::mat4 u = { 1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
			if (interfaces[i].icon_tex.tex != NULL) {
				glUseProgram(interfaces[i].program);
				glBindVertexArray(interfaces[i].VAO_icon);
				Uniform1i(interfaces[i].program, "isMain", false);
				Uniform1i(interfaces[i].program, "onCursor", false);

				if(interfaces[i].type != 4)
					UniformMatrix4fv(interfaces[i].program, "T", 1, GL_FALSE, &T[0][0]);
				else
					UniformMatrix4fv(interfaces[i].program, "T", 1, GL_FALSE, &u[0][0]);
				glBindTexture(GL_TEXTURE_2D, interfaces[i].icon_tex.tex);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindVertexArray(0);
				glUseProgram(0);
			}

			glUseProgram(interfaces[i].program);
			glBindVertexArray(interfaces[i].VAO);
			glActiveTexture(GL_TEXTURE0);
			
			Uniform1i(interfaces[i].program, "isMain", false);
			Uniform1i(interfaces[i].program, "onCursor", interfaces[i].onCursor);
			
			if(interfaces[i].type != 4)
				UniformMatrix4fv(interfaces[i].program, "T", 1, GL_FALSE, &T[0][0]);
			else
				UniformMatrix4fv(interfaces[i].program, "T", 1, GL_FALSE, &u[0][0]);

			if (interfaces[i].id == 2)
				Uniform1i(interfaces[i].program, "rbox", true);
			else
				Uniform1i(interfaces[i].program, "rbox", false);

			glBindTexture(GL_TEXTURE_2D, interfaces[i].texture.tex);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
			glUseProgram(0);

			float sx, sy;
			sx = 1 / float(width); sy = 1 / float(height);

			if(interfaces[i].type != 4)
				tr.RenderText(interfaces[i].text, interfaces[i].location.x - interfaces[i].width / 2 + 0.02, 
				interfaces[i].location.y - interfaces[i].height / 2 + 0.02, sx, sy, T, interfaces[i].type);
			else
				tr.RenderText(interfaces[i].text, interfaces[i].location.x - interfaces[i].width / 2 + 0.02,
					interfaces[i].location.y - interfaces[i].height / 2 + 0.02, sx, sy, u, interfaces[i].type);

		}


		glEnable(GL_DEPTH_TEST);
	}

	void render_text() {
		float sx, sy;
		sx = 1 / float(width); sy = 1 / float(height);
	
		tr.RenderText("Hello", -1 + 8 * sx, 1 - 50 * sy, sx, sy);

	}


	void cleanup() override {
		Cleanup_FindSurface();
		Cleanup_RealSense();
		Cleanup_OpenGL();
		Cleanup_Interface();
	}

	void onWindowSize(GLFWwindow* window, int width, int height) override {
		glViewport(0, 0, width, height);
		this->width = width;
		this->height = height;
	}

	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override {

		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE: 
				glfwSetWindowShouldClose(window, GLFW_TRUE); 
				break;

			case GLFW_KEY_0:
			case GLFW_KEY_1:
				//Findplane = !Findplane;
				break;
			case GLFW_KEY_2:
				//editmode = !editmode;
				//Run_edit();
				break;
			case GLFW_KEY_3:
				Save_box();
				break;
			case GLFW_KEY_4:
				Load_box();
				break;
			case GLFW_KEY_5:
			case GLFW_KEY_6:
			case GLFW_KEY_7:
			case GLFW_KEY_8:
			case GLFW_KEY_9:
				printf("%d key is pressed.\n", key - GLFW_KEY_0);
				break;

			case GLFW_KEY_SPACE: 
				printf("space key is pressed.\n");
				//if (!Findbox)
				//	Run_FindBox();
				//else
				//	Findbox = false;
				break;

			case GLFW_KEY_A:
			case GLFW_KEY_B:
			case GLFW_KEY_C:
			case GLFW_KEY_D:
			//case GLFW_KEY_E:
			case GLFW_KEY_F:
			case GLFW_KEY_G:
			case GLFW_KEY_H:
				//Recommend_Box();
				break;
			case GLFW_KEY_I:
			case GLFW_KEY_J:
			case GLFW_KEY_K:
			case GLFW_KEY_L:
			case GLFW_KEY_M:
			case GLFW_KEY_N:
			case GLFW_KEY_O:
			case GLFW_KEY_P:
			//case GLFW_KEY_Q:
			//
			case GLFW_KEY_S:
				//printf("size control mode on!\n");
				//transform_side_mode = !transform_side_mode;
				break;
			case GLFW_KEY_T:
				//if (!select_mode) {
				//	printf("Wrapping paper select mode on\n");
				//	select_mode = !select_mode;
				//}
				//else {
				//	printf("Wrapping paper select mode off\n");
				//	select_mode = !select_mode;
				//}
				break;
			case GLFW_KEY_U:
			case GLFW_KEY_V:
			//case GLFW_KEY_W:
			case GLFW_KEY_X:
			case GLFW_KEY_Y:
			case GLFW_KEY_Z:
				printf("%c key is pressed.\n", static_cast<char>(key - GLFW_KEY_A + 'A'));
				break;
			
			case GLFW_KEY_Q: //behavior = camera::Trackball::Behavior::ROTATING; break;
			case GLFW_KEY_W: //behavior = camera::Trackball::Behavior::PANNING; break;
			case GLFW_KEY_E: //behavior = camera::Trackball::Behavior::ZOOMING; break;
				break;
			case GLFW_KEY_R: trackball.reset(); break;
			
			case GLFW_KEY_UP:
				//if (editmode) {
				//	side_count += 6;
				//	if (side_count == 36) {
				//		side_count = 0;
				//	}
				//}
				//break;
			case GLFW_KEY_RIGHT:
				//if (editmode) {
				//	Size_up();
				//}
				break;
			case GLFW_KEY_LEFT:
				//if (editmode) {
				//	Size_down();
				//}
				break;
			case GLFW_KEY_F1:
				//if (editmode) {
				//	unit_cm += (float)0.01;
				//	printf("사이즈 조절 단위가 변경되었습니다. 현재 단위 : %dcm\n", (int)(unit_cm * 100));
				//}
				break;
			case GLFW_KEY_F2:
				//if (editmode) {
				//	if (unit_cm > 0.01) {
				//		unit_cm -= (float)0.01;
				//		printf("사이즈 조절 단위가 변경되었습니다. 현재 단위 : %dcm\n", (int)(unit_cm * 100));
				//	}
				//	else {
				//		printf("더 이상 사이즈 조절 단위를 줄일 수 없습니다.");
				//	}
				//}
				break;
			}
		}
	}

	void onCursorPos(GLFWwindow* window, double x, double y) override {
		trackball.motion(float(x) / width, float(y) / height);
		x = x / width; y = y / height;
		float fx, fy;
		fx = x - 0.5; fy = y - 0.5;
		fx *= 2; fy *= 2;
		fy = -fy;

		if (main_mode) {
			for (int i = 0; i < main_interface.size(); i++) {
				main_interface[i].onCursor = false;
			}

			int a = checkClickButton(main_interface, fx, fy);
			if (a >= 0) {
				if(main_interface[a].type != 0) {
					main_interface[a].onCursor = true;
				}
			}

		}
		else if (camera_mode) {
			for (int i = 0; i < camera_interface.size(); i++) {
				camera_interface[i].onCursor = false;
			}
			int a = checkClickButton(camera_interface, fx, fy);
			if (a >= 0) {
				if (camera_interface[a].type != 0) {
					camera_interface[a].onCursor = true;
				}
			}
		}
		else if (edit_mode) {
			for (int i = 0; i < edit_interface.size(); i++) {
				edit_interface[i].onCursor = false;
			}
			int a = checkClickButton(edit_interface, fx, fy);
			if (a >= 0) {
				if (edit_interface[a].type != 0) {
					edit_interface[a].onCursor = true;
				}
			}
		}
		
		/////버튼위치에 올라갈경우 테두리 변경 및 투명도 변경
	}

	void onMouseButton(GLFWwindow* window, int button, int action, int mods) override {
		double dx, dy; glfwGetCursorPos(window, &dx, &dy);
		float x = float(dx / width);
		float y = float(dy / height);
		float fx, fy; 
		//마우스 커서 클릭위치 좌표를 실제 윈도우 좌표로 조정
		fx = x - 0.5; fy = y - 0.5;
		fx *= 2; fy *= 2;
		fy = -fy;
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_PRESS) {
				trackball.mouse(x, y, behavior);
				int index = -1;

				if (main_mode) {
					index = checkClickButton(main_interface, fx, fy);
					switch (index) {
					case 0:
						break;
					case 1:
						camera_mode = true;
						edit_mode = false;
						main_mode = false;
						break;
					case 2:
						if (edit_enable) {
							camera_mode = false;
							edit_mode = true;
							main_mode = false;
						}
						break;
					case 3:

					case -1: break;
					}


				}
				else if (camera_mode) {
					if (state == IS::Entered) {
						index = checkClickButton(camera_interface, fx, fy);
						switch (index) {
						case 0://background
							break;
						case 1://Find box
							//glfwSetCursor(window, plane_cursor);
							//find_plane = true;
							floor_idx = Find_Floor();
							Run_FindSurface(floor_idx, depth_points[floor_idx].z);
							Run_FindBox();
							break;
						case 2:
							Findbox = !Findbox;
							break;
						case 3://back
							camera_mode = false;
							edit_mode = false;
							main_mode = true;
							Findbox = false;
							cleanMouseBehavior();
							break;
						case 4://rotation
							if (!rotation_mode) {
								cleanMouseBehavior();
								rotation_mode = true;
								glfwSetCursor(window, rotate_cursor);
								behavior = camera::Trackball::Behavior::ROTATING;
							}
							else {
								cleanMouseBehavior();
							}
							break;
						case 5://zoom
							if (!zoom_mode) {
								cleanMouseBehavior();
								zoom_mode = true;
								glfwSetCursor(window, zoom_cursor);
								behavior = camera::Trackball::Behavior::ZOOMING;
							}
							else {
								cleanMouseBehavior();
							}
							break;
						case 6://panning
							if (!panning_mode) {
								cleanMouseBehavior();
								panning_mode = true;
								glfwSetCursor(window, panning_cursor);
								behavior = camera::Trackball::Behavior::PANNING;
							}
							else {
								cleanMouseBehavior();
							}
							break;
						case -1: break;
						}
					}
					//if (index == -1 && find_plane) {
					//	ray r = trackball.generate_ray(x, y);
					//	unsigned int index = 0;
					//	intersect_points(reinterpret_cast<std::vector<glm::vec3>&>(depth_points), r, index);
					//	float depth = glm::distance(reinterpret_cast<glm::vec3&>(depth_points[index]), r.org);
					//
					//	Run_FindSurface(index, depth);
					//	find_plane = false;
					//	glfwSetCursor(window, std_cursor);
					//}
				}
				else if (edit_mode && state == IS::Entered) {
					index = checkClickButton(edit_interface, fx, fy);
					if (texture_change_mode) {
						if (checkClickButton(texture_interface, fx, fy) >= 0) {
							WrapperIdx = checkClickButton(texture_interface, fx, fy);
						}
						if (WrapperIdx == 3 && isSelectWrapper) {
							
							isSelectWrapper = false;
						}
						else if (WrapperIdx != -1 && WrapperIdx != 3 && !isSelectWrapper) {
							isSelectWrapper = !isSelectWrapper;
						}
					}

					if (transform_side_mode) {
						if (checkClickButton(unit_interface, fx, fy) >= 0) {
							selected_arrow = checkClickButton(unit_interface, fx, fy);
						}
						if (selected_arrow == 1) {
							//unit_cm += (float)0.01;
							//s_unit = std::to_string((int)(unit_cm*100)) + "cm";
							//unit_interface[4].text = s_unit.c_str();
							//printf("사이즈 조절 단위가 변경되었습니다. 현재 단위 : %dcm\n", (int)(unit_cm * 100));
						}
						else if (selected_arrow == 0) {
							if (unit_cm > 0.01) {
								//unit_cm -= (float)0.01;
								//s_unit = std::to_string((int)(unit_cm*100)) + "cm";
								//unit_interface[4].text = s_unit.c_str();
								//printf("사이즈 조절 단위가 변경되었습니다. 현재 단위 : %dcm\n", (int)(unit_cm * 100));
							}
							else {
								//printf("더 이상 사이즈 조절 단위를 줄일 수 없습니다.");
							}
						}
						else if (selected_arrow == 2) {
							if (transform_side_mode) {
								Size_up();
								box_vert = glm::distance(box_points[1], box_points[2]);
								box_hori = glm::distance(box_points[2], box_points[8]);
								box_height = glm::distance(box_points[1], box_points[0]);

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
						}
						else if (selected_arrow == 3) {
							if (transform_side_mode) {
								Size_down();
								box_vert = glm::distance(box_points[1], box_points[2]);
								box_hori = glm::distance(box_points[2], box_points[8]);
								box_height = glm::distance(box_points[1], box_points[0]);

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
						}
						else if (selected_arrow == 4) {
							if (transform_side_mode) {
								side_count += 6;
								if (side_count == 36) {
									side_count = 0;
								}
							}
						}
						selected_arrow = -1;
					}
					std::vector<glm::vec3> box_edge_pos;
					switch (index) {
					case 0:
						break;
					case 1://wrapping
						if (!texture_change_mode) {
							printf("texture change mode on!\n");
						}
						else if (texture_change_mode) {
							printf("texture change mode off!\n");
						}
						texture_change_mode = !texture_change_mode;
						break;
					case 2://size control
						if (!transform_side_mode) {
							printf("size control mode on!\n");
						}
						else if (transform_side_mode) {
							printf("size control mode off!\n");
						}
						transform_side_mode = !transform_side_mode;
						break;
					case 3://resume
						box_points = box_points_org;
						box_vert = glm::distance(box_points[1], box_points[2]);
						box_hori = glm::distance(box_points[2], box_points[8]);
						box_height = glm::distance(box_points[1], box_points[0]);

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

						VertexBufferData(VAO_box, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
						VertexBufferData(VAO_edit, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);
						VertexBufferData(VAO_side, VBO_POS, box_points.size(), sizeof(glm::vec3), box_points.data(), GL_STATIC_DRAW);

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

						break;
					case 4://back
						camera_mode = false;
						edit_mode = false;
						main_mode = true;
						//Findbox = false;
						cleanMouseBehavior();
						break;
					case 5://rotation
						if (!rotation_mode) {
							cleanMouseBehavior();
							rotation_mode = true;
							glfwSetCursor(window, rotate_cursor);
							behavior = camera::Trackball::Behavior::ROTATING;
						}
						else {
							cleanMouseBehavior();
						}
						break;
					case 6://zoom
						if (!zoom_mode) {
							cleanMouseBehavior();
							zoom_mode = true;
							glfwSetCursor(window, zoom_cursor);
							behavior = camera::Trackball::Behavior::ZOOMING;
						}
						else {
							cleanMouseBehavior();
						}
						break;
					case 7://panning
						if (!panning_mode) {
							cleanMouseBehavior();
							panning_mode = true;
							glfwSetCursor(window, panning_cursor);
							behavior = camera::Trackball::Behavior::PANNING;
						}
						else {
							cleanMouseBehavior();
						}
						break;
					}

				}
			}
			else {
				if (action == GLFW_RELEASE) {
					trackball.mouse(x, y, camera::Trackball::Behavior::NOTHING);
				}
			}


			//if (!select_mode) {
			//	if (action == GLFW_PRESS) {
			//		trackball.mouse(x, y, behavior);
			//	}
			//	else if (action == GLFW_RELEASE) {
			//		trackball.mouse(x, y, camera::Trackball::Behavior::NOTHING);
			//	}
			//}
			//else {
			//	if (action == GLFW_PRESS) {
			//		float fx, fy; //마우스 커서 클릭위치 좌표를 실제 윈도우 좌표로 조정
			//		fx = x - 0.5; fy = y - 0.5;
			//		fx *= 2; fy *= 2;
			//		fy = -fy;
			//		for (int i = 0; i < select_pos.size(); i+=6) {
			//			if (select_pos.at(i).x <= fx && fx <= select_pos.at(i + 2).x && select_pos.at(i + 3).y <= fy && fy <= select_pos.at(i).y) {
			//				Run_selectPaper(i/6+1);
			//				break;
			//			}
			//		}
			//	}
			//}


			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_PRESS) {
				if (camera_mode || edit_mode) {
					switch (state)
					{
					case UserData::InterfaceState::Hide:
						state = IS::Entering;
						break;
					case UserData::InterfaceState::Entering:
						state = IS::Exiting;
						break;
					case UserData::InterfaceState::Entered:
						state = IS::Exiting;
						break;
					case UserData::InterfaceState::Exiting:
						state = IS::Entering;
						break;
					default:
						break;
					}

				}
					
					//view_interface = !view_interface;
			}


			//if (action == GLFW_PRESS) { //find plane
			//	ray r = trackball.generate_ray(x, y);
			//	unsigned int index = 0;
			//	intersect_points(reinterpret_cast<std::vector<glm::vec3>&>(depth_points), r, index);
			//	float depth = glm::distance(reinterpret_cast<glm::vec3&>(depth_points[index]), r.org);
			//	
			//	Run_FindSurface(index, depth);
			//
			break;
		}
	}

	void onScroll(GLFWwindow* window, double xoffset, double yoffset) override {

	}
};

int main()
{
	App app;

	try {
		app.Init(1024, 768, "Box Packaging AR View");
		app.Run();
	}
	catch (std::runtime_error& e) {
		fprintf(stderr, "%s\n", e.what());
	}
	catch (std::exception& e) {
		fprintf(stderr, "%s\n", e.what());
	}

    return 0;
}

