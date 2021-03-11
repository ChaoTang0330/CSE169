////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Project 5";

// Objects to render
//Cube * Window::cube;
Skeleton* Window::skel = nullptr;
Skin* Window::skin = nullptr;
bool Window::drawSkelFlag = false;
Player* Window::player = nullptr;
Cloth* Window::cloth = nullptr;
ParticleSystem* Window::particleSys = nullptr;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;
GLuint Window::particleShaderProgram;

// File name
std::string Window::skelFileName = "";
std::string Window::skinFileName = "";
std::string Window::animFileName = "";

//gui
std::vector<std::string> Window::jointsName;
std::vector<glm::vec3> Window::jointsRot;
std::vector<glm::vec3> Window::jointsRotMin;
std::vector<glm::vec3> Window::jointsRotMax;

// FPS count
clock_t Window::deltaT = 0;
clock_t Window::prevT;
int Window::countFPS = 0;
int Window::currFPS = 0;

////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	particleShaderProgram = LoadShaders("shaders/shader_particle.vert", "shaders/shader_particle.frag");
	if (!particleShaderProgram)
	{
		std::cerr << "Failed to initialize particle shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// Create a cube
	//cube = new Cube();
	//cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

	if (!skelFileName.empty())
	{
		skel = new Skeleton(skelFileName, shaderProgram);

		int numOfJoint = skel->getNumOfJoint();
		jointsRot.resize(numOfJoint);
		jointsRotMin.resize(numOfJoint);
		jointsRotMax.resize(numOfJoint);
		for (int i = 0; i < numOfJoint; i++)
		{
			std::string jointName = skel->getJointName(i);
			jointsName.push_back(jointName);
		}
	}
	
	if (!skinFileName.empty())
	{
		skin = new Skin(skinFileName, skel, shaderProgram);

		if (!animFileName.empty())
		{
			Animation* anim = new Animation(animFileName);
			player = new Player(skel, anim);
		}
	}

	//cloth = new Cloth(
	//	4.0f, // size 
	//	100.0, //mass 
	//	25, // N particles
	//	glm::vec3(-2.0f, 3.0f, 0.0f), //topleft
	//	glm::vec3(1.0f, 0.0f, 0.0f), // horizontal
	//	glm::vec3(0.0f, -1.0f, 0.0f), // vertical
	//	shaderProgram);

	particleSys = new ParticleSystem(particleShaderProgram, shaderProgram);

	prevT = clock();

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	//delete cube;
	if (skel) delete skel;
	if (skin) delete skin;
	if (cloth) delete cloth;
	if (particleSys) delete particleSys;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
	glDeleteProgram(particleShaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwSetWindowPos(window, 700, 300);

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	if (height == 0.0f) return;
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	// Perform any updates as necessary. 
	Cam->Update();

	//cube->update();
	if (player != nullptr) player->update();
	else if (skel != nullptr)skel->update(glm::mat4(1.0));

	if (skin != nullptr)skin->update();

	if (cloth != nullptr) cloth->Update();

	if (particleSys != nullptr) particleSys->Update();
	
	//updateRot();
}

void Window::displayCallback(GLFWwindow* window)
{	
	//FPS
	clock_t currT = clock();
	deltaT += currT - prevT;
	prevT = currT;
	countFPS++;
	if (deltaT >= 1000)
	{
		currFPS = countFPS;
		countFPS = 0;
		deltaT -= 1000;
	}

	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	//cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	
	if (skin != nullptr)
	{
		skin->draw(Cam->GetViewProjectMtx());
	}
	if ((skin == nullptr || drawSkelFlag) && skel != nullptr)
	{
		skel->draw(Cam->GetViewProjectMtx());
	}

	if (cloth != nullptr)
	{
		cloth->Draw(Cam->GetViewProjectMtx());
	}

	if (particleSys != nullptr)
	{
		particleSys->Draw(Cam->GetViewProjectMtx());
	}


	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();

	plotImGUI();

	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;

		case GLFW_KEY_S:
			drawSkelFlag ^= true;
			break;

		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
void Window::plotImGUI()
{
	//start imgui new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	//gui window
	ImGui::Begin("Coefficients", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(Window::width - 260, 0));
	ImGui::SetWindowSize(ImVec2(260, Window::height));

	std::string fps = "FPS: " + std::to_string(currFPS);
	ImGui::Text(fps.c_str());

	if (ImGui::TreeNode("Particle"))
	{
		float r = particleSys->GetRadius();
		if (ImGui::DragFloat("Radius", &r))
		{
			particleSys->SetRadius(r);
		}

		float m = particleSys->GetMass();
		if (ImGui::DragFloat("Mass", &m))
		{
			particleSys->SetMass(m);
		}

		float g = particleSys->GetGravity();
		if (ImGui::DragFloat("g", &g))
		{
			particleSys->SetGravity(g);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Particle Creation"))
	{
		float rate = particleSys->GetCreationRate();
		if (ImGui::DragFloat("Rate", &rate))
		{
			particleSys->SetCreationRate(rate);
		}

		float lifeSpan = particleSys->GetInitialLifeSpan();
		if (ImGui::DragFloat("Life", &lifeSpan))
		{
			particleSys->SetInitialLifeSpan(lifeSpan);
		}

		float lifeSpanVar = particleSys->GetInitialLifeSpanVar();
		if (ImGui::DragFloat("Life Var", &lifeSpanVar))
		{
			particleSys->SetInitialLifeSpanVar(lifeSpanVar);
		}

		glm::vec3 pos = particleSys->GetInitialPos();
		if (ImGui::InputFloat3("Pos", glm::value_ptr(pos)))
		{
			particleSys->SetInitialPos(pos);
		}

		glm::vec3 posVar = particleSys->GetInitialPosVar();
		if (ImGui::InputFloat3("Pos Var", glm::value_ptr(posVar)))
		{
			particleSys->SetInitialPosVar(posVar);
		}

		glm::vec3 v = particleSys->GetInitialVelocity();
		if (ImGui::InputFloat3("V", glm::value_ptr(v)))
		{
			particleSys->SetInitialVelocity(v);
		}

		glm::vec3 v_var = particleSys->GetInitialVelocityVar();
		if (ImGui::InputFloat3("V_var", glm::value_ptr(v_var)))
		{
			particleSys->SetInitialVelocityVar(v_var);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Aerodynamic Force"))
	{
		float rho = particleSys->GetAirDensity();
		if (ImGui::DragFloat("rho", &rho))
		{
			particleSys->SetAirDensity(rho);
		}

		float C_d = particleSys->GetDragConst();
		if (ImGui::DragFloat("C_d", &C_d))
		{
			particleSys->SetDragConst(C_d);
		}

		glm::vec3 V_wind = particleSys->GetWindSpeed();
		if (ImGui::InputFloat3("V_wind", glm::value_ptr(V_wind)))
		{
			particleSys->SetWindSpeed(V_wind);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Ground"))
	{
		glm::vec3 center = particleSys->GetGroundCenter();
		if (ImGui::InputFloat3("V_wind", glm::value_ptr(center)))
		{
			particleSys->SetGroundCenter(center);
		}

		float size = particleSys->GetGroundSize();
		if (ImGui::DragFloat("size", &size))
		{
			particleSys->SetGroundSize(size);
		}

		float epsilon = particleSys->GetElasticity();
		if (ImGui::DragFloat("Elasticity", &epsilon))
		{
			particleSys->SetElasticity(epsilon);
		}

		float mu = particleSys->GetFriction();
		if (ImGui::DragFloat("Friction", &mu))
		{
			particleSys->SetFriction(mu);
		}

		ImGui::TreePop();
	}

	ImGui::End();

	//draw imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//project 1,2,3
//void Window::plotImGUI()
//{
//	//start imgui new frame
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//	//bool show_demo_window = true;
//	//ImGui::ShowDemoWindow(&show_demo_window);
//
//	ImGuiWindowFlags window_flags = 0;
//	window_flags |= ImGuiWindowFlags_NoTitleBar;
//	window_flags |= ImGuiWindowFlags_NoResize;
//	//gui window
//	ImGui::Begin("DOFs", NULL, window_flags);
//	ImGui::SetWindowPos(ImVec2(Window::width - 190, 0));
//	ImGui::SetWindowSize(ImVec2(190, Window::height));
//
//	int numOfJoint = 0;
//	if(skel != nullptr) numOfJoint = skel->getNumOfJoint();
//
//	for (int i = 0; i < numOfJoint; i++)
//	{
//		if (ImGui::TreeNode(jointsName[i].c_str()))
//		{
//			glm::vec3 rot, rotMin, rotMax;
//			skel->getJointRot(i, rot);
//			skel->getJointRotMin(i, rotMin);
//			skel->getJointRotMax(i, rotMax);
//
//			if (ImGui::DragFloat("RotX", &rot.x,
//				0.005f,
//				rotMin.x,
//				rotMax.x,
//				"%.3f",
//				ImGuiSliderFlags_AlwaysClamp)
//				||
//				ImGui::DragFloat("RotY", &rot.y,
//					0.005f,
//					rotMin.y,
//					rotMax.y,
//					"%.3f",
//					ImGuiSliderFlags_AlwaysClamp)
//				||
//				ImGui::DragFloat("RotZ", &rot.z,
//					0.005f,
//					rotMin.z,
//					rotMax.z,
//					"%.3f",
//					ImGuiSliderFlags_AlwaysClamp)
//				)
//			{
//				skel->setJointRot(i, rot);
//			}
//			
//
//			ImGui::TreePop();
//		}
//	}
//
//	ImGui::End();
//
//	//draw imgui
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}


//project4
//void Window::plotImGUI()
//{
//	//start imgui new frame
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//	//bool show_demo_window = true;
//	//ImGui::ShowDemoWindow(&show_demo_window);
//
//	ImGuiWindowFlags window_flags = 0;
//	window_flags |= ImGuiWindowFlags_NoTitleBar;
//	window_flags |= ImGuiWindowFlags_NoResize;
//	//gui window
//	ImGui::Begin("Coefficients", NULL, window_flags);
//	ImGui::SetWindowPos(ImVec2(Window::width - 260, 0));
//	ImGui::SetWindowSize(ImVec2(260, Window::height));
//
//	std::string fps = "FPS: " + std::to_string(cloth->GetFPS());
//	ImGui::Text(fps.c_str());
//
//	if (ImGui::TreeNode("Fixed Points"))
//	{
//		float posShift = 0.05f;
//		if (ImGui::Button("\t+\t###XP"))
//		{
//			cloth->TranslateFixedParticles(0, posShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###XM"))
//		{
//			cloth->TranslateFixedParticles(0, -posShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Axis X");
//
//		if (ImGui::Button("\t+\t###YP"))
//		{
//			cloth->TranslateFixedParticles(1, posShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###YM"))
//		{
//			cloth->TranslateFixedParticles(1, -posShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Axis Y");
//
//		if (ImGui::Button("\t+\t###ZP"))
//		{
//			cloth->TranslateFixedParticles(2, posShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###ZM"))
//		{
//			cloth->TranslateFixedParticles(2, -posShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Axis Z");
//
//		float rotShift = 0.02f;
//		if (ImGui::Button("\t+\t###RXP"))
//		{
//			cloth->RotateFixedParticles(0, rotShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###RXM"))
//		{
//			cloth->RotateFixedParticles(0, -rotShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Rot X");
//
//		if (ImGui::Button("\t+\t###RYP"))
//		{
//			cloth->RotateFixedParticles(1, rotShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###RYM"))
//		{
//			cloth->RotateFixedParticles(1, -rotShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Rot Y");
//
//		if (ImGui::Button("\t+\t###RZP"))
//		{
//			cloth->RotateFixedParticles(2, rotShift);
//		}
//		ImGui::SameLine();
//		if (ImGui::Button("\t-\t###RZM"))
//		{
//			cloth->RotateFixedParticles(2, -rotShift);
//		}
//		ImGui::SameLine();
//		ImGui::Text("Rot Z");
//
//		ImGui::NewLine();
//
//		int numOfFixedPoints = cloth->GetFixedParticleNum();
//		for (int i = 0; i < numOfFixedPoints; i++)
//		{
//			glm::vec3 pos = cloth->GetFixedParticlePos(i);
//			std::string name = "Pos " + std::to_string(i);
//			if (ImGui::InputFloat3(name.c_str(), glm::value_ptr(pos)))
//			{
//				cloth->SetFixedParticlePos(i, pos);
//			}
//		}
//		ImGui::TreePop();
//	}
//
//	if (ImGui::TreeNode("Cloth Coefficients"))
//	{
//		float clothTotalMass = cloth->GetMass();
//		if (ImGui::DragFloat("Mass", &clothTotalMass))
//		{
//			cloth->SetMass(abs(clothTotalMass));
//		}
//
//		float g = cloth->GetGravityAcce();
//		if (ImGui::DragFloat("g", &g))
//		{
//			cloth->SetGravityAcce(abs(g));
//		}
//
//		float groundPos = cloth->GetGroundPos();
//		if (ImGui::DragFloat("Ground", &groundPos))
//		{
//			cloth->SetGroundPos(groundPos);
//		}
//
//		ImGui::TreePop();
//	}
//
//	if (ImGui::TreeNode("SpringDamper Coefficients"))
//	{
//		float k_spring = cloth->GetSpringConst();
//		if (ImGui::DragFloat("Spring Constant", &k_spring))
//		{
//			cloth->SetSpringConst(abs(k_spring));
//		}
//
//		float k_damper = cloth->GetDampingConst();
//		if (ImGui::DragFloat("Damping Constant", &k_damper))
//		{
//			cloth->SetDampingConst(abs(k_damper));
//		}
//
//		ImGui::TreePop();
//	}
//
//	if (ImGui::TreeNode("Aerodynamic Coefficients"))
//	{
//		glm::vec3 windVelocity = cloth->GetWindVelocity();
//		if (ImGui::InputFloat3("V_wind", glm::value_ptr(windVelocity)))
//		{
//			cloth->SetWindVelocity(windVelocity);
//			/*std::cout << "Wind, x: " << windVelocity.x
//				<< ", y: " << windVelocity.y
//				<< ", z: " << windVelocity.z << std::endl;*/
//		}
//
//
//		float rho = cloth->GetFluidDensity();
//		if (ImGui::DragFloat("rho", &rho))
//		{
//			cloth->SetFluidDensity(abs(rho));
//		}
//
//		float C_d = cloth->GetDragConst();
//		if (ImGui::DragFloat("C_d", &C_d))
//		{
//			cloth->SetDragConst(abs(C_d));
//			//std::cout << "C_d: " << C_d << std::endl;
//		}
//
//		ImGui::TreePop();
//	}
//
//	ImGui::End();
//
//	//draw imgui
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}

////////////////////////////////////////////////////////////////////////////////
