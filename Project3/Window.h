#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "Cube.h"
#include "shader.h"
#include "Camera.h"
#include "skeleton.h"
#include "skin.h"
#include "player.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

////////////////////////////////////////////////////////////////////////////////

class Window
{
public:
	// Window Properties
	static int width;
	static int height;
	static const char* windowTitle;

	// Objects to render
	//static Cube* cube;
	static Skeleton* skel;
	static bool drawSkelFlag;
	static Skin* skin;
	static Player* player;

	// Shader Program 
	static GLuint shaderProgram;

	// Act as Constructors and desctructors 
	static bool initializeProgram();
	static bool initializeObjects();
	static void cleanUp();

	// for the Window
	static GLFWwindow* createWindow(int width, int height);
	static void resizeCallback(GLFWwindow* window, int width, int height);

	// update and draw functions
	static void idleCallback();
	static void displayCallback(GLFWwindow*);

	// helper to reset the camera
	static void resetCamera();

	// callbacks - for interaction
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_callback(GLFWwindow* window, double currX, double currY);

	// set files
	static std::string skelFileName;
	static void setSkelFile(char* name) { skelFileName = name; }
	static std::string skinFileName;
	static void setSkinFile(char* name) { skinFileName = name; }
	static std::string animFileName;
	static void setAnimFile(char* name) { animFileName = name; }

	// GUI
	static void plotImGUI();
	static std::vector<std::string> jointsName;
	static std::vector<glm::vec3> jointsRot;
	static std::vector<glm::vec3> jointsRotMin;
	static std::vector<glm::vec3> jointsRotMax;
};

////////////////////////////////////////////////////////////////////////////////

#endif