//context
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//imgui-> GUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//tini dialog -> Import/export
#include "tinyfiledialogs.h"
#include "nfd.h"
#include "ImGuiFileDialog.h"

//filesystem, movement and import
#include <learnopengl/filesystem.h>


//system
#include <stdio.h>
#include <Commdlg.h>
#include <windows.h>
#include "resource.h"
#include <iostream>
//settings and camera
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include "program_settings.h"
#include "static_geometry.h"
#include "mesh_decimator.h"

// ------------------------------------------------------------------
//								DECLARATIONS
// ------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

//------------------------------------------------------------------
// settings & VARIABLES
//------------------------------------------------------------------
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::mat4 view;
glm::mat4 projection;
//ui
bool inUI = true;
bool cameraMode = false;
bool lockOn = false;
bool fileOpened = false;
bool isInWireframe = false;
bool isOpeningFile = false;
bool isSavingFile = false;

//bool show_demo_window = true;
static bool show_app_metrics = false;
static bool show_app_style_editor = false;
static bool show_settings_window = true;
static bool show_modelSettings_window = false;
static bool show_lightSettings_window = false;
static bool show_decimateSettings_window = true;
static bool show_help_window = true;
static bool show_app_about = false;

ImVec4 clear_color = ImVec4(0.24f, 0.4f, 0.9f, 1.0f);
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//model
Model currentModel;
Model outputModel;

glm::vec3 importModelPos(0.0f, 0.0f, 0.0f);
glm::vec3 modelZeroPos(0.0f, 0.0f, 0.0f);
glm::vec3 objectColor(1.2f, 2.0f, 2.0f);
// lighting
glm::vec3 lightZeroPos(1.2f, 2.0f, 2.0f);
glm::vec3 lightPos(1.2f, 2.0f, 2.0f);

//context
GLFWwindow* window;

//Mesh Decimation
float decimatePercentage = 0.75f;
float specularStrength = 0.5f;

// ------------------------------------------------------------------
//								METHODS
// ------------------------------------------------------------------
//contextCreation
int createGLFWWindow()
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
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tool & Plugin: Mesh Decimator", NULL, NULL);
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 0;
}

//for program start, default objects for cmd load
std::string getLoadedFile()
{
	int cmdFileSelect;


	while (true)
	{
		std::cout << "Please select which file you would like to import and press Enter" << std::endl << "write 1 for Planet" << std::endl << "write 2 for nanosuit" << std::endl << "write 3 for rock " << std::endl << "write 4 for tiger " << std::endl;
		std::cout << "Waiting for input..." << std::endl;
		//std::cin >> move;
		std::cin >> cmdFileSelect;

		switch (cmdFileSelect)
		{
		case 1:
		{
			std::cout << "Selected 1: planet" << std::endl;
			return "resources/objects/noTexture/planet.obj";
		}
		case 2:
		{
			std::cout << "Selected 2: Nanosuit" << std::endl;
			return  "resources/objects/noTexture/nanosuit.obj";
		}
		case 3:
		{
			std::cout << "Selected: 3: rock" << std::endl;
			return "resources/objects/noTexture/rock.obj";
		}
		case 4:
		{
			std::cout << "Selected: 4: tiger.off" << std::endl;
			return "resources/objects/noTexture/tiger.off";
		}
		default:
		{
			std::cout << "Please enter a valid number!" << std::endl;
		}
		}
	}

	return "";
}
//ui content: buttons,sliders,..
void startMyGui()
{
	//open new object
	if (isOpeningFile)
	{
		if (ImGuiFileDialog::Instance()->FileDialog("Choose File", ".obj\0.off\0\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				ProgramSettings::ImportedModelPath = ImGuiFileDialog::Instance()->GetFilepathName();

				std::cout << "openButton ok and model " << ProgramSettings::ImportedModelPath << " has been imported!" << endl;

				currentModel = Model(ProgramSettings::ImportedModelPath);
				MeshDecimator::setInputModel(&currentModel, &outputModel);
			}

			isOpeningFile = false;
		}
	}

	if (isSavingFile)
	{
		if (ImGuiFileDialog::Instance()->FileDialog("Choose File", ".obj\0.off\0\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				ProgramSettings::exportedModelPath = ImGuiFileDialog::Instance()->GetFilepathName();

				std::cout << "model" << ProgramSettings::exportedModelPath << " has been exported!" << endl;
				outputModel.setPath(ProgramSettings::exportedModelPath);
				//outputModel = Model(ProgramSettings::exportedModelPath);
				//MeshDecimator::setInputModel(&currentModel, &outputModel);
			}

			isSavingFile = false;
		}
	}

	//all gui windows
	if (show_settings_window)
	{
		ImGui::Begin("Global Settings", &show_settings_window);
		{


			ImGui::Text("Change Background Color");                           // Display some text (you can use a format string too)

			ImGui::ColorEdit3("Background Color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::Checkbox("Show Mesh Decimation Settings Window", &show_decimateSettings_window);
			ImGui::Checkbox("Show Model Settings Window", &show_modelSettings_window);
			ImGui::Checkbox("Show Light Settings Window", &show_lightSettings_window);
			ImGui::Checkbox("Show Metrics Window", &show_app_metrics);
			ImGui::Checkbox("is OpeningFile bool", &isOpeningFile);
			ImGui::Checkbox("Show Help Window", &show_help_window);
			ImGui::Checkbox("Wireframe", &isInWireframe);
			if (isInWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			ImGui::Checkbox("Camera Mode", &cameraMode);
			if (cameraMode) {
				inUI = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {

				inUI = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			ImGui::Checkbox("Show About Window", &show_app_about);
			ImGui::Checkbox("lock on camera", &lockOn);
			if(lockOn)
			{				
				view = camera.lockOnModel(importModelPos);
			}
			
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}

	if (show_modelSettings_window)
	{
		ImGui::Begin("Model Settings", &show_modelSettings_window);
		{

			ImGui::Text("Change Object Position");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("X-Axis", &importModelPos.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Y-Axis", &importModelPos.y, -2.0f, 2.0f);
			ImGui::SliderFloat("Z-Axis", &importModelPos.z, -2.0f, 2.0f);
			if (ImGui::Button("Reset Position"))
				importModelPos = modelZeroPos;
			ImGui::Text("Change Object Color");
			ImGui::ColorEdit3("Object Color", (float*)&objectColor); // Edit 3 floats representing a color
			ImGui::Text("Change Object lighting Properties");
			ImGui::SliderFloat("SpecularStrength", &specularStrength, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		}
		ImGui::End();
	}

	if (show_decimateSettings_window)
	{
		ImGui::Begin("Decimate Settings", &show_decimateSettings_window);
		{
			//only the first of the buttons works, no matter which you set
			//ImGuiDir downButton = ImGuiDir(3);  3 is down in the enum
			if (ImGui::ArrowButton("Decimate Model:", 3))
			{
				if (decimatePercentage > 0.0f)
					decimatePercentage -= 0.01f;
			}

			//ImGuiDir upButton = ImGuiDir(2);  2 is up in the enum
			if (ImGui::ArrowButton("Decimate Model:", 2))
			{
				//if(decimatePercentage < 1.0f)
				decimatePercentage += 0.01f;
			}

			if (ImGui::SliderFloat("Decimate Percentage", &decimatePercentage, 0.0f, 1.0f))
			{
				//MeshDecimator::setInputModel(&currentModel);
				//currentModel = *MeshDecimator::getDecimatedModel(decimatePercentage);
			}
			ImGui::ProgressBar(decimatePercentage);
			if (ImGui::Button("Model decimation"))
			{
				MeshDecimator::decimate(decimatePercentage);
			}

		}
		ImGui::End();
	}


	if (show_lightSettings_window)
	{
		ImGui::Begin("Light Settings", &show_lightSettings_window);
		{
			ImGui::Text("Change Light Position");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("X-Axis", &lightPos.x, -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Y-Axis", &lightPos.y, -5.0f, 5.0f);
			ImGui::SliderFloat("Z-Axis", &lightPos.z, -5.0f, 5.0f);

			if (ImGui::Button("Reset Position"))
			{
				lightPos = lightZeroPos;
				//model = glm::translate(model, lightPos);
			}
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

	if (show_help_window)
	{
		ImGui::Begin("Help window", &show_help_window, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Commands: ", ImGui::GetVersion());
		ImGui::Separator();
		ImGui::Text("Use WASD to move the camera angle around as in first person");
		ImGui::Text("Press TAB to exit Camera Mode");
		ImGui::Text("Press C to enter camera Mode, mouse controls camera");
		ImGui::Text("Press 0 to enter Wireframe Mode and F to exit");
		ImGui::Text("beware to not have camera Lock on and camera mode simultaneously on");
		ImGui::End();
	}


	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File.."))
		{
			if (ImGui::Button("Open new Object")) {

				isOpeningFile = true;

				if (ProgramSettings::ImportedModelPath.length() > 0)
					ImGui::Text("Chosen model file : %s", ProgramSettings::ImportedModelPath.c_str());
			}
			if (ImGui::MenuItem("Save As..")) 
			{
				isSavingFile = true;
				if (ProgramSettings::exportedModelPath.length() > 0)
					ImGui::Text("Chosen model file : %s", ProgramSettings::exportedModelPath.c_str());
			}
			ImGui::MenuItem("Exit");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::Checkbox("Camera Mode", &cameraMode);
			ImGui::MenuItem("Mesh Decimator", NULL, &show_decimateSettings_window);
			ImGui::MenuItem("Global Settings", NULL, &show_settings_window);
			ImGui::MenuItem("Model Settings", NULL, &show_modelSettings_window);
			ImGui::MenuItem("Light Settings",NULL, &show_lightSettings_window);
			ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
			ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
			ImGui::MenuItem("About Mesh Decimator", NULL, &show_app_about);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Decimate Mesh"))
		{
			ImGui::MenuItem("Mesh Decimator", NULL, &show_decimateSettings_window);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("help"))
		{
			ImGui::MenuItem("Help", NULL, &show_help_window);
			ImGui::MenuItem("About Mesh Decimator", NULL, &show_app_about);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

	}

	ImGui::Render();

}

//context init,version, style, flags
void configGui() 
{
	//binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
														   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
}
//render lamp and import object
void renderStuff(Shader lampShader, Shader importShader) 
{
	// view/projection transformations
	//do they have to be created every frame or is it okay to only change its definition?
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	if(!lockOn)
		view = camera.GetViewMatrix();

	// don't forget to enable shader before setting uniforms
	//draw the lamp object
	lampShader.use();
	lampShader.setMat4("projection", projection);
	lampShader.setMat4("view", view);
	glm::mat4 model;
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
	lampShader.setMat4("model", model);

	StaticGeometry::renderLightCube();
	
	importShader.use();
	importShader.setVec3("objectColor", objectColor);
	importShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	importShader.setVec3("lightPos", lightPos);
	importShader.setVec3("viewPos", camera.Position);
	importShader.setMat4("projection", projection);
	importShader.setMat4("view", view);
	glm::mat4 model3;
	model3 = glm::translate(model3, importModelPos); // translate it down so it's at the center of the scene
	model3 = glm::scale(model3, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	importShader.setMat4("model", model3);
	importShader.setFloat("specularStrength", specularStrength);


	outputModel.Draw(importShader);
}

// ------------------------------------------------------------------
//								MAIN
// ------------------------------------------------------------------
int main()
{
	//cmd program start
	std::string cmdFile = getLoadedFile();
	if (createGLFWWindow() == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	Shader importShader("shaders/2.2.basic_lightingImport.vs", "shaders/2.2.basic_lightingImport.fs");
	Shader lampShader("shaders/2.2.lamp.vs", "shaders/2.2.lamp.fs");

	//lamp object
	StaticGeometry::load();

	//program opens in cmd mode, here we set the default path
	currentModel = Model(FileSystem::getPath(cmdFile));
	MeshDecimator::setInputModel(&currentModel, &outputModel);
	//currentModel.setPath(cmdFile);

	//output in console Opengl version
	fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));

	//Gui Setup
	configGui();

	//---------------------------------------------------------------------------------------------------------
	//				Rendering Loop,  //only close with Esc or X button           
	//---------------------------------------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// input
		processInput(window);

		// render
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
		//render lamp and import object
		renderStuff(lampShader, importShader);

		//my gui settings
		startMyGui();
		
		//next line is the actual render , and it has to be before the buffers swap
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		//Check for inputEvent, update WindowState, call needed function
		glfwPollEvents();

	}
	// ------------------------------------------------------------------
	//		Cleanup: deallocate all resources once they've outlived their purpose
	// ------------------------------------------------------------------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	StaticGeometry::deleteGeometry();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return EXIT_SUCCESS;
}

// ------------------------------------------------------------------
//		KEYBOARD & MOUSE EVENTS
// ------------------------------------------------------------------

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
		cameraMode = false;
		inUI = true;
	}
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_HAND_CURSOR);
	//Press C for Camera Control
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cameraMode = true;
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

	if (!inUI) {
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
