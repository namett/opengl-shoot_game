#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>

using namespace glm;

int WIDTH = 600;
int HEIGHT = 600;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
// int mainWindow;
// 鼠标移动
double maxUpAngle = 89.9f, minUpangle = -89.9f;
double xCPI = 0.02, yCPI = 0.02;
double _yaw = DEFAULT_YAW, _pitch = DEFAULT_PITCH;
// 鼠标滚动
double fov = DEFAULT_FOV;
double scrSEN = 0.4;
double minFOV = 0.5f, maxFOV = 16.0f;

double headPosX = DEFAULT_HEAD_POS_X;
double headPosZ = DEFAULT_HEAD_POS_Z;
double moveStep = 0.001;
double height = DEFAULT_HEIGHT;

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();

// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;

TriMesh* cube = new TriMesh();

void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(0.0, 0.0, 2.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	// light->setAttenuation(1.0, 0.045, 0.0075); // 衰减系数

	TriMesh* cube = new TriMesh();
	cube->readOff("./assets/off/cube.off");
	// cube->generateCube(vec3(0.2f));
	// cube->setTranslation(vec3(0.0, 0.25, 0)); 
	// cube->setRotation(vec3(-32, 234, 2));
	// cube->setScale(vec3(1.0, 1.0, 1.0));	
	// cube->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	// cube->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	// cube->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	painter->addMesh(cube, "cube_object", "", vshader, fshader); //编号为0
	// meshList.push_back(cube);
	
	TriMesh* plane = new TriMesh();
	plane->generateSquare(vec3(0.0784, 0.0667, 0.6196));
	// plane->setTranslation(vec3(0.0, -0.1, 0));
	// plane->setRotation(vec3(90, 0, 0));
	// plane->setScale(vec3(10.0, 10.0, 10.0));
	// plane->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	// plane->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	// plane->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	painter->addMesh(plane, "plane_object", "", vshader, fshader); //编号为1的平面
	meshList.push_back(plane);
	glClearColor(0.3, 0.4, 0.5, 1.0);
	// glClearColor(0.0, 0.0, 0.0, 1.0);



	// glClearColor(1.0, 1.0, 1.0, 1.0);
	// glClearColor(0.0, 0.0, 0.0, 1.0);
}


void drawshootmodel() {
	mat4 modelView = mat4(1.0);
	modelView = translate(modelView, vec3(0.25));
	modelView = translate(modelView, vec3(headPosX, height, headPosZ));
	modelView = rotate(modelView, float(radians(_yaw)), vec3(0, 1, 0));
	// modelView = rotate(modelView, float(glfwGetTime()), vec3(0, 1, 0));
	painter->drawMesh(0, modelView, light, camera, true);
}
void drawplanemodel() {
	mat4 modelView = mat4(1.0);
	modelView = rotate(modelView, float(radians(90.0f)), vec3(1.0, 0.0, 0.0));
	modelView = scale(modelView, vec3(5.0));
	painter->drawMesh(1, modelView, light, camera, true);
}
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawshootmodel();
	drawplanemodel();
	// painter->drawMeshes(light, camera);

}


void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the light position (drag)." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;

}

// 键盘响应函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: exit(EXIT_SUCCESS); break;
		case GLFW_KEY_H: printHelp(); break;
		default:
			camera->keyboard(key, action, mode);
			break;
		}
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

	xoffset *= xCPI;
	yoffset *= yCPI;
	_yaw -= xoffset;
	_pitch += yoffset;

	if (_pitch > maxUpAngle)	_pitch = maxUpAngle;
	if (_pitch < minUpangle)	_pitch = minUpangle;

	camera->upAngle = _pitch;
	camera->rotateAngle = _yaw;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (fov >= minFOV && fov <= maxFOV) {
		fov -= yoffset * scrSEN;
		_pitch -= yoffset * yCPI * 20;
	}
		
	fov = max(fov, minFOV);
	fov = min(fov, maxFOV);

	if (_pitch > maxUpAngle)	_pitch = maxUpAngle;
	if (_pitch < minUpangle)	_pitch = minUpangle;

	camera->radius = fov;
	camera->upAngle = _pitch;
}
void processinput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		headPosX -= sin(radians(_yaw)) * moveStep * 1;
		headPosZ -= cos(radians(_yaw)) * moveStep * 1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		headPosX += sin(radians(_yaw)) * moveStep * 1;
		headPosZ += cos(radians(_yaw)) * moveStep * 1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		headPosX -= cos(radians(_yaw)) * moveStep * 1;
		headPosZ -= sin(radians(_yaw)) * moveStep * 1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		headPosX += cos(radians(_yaw)) * moveStep * 1;
		headPosZ += sin(radians(_yaw)) * moveStep * 1;
	} 
	camera->cameraPos.x = headPosX;
	camera->cameraPos.z = headPosZ;
}
// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData() {
	// 释放内存
	
	delete camera;
	camera = NULL;

	delete light;
	light = NULL;

	painter->cleanMeshes();

	delete painter;
	painter = NULL;
	
	for (int i=0; i<meshList.size(); i++) {
		delete meshList[i];
	}
	meshList.clear();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "2019192043_dxj_shootgame", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //捕抓鼠标
	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		processinput(window);
		display();
		//reshape();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanData();


	return 0;
}

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}
