#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>
// using namespace std;
using namespace glm;

int WIDTH = 1000;
int HEIGHT = 1000;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
float deltaTime;
float lastFrame = 0.0f;
bool firstMouse = true;
// int mainWindow;
// 鼠标移动
double maxUpAngle = 89.9f, minUpangle = -89.9f;//0.0f;
double xCPI = 0.02, yCPI = 0.02;
double _yaw = DEFAULT_YAW, _pitch = DEFAULT_PITCH;
// 鼠标滚动
double fov = DEFAULT_FOV;
double scrSEN = 0.4;
double minFOV = 0.5f, maxFOV = 16.0f;

double headPosX = DEFAULT_HEAD_POS_X;
double headPosZ = DEFAULT_HEAD_POS_Z;
double moveStep = 0.002;
double height = DEFAULT_HEIGHT;
const float gravity = -0.0002;

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
	light->setTranslation(glm::vec3(0.0, 10.0, 2.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	// light->setAttenuation(1.0, 0.045, 0.0075); // 衰减系数

	TriMesh* cube = new TriMesh();
	cube->readOff("./assets/off/cube.off");
	painter->addMesh(cube, "cube_object", "", vshader, fshader); //编号为0
	meshList.push_back(cube);
	
	TriMesh* plane = new TriMesh();
	plane->generateSquare(vec3(0.0118, 0.4, 0.302));
	painter->addMesh(plane, "plane_object", "", vshader, fshader); //编号为1的平面
	meshList.push_back(plane);
	glClearColor(0.3, 0.4, 0.5, 1.0);
	// glClearColor(1.0, 1.0, 1.0, 1.0);
	// glClearColor(0.0, 0.0, 0.0, 1.0);
}
// 绘制天空盒
void initSkyBox(std::string vshader, std::string fshader) {
	vec3 Black(0,0,0);
	TriMesh* top = new TriMesh();
	top->setNormalize(false);
	top->generateSquare(Black);
	top->setTranslation(glm::vec3(0.0, 20.0, 0.0));
	top->setScale(glm::vec3(40.0, 40.0, 40.0));
	top->setRotation(glm::vec3(-90, 0, 0.0));
	painter->addMesh(top, "top", "./assets/skybox2/top.jpg", vshader, fshader);
	meshList.push_back(top);

	TriMesh* front = new TriMesh();
	front->setNormalize(false);
	front->generateSquare(Black);
	front->setTranslation(glm::vec3(0.0, 0.0, -20.0));
	front->setScale(glm::vec3(40.0, 40.0, 40.0));
	front->setRotation(glm::vec3(0.0, 180.0, 0.0));
	painter->addMesh(front, "front", "./assets/skybox2/front.jpg", vshader, fshader);
	meshList.push_back(front);

	TriMesh* back = new TriMesh();
	back->setNormalize(false);
	back->generateSquare(Black);
	back->setTranslation(glm::vec3(0.0, 0.0, 20.0));
	back->setScale(glm::vec3(40.0, 40.0, 40.0));
	back->setRotation(glm::vec3(0.0, 0.0, 0.0));
	painter->addMesh(back, "back", "./assets/skybox2/back.jpg", vshader, fshader);
	meshList.push_back(back);

	TriMesh* left = new TriMesh();
	left->setNormalize(false);
	left->generateSquare(Black);
	left->setTranslation(glm::vec3(-20.0, 0.0, 0.0));
	left->setScale(glm::vec3(40.0, 40.0, 40.0));
	left->setRotation(glm::vec3(0.0, -90.0, 0.0));
	painter->addMesh(left, "back", "./assets/skybox2/left.jpg", vshader, fshader);
	meshList.push_back(left);

	TriMesh* right = new TriMesh();
	right->setNormalize(false);
	right->generateSquare(Black);
	right->setTranslation(glm::vec3(20.0, 0.0, 0.0));
	right->setScale(glm::vec3(40.0, 40.0, 40.0));
	right->setRotation(glm::vec3(0.0, 90.0, 0.0));
	painter->addMesh(right, "right", "./assets/skybox2/right.jpg", vshader, fshader);
	meshList.push_back(right);

	TriMesh* bottom = new TriMesh();
	bottom->setNormalize(false);
	bottom->generateSquare(Black);
	bottom->setTranslation(glm::vec3(0.0, -20.0, 0.0));
	bottom->setScale(glm::vec3(40.0, 40.0, 40.0));
	bottom->setRotation(glm::vec3(-90.0, 0.0, 0.0));
	painter->addMesh(bottom, "bottom", "./assets/skybox2/bottom.jpg", vshader, fshader);
	meshList.push_back(bottom);
	
}
// 绘制human
float fbas = 0.28;
struct Robot
{
	// 关节大小
	float TORSO_HEIGHT = fbas * 3; //躯干高度
	float TORSO_WIDTH = fbas * 2; //躯干宽度
	float UPPER_ARM_HEIGHT = fbas * 3; // 左臂高度
	float UPPER_ARM_WIDTH =  fbas; // 左臂宽度
	float UPPER_LEG_HEIGHT = fbas * 3; // 右臂高度
	float UPPER_LEG_WIDTH =  fbas; // 右臂宽度
	float HEAD_HEIGHT = fbas * 2; //头的高度
	float HEAD_WIDTH = fbas * 2; //头的宽度

	// 关节角和菜单选项值
	enum {
		Torso,			// 躯干
		Head,			// 头部
		RightUpperArm,	// 右大臂
		LeftUpperArm,	// 左大臂
		RightUpperLeg,	// 右大腿
		LeftUpperLeg,	// 左大腿
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0,    // Torso
		0.0,    // Head
		0.0,    // RightUpperArm
		0.0,    // LeftUpperArm
		0.0,    // RightUpperLeg
		0.0,    // LeftUpperLeg
	};
}robot;
class MatrixStack {
	int		_index;
    int		_size;
    glm::mat4*	_matrices;

public:
	MatrixStack(int numMatrices = 100):_index(0), _size(numMatrices)
        { _matrices = new glm::mat4[numMatrices]; }

    ~MatrixStack()
		{ delete[]_matrices; }

    void push(const glm::mat4& m){
		assert( _index + 1 < _size );
		_matrices[_index++] = m;	 
    }

	glm::mat4& pop(){
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};
int idhuman[100], cnthuman;
float movhuman[100] = {0.53, 0.23, 0.23};
void inithuman() {
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	//身体
	TriMesh *body = new TriMesh();
	// body->setNormalize(false);
	body->readObj("./assets/human/body.obj");
	painter->addMesh(body, "body", "./assets/human/body.png", vshader, fshader);
	meshList.push_back(body);
	idhuman[cnthuman++] = meshList.size() - 1;
	//头
	TriMesh *head = new TriMesh();
	// head->setNormalize(false); //先除2
	head->readObj("./assets/human/head.obj");
	painter->addMesh(head, "head", "./assets/human/head.png", vshader, fshader);
	meshList.push_back(head);
	idhuman[cnthuman++] = meshList.size() - 1;
	//左手
	TriMesh *lefthand = new TriMesh();
	// lefthand->setNormalize(false);
	lefthand->readObj("./assets/human/lefthand.obj");
	painter->addMesh(lefthand, "lefthand", "./assets/human/lefthand.png", vshader, fshader);
	meshList.push_back(lefthand);
	idhuman[cnthuman++] = meshList.size() - 1;
	//右手
	TriMesh *righthand = new TriMesh();
	// righthand->setNormalize(false);
	righthand->readObj("./assets/human/righthand.obj");
	painter->addMesh(righthand, "righthand", "./assets/human/righthand.png", vshader, fshader);
	meshList.push_back(righthand);
	idhuman[cnthuman++] = meshList.size() - 1;
	//左腿
	TriMesh *leftleg = new TriMesh();
	// leftleg->setNormalize(false);
	leftleg->readObj("./assets/human/leftleg.obj");
	painter->addMesh(leftleg, "leftleg", "./assets/human/leftleg.png", vshader, fshader);
	meshList.push_back(leftleg);
	idhuman[cnthuman++] = meshList.size() - 1;
	//右腿
	TriMesh *rightleg = new TriMesh();
	// rightleg->setNormalize(false);
	rightleg->readObj("./assets/human/rightleg.obj");
	painter->addMesh(rightleg, "rightleg", "./assets/human/rightleg.png", vshader, fshader);
	meshList.push_back(rightleg);
	idhuman[cnthuman++] = meshList.size() - 1;
	
}
void torso(glm::mat4 modelMatrix) { //躯干
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.TORSO_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.TORSO_WIDTH, robot.TORSO_HEIGHT, robot.TORSO_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Torso], modelMatrix * instance, light, camera, true);
}
// 头部
void head(glm::mat4 modelMatrix) {
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.HEAD_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.HEAD_WIDTH, robot.HEAD_HEIGHT, robot.HEAD_WIDTH));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Head], modelMatrix * instance, light, camera, true);
}
// 左大臂
void left_upper_arm(glm::mat4 modelMatrix) {
    // 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.UPPER_ARM_WIDTH, robot.UPPER_ARM_HEIGHT, robot.UPPER_ARM_WIDTH));
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.LeftUpperArm], modelMatrix * instance, light, camera, true);	
}
// 右大臂
void right_upper_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.UPPER_ARM_WIDTH, robot.UPPER_ARM_HEIGHT, robot.UPPER_ARM_WIDTH));
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.RightUpperArm], modelMatrix * instance, light, camera, true);
}
// 左大腿
void left_upper_leg(glm::mat4 modelMatrix) {
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.UPPER_LEG_WIDTH, robot.UPPER_LEG_HEIGHT, robot.UPPER_LEG_WIDTH));
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.LeftUpperLeg], modelMatrix * instance, light, camera, true);

}
// 右大腿
void right_upper_leg(glm::mat4 modelMatrix) {
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));
	
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	
	// instance = glm::scale(instance, glm::vec3(robot.UPPER_LEG_WIDTH, robot.UPPER_LEG_HEIGHT, robot.UPPER_LEG_WIDTH));
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.RightUpperLeg], modelMatrix * instance, light, camera, true);
}


TriMesh *bullet[1000]; //最多1000发子弹
vec3 mov[1000], bas[1000];
int indexbullet[1000];
float ang[1000];
bool vis[1000]; //判断是否绘制
int cntbullet = 0, cntplanet = 0;
std::vector <int> idcolorplane;
std::vector <vec2> pcolorplane; //(x, z)
void generatebullet() {
	if (cntbullet >= 1000) return;
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	int now = cntbullet++;
	mov[now] = bas[now] = vec3(headPosX, gravity, headPosZ);
	ang[now] = _yaw;
	bullet[now] = new TriMesh();
	bullet[now]->readOff("./assets/off/sphere.off");
	std::string name = "bullet" + now;
	painter->addMesh(bullet[now], name, "", vshader, fshader);
	meshList.push_back(bullet[now]);
	indexbullet[now] = meshList.size() - 1;
}
void generatebulletplane(vec2 center) {
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	int now = cntplanet++;
	TriMesh* plane = new TriMesh();
	plane->generateSquare(vec3(0.2471, 0.0157, 0.6196));
	std::string name = "plane_object" + now;
	painter->addMesh(plane, name, "", vshader, fshader); //编号为1的平面
	meshList.push_back(plane);
	idcolorplane.push_back(meshList.size() - 1);
	pcolorplane.push_back(center);
}
void drawbullet() {
	mat4 modelView = mat4(1.0);
	for (int i = 0; i < cntbullet; i++) {
		if (mov[i].y + 0.4 < 0) {
			if (!vis[i]) {
				vis[i] = true;
				generatebulletplane(vec2(mov[i].x, mov[i].z));
			}
			continue;
		}
		modelView = mat4(1.0);
		modelView = translate(modelView, vec3(0.0, 0.4, 0.0));
		modelView = translate(modelView, mov[i]);
		modelView = scale(modelView, vec3(0.2));
		mov[i] = mov[i] + vec3(-0.003 * sin(radians(ang[i])), bas[i].y, -0.003 * cos(radians(ang[i])));
		painter->drawMesh(indexbullet[i], modelView, light, camera, true); //true设置是否绘制阴影		
	}
}
void drawbulletplane() {
	mat4 modelView = mat4(1.0);
	for (int i = 0; i < cntplanet; i++) {
		modelView = mat4(1.0);
		modelView = translate(modelView, vec3(pcolorplane[i].x, -0.002, pcolorplane[i].y)); //
		// modelView = translate(modelView, vec3(0, -0.002, 0)); //
		modelView = rotate(modelView, float(radians(ang[i])), vec3(0.0, 1.0, 0.0));
		modelView = rotate(modelView, float(radians(90.0f)), vec3(1.0, 0.0, 0.0));
		
		// modelView = translate(modelView, vec3(pcolorplane[i].x, 0, pcolorplane[i].y));
		painter->drawMesh(idcolorplane[i], modelView, light, camera, false);
	}
}
void drawshootmodel() {
	mat4 modelView = mat4(1.0);
	modelView = translate(modelView, vec3(0.0, 0.23, 0.0));
	modelView = translate(modelView, vec3(headPosX, height, headPosZ));
	modelView = rotate(modelView, float(radians(_yaw)), vec3(0, 1, 0));
	painter->drawMesh(0, modelView, light, camera, true); //true设置是否绘制阴影
}
void drawplanemodel() {
	mat4 modelView = mat4(1.0);
	modelView = translate(modelView, vec3(0.0, -0.005, 0.0)); //
	modelView = rotate(modelView, float(radians(90.0f)), vec3(1.0, 0.0, 0.0));
	modelView = scale(modelView, vec3(30.0));
	painter->drawMesh(1, modelView, light, camera, false);
}
void drawhuman() {
	mat4 modelMatrix = mat4(1.0);
	// 保持变换矩阵的栈
	MatrixStack mstack;
    // 躯干（这里我们希望机器人的躯干只绕Y轴旋转，所以只计算了RotateY）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(headPosX, 0.5, headPosZ));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Torso]), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::scale(modelMatrix, vec3(0.5));
	torso(modelMatrix);
	mstack.push(modelMatrix); // 保存躯干变换矩阵
    // 头部（这里我们希望机器人的头部只绕Y轴旋转，所以只计算了RotateY）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, robot.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Head]), glm::vec3(0.0, 1.0, 0.0));
	head(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵

    // =========== 左臂 ===========
	mstack.push(modelMatrix);   // 保存躯干变换矩阵
    // 左大臂（这里我们希望机器人的左大臂只绕X轴旋转，所以只计算了RotateX，后面同理）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5 * robot.TORSO_WIDTH - 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	left_upper_arm(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
    // =========== 右臂 ===========
	mstack.push(modelMatrix);   // 保存躯干变换矩阵
	// 右大臂（这里我们希望机器人的右大臂只绕X轴旋转，所以只计算了RotateX，后面同理）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	right_upper_arm(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
	// =========== 左腿 ===========
	mstack.push(modelMatrix);   // 保存躯干变换矩阵
    // @TODO: 左大腿
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_LEG_WIDTH, 0, 0.0));
	// std::cout<<-0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_LEG_WIDTH;
	// modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	left_upper_leg(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
 	// @TODO: 右大腿
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5 * robot.TORSO_WIDTH - 0.5 * robot.UPPER_LEG_WIDTH, 0, 0.0));
	// modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	right_upper_leg(modelMatrix);
}
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawhuman();
	// drawshootmodel();

	// drawplanemodel();
	// drawbulletplane();
	// painter->drawMeshes(light, camera, 2, 7);
	// if (cntbullet) drawbullet();
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

// 键盘响应函数 TODO!!!!!!!!!!!!!!!!!!!!!!!!
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		generatebullet();
	}
}
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

	robot.theta[robot.Torso] = _yaw; //设置躯干的转角
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
	deltaTime = 1;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		headPosX -= sin(radians(_yaw)) * moveStep;
		headPosZ -= cos(radians(_yaw)) * moveStep;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		headPosX += sin(radians(_yaw)) * moveStep;
		headPosZ += cos(radians(_yaw)) * moveStep;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		headPosX -= sin(radians(_yaw + 90)) * moveStep;
		headPosZ -= cos(radians(_yaw + 90)) * moveStep;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		headPosX += sin(radians(_yaw + 90)) * moveStep;
		headPosZ += cos(radians(_yaw + 90)) * moveStep;
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
float pre_time = 0;
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
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
	initSkyBox("./shaders/skyvshader.glsl", "./shaders/skyfshader.glsl");
	inithuman();
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);

	// generatebullet();
	// int s = 0;
	while (!glfwWindowShouldClose(window))
	{
	    float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		// s++;
		// std::cout << deltaTime << std::endl;
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
