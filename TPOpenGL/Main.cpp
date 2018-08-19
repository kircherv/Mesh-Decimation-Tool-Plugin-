//context
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>


//math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//filesystem movement and import
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

//system
#include <iostream>
//import done in code, no gui yet
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

//------------------------------------------------------------------
// settings
//------------------------------------------------------------------
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


//ui
bool inUI = false;
bool isInWireframe = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//model position
glm::vec3 modelPos(0.0f, -1.75f, 0.0f);
// ------------------------------------------------------------------
//								MAIN
// ------------------------------------------------------------------
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tool & Plugin: Mesh Decimator", NULL, NULL);
	// Check for Valid Context
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		glfwTerminate();
		return EXIT_FAILURE;
	}
		
	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(window);
	//Enable VSync
	//glfwSwapInterval(1);
	//set Viewport size to program size
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//Current Mouse position x,y 
	glfwSetCursorPosCallback(window, mouse_callback);
	//Set Scroll Callback
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("shaders/1.model_loading.vs", "shaders/1.model_loading.fs");

	// load models
	// -----------
	Model ourModel(FileSystem::getPath("resources/objects/rock/rock.obj"));
	//Model ourModel("resources/objects/nanosuit/nanosuit.obj");
	//Model ourModel("resources/objects/rock/rock.obj");

	//output in console Opengl version
	fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));

	
	//---------------------------------------------------------------------------------------------------------
	//		Gui Setup
	//---------------------------------------------------------------------------------------------------------
	//binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	bool show_demo_window = true;
	static bool show_app_metrics = false;
	static bool show_app_style_editor = false;
	static bool show_app_about = false;
	bool show_another_window = false;
	// Setup style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
	ImVec4 clear_color = ImVec4(0.24f, 0.4f, 0.9f, 1.0f);

	//---------------------------------------------------------------------------------------------------------
	//				Rendering Loop,  //only close with Esc or X button           
	//---------------------------------------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

		
		
		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, modelPos); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

	
		
		
		if (show_another_window)
		{
			ImGui::Begin("Settings", &show_another_window);
			{

				static float f = 0.0f;
				static int counter = 0;
				ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
				ImGui::SliderFloat("Model Position: X-Axis", &modelPos.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("Model Position: Y-Axis", &modelPos.y, -2.0f, 2.0f);
				ImGui::SliderFloat("Model Position: Z-Axis", &modelPos.z, -2.0f, 2.0f);
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				ImGui::Checkbox("Demo Window", &show_demo_window);
				ImGui::Text("Windows");
				if (ImGui::Button("Reset Position"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
					model = glm::translate(model, modelPos);

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Checkbox("Wireframe", &isInWireframe);
				if (isInWireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			}
		ImGui::End();
		}

		if (show_app_metrics) { ImGui::ShowMetricsWindow(&show_app_metrics); }
		if (show_app_style_editor) { ImGui::Begin("Style Editor", &show_app_style_editor); ImGui::ShowStyleEditor(); ImGui::End(); }
		if (show_app_about)
		{
			ImGui::Begin("AboutMesh Decimator", &show_app_about, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Mesh Decimator, %s", ImGui::GetVersion());
			ImGui::Separator();
			ImGui::Text("By Valentin Kircher Bautista.");
			ImGui::Text("Mesh Decimator is licensed under the MIT License, see LICENSE for more information.");
			ImGui::End();
		}
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Open File");
				ImGui::MenuItem("Save As..");
				ImGui::MenuItem("Exit");
				//ShowExampleMenuFile();
				ImGui::EndMenu();
			}
		
			if (ImGui::BeginMenu("Help"))
			{
				//ImGui::ShowMetricsWindow();
				ImGui::MenuItem("Settings", NULL, &show_another_window);
				ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
				ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
				ImGui::MenuItem("About Mesh Decimator", NULL, &show_app_about);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		
		}


		//good but too crowded, do not need so many sub buttons
		// 4. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		//if (show_demo_window)
		//{
		//	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		//	ImGui::ShowDemoWindow(&show_demo_window);
		//}

		ImGui::Render();
		//next line is the actual render , and it has to be before the buffers swap
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		//Check for inputEvent, update WindowState, call needed function
		glfwPollEvents();

	}
	// ------------------------------------------------------------------
	//		Cleanup
	// ------------------------------------------------------------------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	
	glfwTerminate();
	return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);


	//Press Tab for mouse Control
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		inUI = true;
	}
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_HAND_CURSOR);
	//Press C for Camera Control
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		inUI = false;
	}
	// Press 0 to show Wireframe
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		isInWireframe = true;
	}
	//Press F to Fill Rectangle
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		isInWireframe = false;
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!inUI)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
