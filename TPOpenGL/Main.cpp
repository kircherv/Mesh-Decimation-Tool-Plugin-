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
bool isRotatingObject = false;
bool isContinousRotatingObject = false;
bool isMovingLight = false;
bool fileOpened = false;
bool isInWireframe = false;
bool isOpeningFile = false;
bool isOpeningNewFile = false;
bool isSavingFile = false;

//bool show_demo_window = true;
static bool show_app_metrics = false;
static bool show_app_style_editor = false;
static bool show_settings_window = true;
static bool show_modelSettings_window = true;
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
float modelSizef = 0.2f;
float modelRotation = 0.0f;
int rotationstate = 0;
glm::vec3 importModelPos(0.0f, 0.0f, 0.0f);
glm::vec3 rotateAxis(0.0f, 1.0f, 0.0f);
glm::vec3 modelZeroPos(0.0f, 0.0f, 0.0f);
glm::vec3 objectColor(0.2f, 0.8f, 0.8f);
// lighting
glm::vec3 lightZeroPos(1.2f, 2.0f, 2.0f);
glm::vec3 lightPos(1.2f, 2.0f, 2.0f);

//context
GLFWwindow* window;

//Mesh Decimation
float decimatePercentage = 0.75f;
//model light properties
float specularStrength = 0.5f;
float ambientStrength = 0.1f;
float diffuseStrength = 0.8f;

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
		std::cout << "Please select which file you would like to import and press Enter" << std::endl << "write 1 for Planet" << std::endl << "write 2 for Rabbit" << std::endl << "write 3 for rock " << std::endl << "write 4 for tiger " << std::endl;
		std::cout << "Waiting for input..." << std::endl;
		//std::cin >> move;
		std::cin >> cmdFileSelect;

		switch (cmdFileSelect)
		{
		case 1:
		{
			std::cout << "Selected 1: planet" << std::endl;
			return "3D Objects/planet.obj";
		}
		case 2:
		{
			std::cout << "Selected 2: Rabbit" << std::endl;
			return  "3D Objects/rabbit.off";
		}
		case 3:
		{
			std::cout << "Selected: 3: rock" << std::endl;
			return "3D Objects/rock.obj";
		}
		case 4:
		{
			std::cout << "Selected: 4: tiger.off" << std::endl;
			return "3D Objects/tiger.off";
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
//Imgui filedialog has an issue with showing all files and filter selection, that is why there are two open buttons
void startMyGui()
{
	//open new object
	if (isOpeningFile)
	{
		//for file Selection: open button1
		if (ImGuiFileDialog::Instance()->FileDialog("Choose File", ".obj\0.off\0\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				ProgramSettings::ImportedModelPath = ImGuiFileDialog::Instance()->GetFilepathName();

				std::string extension = ProgramSettings::ImportedModelPath;
				std::string ext = extension.substr(extension.length() - 4);
				std::cout << "model" << ProgramSettings::ImportedModelPath << " has been imported!" << "extension: " << ext << endl;

				currentModel = Model(ProgramSettings::ImportedModelPath);
				MeshDecimator::setInputModel(&currentModel, &outputModel);
			}

			isOpeningFile = false;
		}
	}


	//open new object
	if (isOpeningNewFile)
	{
		//use next line for all line readIn: open button2
		if (ImGuiFileDialog::Instance()->FileDialog("Choose File", "\0\0.obj\0.off\0\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				ProgramSettings::ImportedModelPath = ImGuiFileDialog::Instance()->GetFilepathName();

				std::string extension = ProgramSettings::ImportedModelPath;
				std::string ext = extension.substr(extension.length() - 4);
				std::cout << "model" << ProgramSettings::ImportedModelPath << " has been imported!" << "extension: " << ext << endl;

				currentModel = Model(ProgramSettings::ImportedModelPath);
				MeshDecimator::setInputModel(&currentModel, &outputModel);
			}

			isOpeningNewFile = false;
		}
	}

	//save as..explorer window
	if (isSavingFile)
	{
		if (ImGuiFileDialog::Instance()->FileDialog("Choose File", "\0\0", ".", ""))
		{
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				ProgramSettings::exportedModelPath = ImGuiFileDialog::Instance()->GetFilepathName();

				std::string extension = ProgramSettings::exportedModelPath;
				std::string ext = extension.substr(extension.length() - 4);
				//	std::cout << "model" << ProgramSettings::exportedModelPath << " has been exported!" << "extension: "  << ext<< endl;
				if (ext == ".obj")
				{
					std::cout << "model" << ProgramSettings::exportedModelPath << " has been exported!" << "extension: " << ext << endl;
					outputModel.exportModelAsObj(ProgramSettings::exportedModelPath);
				}
				else if (ext == ".off")
				{
					std::cout << "model" << ProgramSettings::exportedModelPath << " has been exported!" << "extension: " << ext << endl;
					outputModel.exportModelAsOff(ProgramSettings::exportedModelPath);
				}
				else
				{
					outputModel.exportModelAsOff(ProgramSettings::exportedModelPath);
					std::cout << "ext: " << ext << "is not allowed" << endl;
				}

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
			ImGui::Checkbox("Wireframe Mode", &isInWireframe);
			ImGui::Checkbox("Camera Mode", &cameraMode); ImGui::SameLine();
			ImGui::Text(" | Press TAB to exit Camera Mode"); 
			ImGui::Checkbox("Camera: lock on Model", &lockOn);
			ImGui::Separator();
			ImGui::Checkbox("Show Mesh Decimation Settings Window", &show_decimateSettings_window);
			ImGui::Checkbox("Show Model Settings Window", &show_modelSettings_window);
			ImGui::Checkbox("Show Light Settings Window", &show_lightSettings_window);
			ImGui::Checkbox("Show Metrics Window", &show_app_metrics);
			ImGui::Checkbox("Show Help Window", &show_help_window);
			if (isInWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			
			if (cameraMode) {
				inUI = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {

				inUI = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			
			ImGui::Checkbox("Show About Window", &show_app_about);
			if (lockOn)
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
			ImGui::Text("Object Information: Vertices: %d, Indices: %d, Faces: %d", outputModel.getNumVertices(), outputModel.getNumIndices(), outputModel.getNumFaces());
			ImGui::Separator();
			ImGui::Text("Change Object Size");				
			ImGui::SliderFloat("Model Size", &modelSizef, 0.01f, 10.0f);
			ImGui::NewLine();
			ImGui::Text("Change Object Color");
			ImGui::ColorEdit3("Object Color", (float*)&objectColor); // Edit 3 floats representing a color
			ImGui::NewLine();
			ImGui::Text("Change Object Position");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("Move Object in X-Axis", &importModelPos.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Move Object in Y-Axis", &importModelPos.y, -2.0f, 2.0f);
			ImGui::SliderFloat("Move Object in Z-Axis", &importModelPos.z, -2.0f, 2.0f);
			if (ImGui::Button("Reset Position"))
				importModelPos = modelZeroPos;
			ImGui::NewLine();

			ImGui::Text("Change Object Rotation");
		
			if (ImGui::RadioButton("X Axis", &rotationstate, 0))
				rotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			ImGui::SameLine();
			if (ImGui::RadioButton("Y Axis", &rotationstate, 1))
				rotateAxis = glm::vec3(1.0f, 0.0f, 0.0f);
			ImGui::SameLine();
			if (ImGui::RadioButton("Z Axis", &rotationstate, 2))
				rotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);

			
			ImGui::SliderFloat("Rotate Object", &modelRotation, 0.0f, 6.2f);
			ImGui::Checkbox("Contineuos Object Rotation", &isContinousRotatingObject);
			ImGui::Text("Change Object Lighting Properties");
			ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

		}
		ImGui::End();
	}

	if (show_decimateSettings_window)
	{
		ImGui::Begin("Decimate Settings", &show_decimateSettings_window);
		{

			ImGui::Text("For Mesh Decimation: Use the Slider or Up and Down Arrows ");
			//only the first of the buttons works, no matter which you set
			//ImGuiDir downButton = ImGuiDir(3);  3 is down in the enum
			if (ImGui::ArrowButton("Decimate Model:", 3))
			{
				if (decimatePercentage > 0.01f)
					decimatePercentage -= 0.01f;
				MeshDecimator::decimate(decimatePercentage);
			}


			if (ImGui::SliderFloat("Decimate Percentage", &decimatePercentage, 0.001f, 1.0f))
			{
				MeshDecimator::decimate(decimatePercentage);
				//MeshDecimator::setInputModel(&currentModel);
				//currentModel = *MeshDecimator::getDecimatedModel(decimatePercentage);
			}
			if (ImGui::ArrowButton("Decimate Model:", 2))
			{
				MeshDecimator::decimate(decimatePercentage);
				decimatePercentage = decimatePercentage + 0.01f;
			}
			ImGui::ProgressBar(decimatePercentage);
			//ImGuiDir upButton = ImGuiDir(2);  2 is up in the enum
			

		}
		ImGui::End();
	}


	if (show_lightSettings_window)
	{
		ImGui::Begin("Light Settings", &show_lightSettings_window);
		{
			ImGui::Text("Change Light Position");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("Move Light in X-Axis", &lightPos.x, -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat("Move Light in Y-Axis", &lightPos.y, -5.0f, 5.0f);
			ImGui::SliderFloat("Move Light in Z-Axis", &lightPos.z, -5.0f, 5.0f);


			if (ImGui::Button("Reset Position"))
			{
				lightPos = lightZeroPos;
				//model = glm::translate(model, lightPos);
			}

			ImGui::Checkbox("Continous Lamp Movement", &isMovingLight);
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
		ImGui::Text("Mesh Decimator is licensed under the MIT License"); 
		ImGui::Text("see https ://github.com/kircherv/Mesh-Decimation-Tool-Plugin- for more information.");
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
		ImGui::Text("Beware to not have camera Lock on and camera mode simultaneously on");
		ImGui::End();
	}


	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File.."))
		{
			if (ImGui::Button("Open new Object(allFiles)")) {

				isOpeningNewFile = true;

				if (ProgramSettings::ImportedModelPath.length() > 0)
					ImGui::Text("Chosen model file : %s", ProgramSettings::ImportedModelPath.c_str());
			}

			if (ImGui::Button("Open Object(Select Format)")) {

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
			ImGui::Checkbox("Go in Camera Mode", &cameraMode);
			ImGui::MenuItem("Show Mesh Decimator", NULL, &show_decimateSettings_window);
			ImGui::MenuItem("Show Global Settings", NULL, &show_settings_window);
			ImGui::MenuItem("Show Model Settings", NULL, &show_modelSettings_window);
			ImGui::MenuItem("Show Light Settings", NULL, &show_lightSettings_window);
			ImGui::MenuItem("Show Metrics", NULL, &show_app_metrics);
			ImGui::MenuItem("Show Style Editor", NULL, &show_app_style_editor);
			ImGui::MenuItem("Show About Mesh Decimator", NULL, &show_app_about);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Decimate Mesh"))
		{
			ImGui::MenuItem("Mesh Decimator Tool", NULL, &show_decimateSettings_window);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("Show Help", NULL, &show_help_window);
			ImGui::MenuItem("Show About Mesh Decimator", NULL, &show_app_about);
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
		if (!lockOn)
			view = camera.GetViewMatrix();

		// don't forget to enable shader before setting uniforms
		//draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		glm::mat4 model;
		if (isMovingLight)
		{
			lightPos.x = sin(glfwGetTime());
			lightPos.y = sin(glfwGetTime());
			lightPos.z = sin(glfwGetTime());
		}
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

		if (isContinousRotatingObject)
			model3 = glm::rotate(model3, (float)glfwGetTime()*0.25f, glm::vec3(0.0f, 1.0f, 0.0f));
		model3 = glm::rotate(model3, modelRotation, rotateAxis);
		model3 = glm::scale(model3, glm::vec3(modelSizef));	// it's a bit too big for our scene, so scale it down
		importShader.setMat4("model", model3);
		importShader.setFloat("specularStrength", specularStrength);
		importShader.setFloat("ambientStrength", ambientStrength);
		importShader.setFloat("diffuseStrength", diffuseStrength);


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
		//auto pathqqq = FileSystem::getPath(cmdFile);
		currentModel = Model(cmdFile);
		//how do i make cmdFile a relative path
		//currentModel = Model(cmdFile);
		MeshDecimator::setInputModel(&currentModel, &outputModel);
		//currentModel.setPath(cmdFile);

		//output in console Opengl versio

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
