#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>
using namespace glm;
int WIDTH = 1000;
int HEIGHT = 1000;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
float deltaTime;
float lastFrame = 0.0f;
bool firstMouse = true;
// 鼠标移动
double maxUpAngle = 89.9f, minUpangle = -89.9f; // 0.0f;
double xCPI = 0.02, yCPI = 0.02;
double _yaw = DEFAULT_YAW, _pitch = DEFAULT_PITCH;
// 鼠标滚动
// 观察半径
double fov = DEFAULT_FOV;
double scrSEN = 0.4;
double minFOV = 0.5f, maxFOV = 16.0f;

double headPosX = DEFAULT_HEAD_POS_X;
double headPosZ = DEFAULT_HEAD_POS_Z;
double moveStep = 0.002; // TODO速率
double height = DEFAULT_HEIGHT;
const float gravity = -0.0004; // TODO速率

// human移动
#define W 0
#define S 1
#define A 2
#define D 3
float basS = 0.5; // TODO速率 像这种就应该设置为const
float s[4] = {basS, basS, basS, basS};
bool svis[4];

//跳跃参数
float jump = 0.0, jumpspeed = 0.002;
int cntjump = 0;

//僵尸的移动参数+跳跃参数
vec3 enemypos;
float enemyjump = 0.0, enemyjumpspeed = 0.002;
int enemycntjump = 0;
float enemyx = 0.0, enemyz = 0.0; //后退0.5

// 剑移动参数
int cntsword = 0;
float angsword = 0.0;

Camera *camera = new Camera();
Light *light = new Light();
MeshPainter *painter = new MeshPainter();

mat4 mbullet[1000];
int cntm = 0;
bool vismbullet[1000];
// 切换摄像机视角
bool switch_perspective = true;
// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;

TriMesh *cube = new TriMesh();

void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(0.0, 10.0, 2.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0));  // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0));  // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	// light->setAttenuation(1.0, 0.045, 0.0075); // 衰减系数

	TriMesh *cube = new TriMesh();
	cube->readOff("./assets/off/cube.off");
	painter->addMesh(cube, "cube_object", "", vshader, fshader); //编号为0
	meshList.push_back(cube);

	TriMesh *plane = new TriMesh();
	plane->readObj("./assets/plane.obj");
	// plane->generateSquare(vec3(0.0118, 0.4, 0.302));
	painter->addMesh(plane, "plane_object", "./assets/plane.png", vshader, fshader); //编号为1的平面
	// painter->addMesh(plane, "plane_object", "", vshader, fshader); //编号为1的平面
	meshList.push_back(plane);
	glClearColor(0.3, 0.4, 0.5, 1.0);
}
// 绘制天空盒
void initSkyBox(std::string vshader, std::string fshader)
{
	vec3 Black(0, 0, 0);
	TriMesh *top = new TriMesh();
	top->setNormalize(false);
	top->generateSquare(Black);
	top->setTranslation(glm::vec3(0.0, 20.0, 0.0));
	top->setScale(glm::vec3(40.0, 40.0, 40.0));
	top->setRotation(glm::vec3(-90, 0, 0.0));
	painter->addMesh(top, "top", "./assets/skybox2/top.jpg", vshader, fshader);
	meshList.push_back(top);

	TriMesh *front = new TriMesh();
	front->setNormalize(false);
	front->generateSquare(Black);
	front->setTranslation(glm::vec3(0.0, 0.0, -20.0));
	front->setScale(glm::vec3(40.0, 40.0, 40.0));
	front->setRotation(glm::vec3(0.0, 180.0, 0.0));
	painter->addMesh(front, "front", "./assets/skybox2/front.jpg", vshader, fshader);
	meshList.push_back(front);

	TriMesh *back = new TriMesh();
	back->setNormalize(false);
	back->generateSquare(Black);
	back->setTranslation(glm::vec3(0.0, 0.0, 20.0));
	back->setScale(glm::vec3(40.0, 40.0, 40.0));
	back->setRotation(glm::vec3(0.0, 0.0, 0.0));
	painter->addMesh(back, "back", "./assets/skybox2/back.jpg", vshader, fshader);
	meshList.push_back(back);

	TriMesh *left = new TriMesh();
	left->setNormalize(false);
	left->generateSquare(Black);
	left->setTranslation(glm::vec3(-20.0, 0.0, 0.0));
	left->setScale(glm::vec3(40.0, 40.0, 40.0));
	left->setRotation(glm::vec3(0.0, -90.0, 0.0));
	painter->addMesh(left, "back", "./assets/skybox2/left.jpg", vshader, fshader);
	meshList.push_back(left);

	TriMesh *right = new TriMesh();
	right->setNormalize(false);
	right->generateSquare(Black);
	right->setTranslation(glm::vec3(20.0, 0.0, 0.0));
	right->setScale(glm::vec3(40.0, 40.0, 40.0));
	right->setRotation(glm::vec3(0.0, 90.0, 0.0));
	painter->addMesh(right, "right", "./assets/skybox2/right.jpg", vshader, fshader);
	meshList.push_back(right);

	TriMesh *bottom = new TriMesh();
	bottom->setNormalize(false);
	bottom->generateSquare(Black);
	bottom->setTranslation(glm::vec3(0.0, -20.0, 0.0));
	bottom->setScale(glm::vec3(40.0, 40.0, 40.0));
	bottom->setRotation(glm::vec3(-90.0, 0.0, 0.0));
	painter->addMesh(bottom, "bottom", "./assets/skybox2/bottom.jpg", vshader, fshader);
	meshList.push_back(bottom);
}
// 绘制human的基准值
float fbas = 0.28;
struct Robot
{
	// 关节大小
	float TORSO_HEIGHT = fbas * 3;	   //躯干高度
	float TORSO_WIDTH = fbas * 2;	   //躯干宽度
	float UPPER_ARM_HEIGHT = fbas * 3; // 左臂高度
	float UPPER_ARM_WIDTH = fbas;	   // 左臂宽度
	float UPPER_LEG_HEIGHT = fbas * 3; // 右臂高度
	float UPPER_LEG_WIDTH = fbas;	   // 右臂宽度
	float HEAD_HEIGHT = fbas * 2;	   //头的高度
	float HEAD_WIDTH = fbas * 2;	   //头的宽度
	float TORCH_WIDTH = fbas / 2;
	float TORCH_HEIGHT = fbas * 2.5;
	float SMOKE_WIDTH = fbas * 3 / 4;
	float SMOKE_HEIGHT = fbas / 4;
	float SWORD_HEIGHT = fbas * 2;
	// 关节角和菜单选项值
	enum
	{
		Torso,		   // 躯干
		Head,		   // 头部
		RightUpperArm, // 右大臂
		LeftUpperArm,  // 左大臂
		RightUpperLeg, // 右大腿
		LeftUpperLeg,  // 左大腿
		Torch,		   //火把放在右边大臂上
		Smoke,		   //烟在火把上方
		Sword,
	};

	// 关节角大小
	GLfloat theta[10] = {
		0.0, // Torso
		0.0, // Head
		0.0, // RightUpperArm
		0.0, // LeftUpperArm
		0.0, // RightUpperLeg
		0.0, // LeftUpperLeg
	};
} robot;
// 矩阵栈，有没有可能就是直接用C++的栈？
class MatrixStack
{
	int _index;
	int _size;
	glm::mat4 *_matrices;

public:
	MatrixStack(int numMatrices = 100) : _index(0), _size(numMatrices)
	{
		_matrices = new glm::mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[] _matrices;
	}

	void push(const glm::mat4 &m)
	{
		assert(_index + 1 < _size);
		_matrices[_index++] = m;
	}

	glm::mat4 &pop()
	{
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};
int idhuman[100], cnthuman;
void inithuman()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	//身体
	TriMesh *body = new TriMesh();
	// body->setNormalize(false);
	body->readObj("./assets/human/body.obj");
	// 每个mesh都会有自己的着色器程序，这扩展性来说这没问题，但是对于当前的需求会造成资源的浪费
	painter->addMesh(body, "body", "./assets/human/body.png", vshader, fshader);
	meshList.push_back(body);
	idhuman[cnthuman++] = meshList.size() - 1;
	//头
	TriMesh *head = new TriMesh();
	head->readObj("./assets/human/head.obj");
	painter->addMesh(head, "head", "./assets/human/head.png", vshader, fshader);
	meshList.push_back(head);
	idhuman[cnthuman++] = meshList.size() - 1;
	//左手
	TriMesh *lefthand = new TriMesh();
	lefthand->readObj("./assets/human/lefthand.obj");
	painter->addMesh(lefthand, "lefthand", "./assets/human/lefthand.png", vshader, fshader);
	meshList.push_back(lefthand);
	idhuman[cnthuman++] = meshList.size() - 1;
	//右手
	TriMesh *righthand = new TriMesh();
	righthand->readObj("./assets/human/righthand.obj");
	painter->addMesh(righthand, "righthand", "./assets/human/righthand.png", vshader, fshader);
	meshList.push_back(righthand);
	idhuman[cnthuman++] = meshList.size() - 1;
	//左腿
	TriMesh *leftleg = new TriMesh();
	leftleg->readObj("./assets/human/leftleg.obj");
	painter->addMesh(leftleg, "leftleg", "./assets/human/leftleg.png", vshader, fshader);
	meshList.push_back(leftleg);
	idhuman[cnthuman++] = meshList.size() - 1;
	//右腿
	TriMesh *rightleg = new TriMesh();
	rightleg->readObj("./assets/human/rightleg.obj");
	painter->addMesh(rightleg, "rightleg", "./assets/human/rightleg.png", vshader, fshader);
	meshList.push_back(rightleg);
	idhuman[cnthuman++] = meshList.size() - 1;
	// 火把
	TriMesh *torch = new TriMesh();
	torch->readObj("./assets/human/torch.obj");
	painter->addMesh(torch, "torch", "./assets/human/torch.png", vshader, fshader);
	meshList.push_back(torch);
	idhuman[cnthuman++] = meshList.size() - 1;
	// 烟
	TriMesh *smoke = new TriMesh();
	smoke->readObj("./assets/human/smoke.obj");
	painter->addMesh(smoke, "smoke", "./assets/human/smoke.png", vshader, fshader);
	meshList.push_back(smoke);
	idhuman[cnthuman++] = meshList.size() - 1;
	// 剑
	TriMesh *sword = new TriMesh();
	sword->readObj("./assets/human/sword.obj");
	painter->addMesh(sword, "sword", "./assets/human/sword.png", vshader, fshader);
	meshList.push_back(sword);
	idhuman[cnthuman++] = meshList.size() - 1;
}
void torso(glm::mat4 modelMatrix)
{ //躯干
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.TORSO_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Torso], modelMatrix * instance, light, camera, true);
}
// 头部
void head(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * robot.HEAD_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Head], modelMatrix * instance, light, camera, true);
}
// 左大臂
void left_upper_arm(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_ARM_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

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

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.RightUpperArm], modelMatrix * instance, light, camera, true);
}
// 左大腿
void left_upper_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.LeftUpperLeg], modelMatrix * instance, light, camera, true);
}
// 右大腿
void right_upper_leg(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * robot.UPPER_LEG_HEIGHT, 0.0));

	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.RightUpperLeg], modelMatrix * instance, light, camera, true);
}
// 右火把
void right_torch(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.0, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));

	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Torch], modelMatrix * instance, light, camera, true);
}
// 右烟
float speed_smoke = 0.0005; // TODO速率
float high_smoke = 0;		// TODO速率*次数
void right_smoke(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	high_smoke += speed_smoke; //移动烟
	if (high_smoke >= 0.16)
		high_smoke = 0;
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.1, 0.0, -high_smoke));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(1, 0, 0));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 1, 0));

	instance = glm::scale(instance, glm::vec3(0.5));
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idhuman[robot.Smoke], modelMatrix * instance, light, camera, true);
}
// 左剑
void left_sword(glm::mat4 modelMatrix)
{
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.2, 0.2, -0.4));
	instance = glm::rotate(instance, glm::radians(180.0f), glm::vec3(0, 0, 1));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(0, 1, 0));

	painter->drawMesh(idhuman[robot.Sword], modelMatrix * instance, light, camera, true);
}
// 敌人和英雄都应该写成一个类，而不是放到main中如此调用
struct Enemy
{
	// 关节大小
	float TORSO_HEIGHT = fbas * 3;	//躯干高度
	float TORSO_WIDTH = fbas * 2;	//躯干宽度
	float FOOT_HEIGHT = fbas * 1.5; //脚的高度
	float FOOT_WIDTH = fbas * 2;	//脚的宽度
	float HEAD_HEIGHT = fbas * 2;	//头的高度
	float HEAD_WIDTH = fbas * 2;	//头的宽度
	// 关节角和菜单选项值
	enum
	{
		Torso,		// 躯干
		Head,		// 头部
		FrontFoot,	// 前腿
		BehindFoot, // 后腿
	};
	GLfloat theta[10] = {
		0.0, // Torso
		0.0, // Head
		0.0, // FrontFoot
		0.0, // BehindFoot
	};
	// 碰撞检测参数
	vec3 Torso_ld, Torso_ur;
	vec3 Head_ld, Head_ur;
	vec3 FrontFoot_ld, FrontFoot_ur;
	vec3 BehindFoot_ld, BehindFoot_ur;
	vec3 center;
	glm::mat4 mv[10];
	glm::mat4 ins[10];
	bool visins[10];
} enemy;
// 获得物体的包围盒的左上角以及右上角顶点
void getcollision(vec3 &ld, vec3 &rd, std::vector<vec3> &points)
{
	float max_x = -FLT_MAX;
	float max_y = -FLT_MAX;
	float max_z = -FLT_MAX;
	float min_x = FLT_MAX;
	float min_y = FLT_MAX;
	float min_z = FLT_MAX;
	for (int i = 0; i < points.size(); i++)
	{
		auto &position = points[i];
		if (position.x > max_x)
			max_x = position.x;
		if (position.y > max_y)
			max_y = position.y;
		if (position.z > max_z)
			max_z = position.z;
		if (position.x < min_x)
			min_x = position.x;
		if (position.y < min_y)
			min_y = position.y;
		if (position.z < min_z)
			min_z = position.z;
	}
	ld = glm::vec3(max_x, max_y, max_z);
	rd = glm::vec3(min_x, min_y, min_z);
}
bool checkcollision(vec3 ld, vec3 rd, int id, mat4 &modelview, vec3 sp)
{
	vec4 p = modelview * vec4(sp, 1.0);
	vec4 sl = vec4(ld, 1), sr = vec4(rd, 1);
	mat4 tmp = enemy.mv[id] * enemy.ins[id];
	sl = tmp * sl;
	sr = tmp * sr;
	if (min(sl.x, sr.x) <= p.x && p.x <= max(sl.x, sr.x) && min(sl.y, sr.y) <= p.y &&
		p.y <= max(sl.y, sr.y) && min(sl.z, sr.z) <= p.z && p.z <= max(sl.z, sr.z))
		return true;
	else
		return false;
}
int idenemy[100], cntenemy;
void initenemy()
{
	enemypos = vec3(headPosX + 3, 0, headPosZ + 3);
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	//身体
	TriMesh *body = new TriMesh();
	body->readObj("./assets/enemy/body.obj");
	painter->addMesh(body, "body", "./assets/enemy/body.png", vshader, fshader);
	meshList.push_back(body);
	idenemy[cntenemy++] = meshList.size() - 1;
	getcollision(enemy.Torso_ld, enemy.Torso_ur, body->getVertexPositions());
	//头
	TriMesh *head = new TriMesh();
	// head->setNormalize(false); //先除2
	head->readObj("./assets/enemy/head.obj");
	painter->addMesh(head, "head", "./assets/enemy/head.png", vshader, fshader);
	meshList.push_back(head);
	idenemy[cntenemy++] = meshList.size() - 1; // 这个方式是真丑呀这样弄
	getcollision(enemy.Head_ld, enemy.Head_ur, head->getVertexPositions());
	//前腿
	TriMesh *frontfoot = new TriMesh();
	frontfoot->readObj("./assets/enemy/foot.obj");
	painter->addMesh(frontfoot, "frontfoot", "./assets/enemy/foot.png", vshader, fshader);
	meshList.push_back(frontfoot);
	idenemy[cntenemy++] = meshList.size() - 1;
	getcollision(enemy.FrontFoot_ld, enemy.FrontFoot_ur, frontfoot->getVertexPositions());

	//后腿
	TriMesh *behindfoot = new TriMesh();
	behindfoot->readObj("./assets/enemy/foot.obj");
	painter->addMesh(behindfoot, "behindfoot", "./assets/enemy/foot.png", vshader, fshader);
	meshList.push_back(behindfoot);
	idenemy[cntenemy++] = meshList.size() - 1;
	getcollision(enemy.BehindFoot_ld, enemy.BehindFoot_ur, behindfoot->getVertexPositions());

	enemy.center = enemy.Torso_ld + enemy.Torso_ur;
	enemy.center = vec3(enemy.center.x / 2, enemy.center.y / 2, enemy.center.z / 2); //得到中心点
}
//敌人躯干
void enemy_torso(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * enemy.TORSO_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(90.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(0.0f), glm::vec3(1, 0, 0));
	enemy.ins[enemy.Torso] = instance;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idenemy[enemy.Torso], modelMatrix * instance, light, camera, true);
}
// 敌人头部
void enemy_head(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, 0.5 * enemy.HEAD_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	enemy.ins[enemy.Head] = instance;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idenemy[enemy.Head], modelMatrix * instance, light, camera, true);
}
// 敌人前脚
void enemy_front_foot(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * enemy.FOOT_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	enemy.ins[enemy.FrontFoot] = instance;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idenemy[enemy.FrontFoot], modelMatrix * instance, light, camera, true);
}
// 敌人后脚
void enemy_behind_foot(glm::mat4 modelMatrix)
{
	// 本节点局部变换矩阵
	glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance, glm::vec3(0.0, -0.5 * enemy.FOOT_HEIGHT, 0.0));
	instance = glm::rotate(instance, glm::radians(90.0f), glm::vec3(0, 1, 0));
	instance = glm::rotate(instance, glm::radians(270.0f), glm::vec3(1, 0, 0));
	enemy.ins[enemy.BehindFoot] = instance;
	// 乘以来自父物体的模型变换矩阵，绘制当前物体
	painter->drawMesh(idenemy[enemy.BehindFoot], modelMatrix * instance, light, camera, true);
}

vec3 mov[1000], bas[1000]; //最多1000发子弹
int indexbullet = -1;
float ang[1000];
bool vis[1000]; //判断是否绘制
int cntbullet = 0, cntplanet = 0, cntpos = 0;
int idcolorplane = -1;
std::vector<vec2> pcolorplane; //(x, z)
mat4 ibullet[1000];
TriMesh *bullet;
std::vector<vec3> vpbullet;
std::vector<vec3i> fbullet;
std::vector<vec3i> vtbullet;
// 干啥的呢？？？
void posbullet()
{ // TODO可以减少循环次数 将子弹移到火把位置
	for (; cntpos < cntm; cntpos++)
	{ //枚举那些未被枚举的子弹
		mov[cntpos] = mbullet[cntpos] * vec4(vpbullet[fbullet[0].x], 1.0);
	}
}
vec3 circlepoint;
void generatebullet()
{
	if (cntbullet >= 1000)
		return;
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	int now = cntbullet++;
	posbullet();
	ang[now] = robot.theta[robot.Torso];
	// 子弹高频出现，所以不用每次都重新读取
	if (indexbullet == -1)
	{
		bullet = new TriMesh();
		bullet->readOff("./assets/off/sphere.off");
		vpbullet = bullet->getVertexPositions();
		fbullet = bullet->getFaces();
		vtbullet = bullet->getTexturesindex();
		std::string name = "bullet" + now;
		painter->addMesh(bullet, name, "", vshader, fshader);
		meshList.push_back(bullet);
		indexbullet = meshList.size() - 1;

		float max_x = -FLT_MAX;
		float max_y = -FLT_MAX;
		float max_z = -FLT_MAX;
		float min_x = FLT_MAX;
		float min_y = FLT_MAX;
		float min_z = FLT_MAX;
		for (int i = 0; i < vpbullet.size(); i++)
		{
			auto &position = vpbullet[i];
			if (position.x > max_x)
				max_x = position.x;
			if (position.y > max_y)
				max_y = position.y;
			if (position.z > max_z)
				max_z = position.z;
			if (position.x < min_x)
				min_x = position.x;
			if (position.y < min_y)
				min_y = position.y;
			if (position.z < min_z)
				min_z = position.z;
		}
		circlepoint = vec3((max_x + min_x) / 2.0, (max_y + min_y) / 2.0, (max_z + min_z) / 2.0);
	}
}

void generatebulletplane(vec2 center)
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	int now = cntplanet++;
	// 只绑定一次平面即可，因为溅射平面是固定的所以不需要多次绑定
	if (idcolorplane == -1)
	{
		TriMesh *plane = new TriMesh();
		plane->generateSquare(vec3(0.2471, 0.0157, 0.6196));
		std::string name = "plane_object" + now;
		painter->addMesh(plane, name, "", vshader, fshader);
		meshList.push_back(plane);
		idcolorplane = meshList.size() - 1; //通过这个优化
	}
	pcolorplane.push_back(center);
}

void drawbullet()
{
	mat4 modelView = mat4(1.0);
	for (int i = 0; i < cntbullet; i++)
	{
		// 优化一下，就比如说，如果已经绘制平面了，那么就不放在modelView中了，用set来优化 TODO:
		if (mov[i].y < 0 || vis[i])
		{
			if (!vis[i])
			{
				vis[i] = true;
				// 如果不用set优化，那么一个数据就被存储了两次，一次是在mov中，一次是在pcolorplane中
				generatebulletplane(vec2(mov[i].x, mov[i].z));
			}
			continue;
		}
		modelView = mat4(1.0);
		modelView = translate(modelView, mov[i]);
		modelView = scale(modelView, vec3(0.2));
		if (!vis[i])
		{
			if (checkcollision(enemy.Torso_ld, enemy.Torso_ur, enemy.Torso, modelView, circlepoint) ||
				checkcollision(enemy.Head_ld, enemy.Head_ur, enemy.Head, modelView, circlepoint) ||
				checkcollision(enemy.FrontFoot_ld, enemy.FrontFoot_ur, enemy.FrontFoot, modelView, circlepoint) ||
				checkcollision(enemy.BehindFoot_ld, enemy.BehindFoot_ur, enemy.BehindFoot, modelView, circlepoint))
			{
				vis[i] = true;
				// TODO
				enemycntjump = 2;
				continue;
			}
		}
		// TODO速率 -0.003
		mov[i] = mov[i] + vec3(-0.006 * sin(radians(ang[i])), gravity, -0.006 * cos(radians(ang[i])));
		painter->drawMesh(indexbullet, modelView, light, camera, true); // true设置是否绘制阴影
	}
}
void drawbulletplane()
{
	mat4 modelView = mat4(1.0);
	for (int i = 0; i < cntplanet; i++)
	{
		modelView = mat4(1.0);
		modelView = translate(modelView, vec3(pcolorplane[i].x, -0.002, pcolorplane[i].y)); //
		modelView = rotate(modelView, float(radians(ang[i])), vec3(0.0, 1.0, 0.0));
		modelView = rotate(modelView, float(radians(90.0f)), vec3(1.0, 0.0, 0.0));

		painter->drawMesh(idcolorplane, modelView, light, camera, false);
	}
}
void drawshootmodel()
{
	mat4 modelView = mat4(1.0);
	modelView = translate(modelView, vec3(0.0, 0.23, 0.0));
	modelView = translate(modelView, vec3(headPosX, height, headPosZ));
	modelView = rotate(modelView, float(radians(_yaw)), vec3(0, 1, 0));
	painter->drawMesh(0, modelView, light, camera, true); // true设置是否绘制阴影
}
void drawplanemodel()
{
	mat4 modelView = mat4(1.0);
	modelView = translate(modelView, vec3(0.0, -0.05, 0.0)); //
	modelView = scale(modelView, vec3(30.0, 1, 30.0));
	painter->drawMesh(1, modelView, light, camera, false);
}

void drawhuman()
{
	if (cntsword == 2)
	{
		if (angsword <= 45)
			angsword += basS;
		else
			cntsword = 1;
	}
	if (cntsword == 1)
	{
		if (angsword >= 0)
			angsword -= basS;
		else
			cntsword = 0, angsword = 0.0;
	}
	mat4 modelMatrix = mat4(1.0);
	// 保持变换矩阵的栈
	MatrixStack mstack;
	// 躯干（这里我们希望机器人的躯干只绕Y轴旋转，所以只计算了RotateY）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(headPosX, 0.45 + jump, headPosZ));
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
	mstack.push(modelMatrix); // 保存躯干变换矩阵
							  // 左大臂（这里我们希望机器人的左大臂只绕X轴旋转，所以只计算了RotateX，后面同理）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5 * robot.TORSO_WIDTH - 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperArm] + angsword), glm::vec3(1.0, 0.0, 0.0));
	left_upper_arm(modelMatrix);
	// =========== 左剑 ===========
	mstack.push(modelMatrix); //保存左臂变换矩阵
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -robot.UPPER_ARM_HEIGHT, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Sword]), glm::vec3(1.0, 0.0, 0.0));
	left_sword(modelMatrix);
	modelMatrix = mstack.pop(); //恢复左臂变换矩阵
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
								// =========== 右臂 ===========
	mstack.push(modelMatrix);	// 保存躯干变换矩阵
	// 右大臂（这里我们希望机器人的右大臂只绕X轴旋转，所以只计算了RotateX，后面同理）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_ARM_WIDTH, robot.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	right_upper_arm(modelMatrix);
	mstack.push(modelMatrix); //保存右大臂矩阵
	// =========== 火把 ===========
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1 * robot.TORCH_WIDTH, -robot.TORCH_HEIGHT, -0.5 * robot.TORCH_HEIGHT));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Torch]), glm::vec3(1.0, 0.0, 0.0));
	right_torch(modelMatrix);
	mstack.push(modelMatrix); //保存火把矩阵
	// =========== 烟 ===========
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, -1.0 * robot.SMOKE_HEIGHT - 1.0 * robot.TORCH_HEIGHT));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.Smoke]), glm::vec3(1.0, 0.0, 0.0));

	while (cntm < cntbullet)
		mbullet[cntm] = modelMatrix, cntm++;
	posbullet();
	// mbullet; //
	right_smoke(modelMatrix);
	modelMatrix = mstack.pop();
	modelMatrix = mstack.pop();
	modelMatrix = mstack.pop();
	// =========== 左腿 ===========
	mstack.push(modelMatrix); // 保存躯干变换矩阵
							  // 左大腿
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5 * robot.TORSO_WIDTH + 0.5 * robot.UPPER_LEG_WIDTH, 0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	left_upper_leg(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
								// 右大腿
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5 * robot.TORSO_WIDTH - 0.5 * robot.UPPER_LEG_WIDTH, 0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.RightUpperLeg]), glm::vec3(1.0, 0.0, 0.0));
	right_upper_leg(modelMatrix);
}
float enemyang = 0;
float enemyspeed = 0.0001;
void calenemyang()
{					  //计算敌人的角度
	vec2 unit(0, -1); //初始向量
	vec2 front(headPosX - enemypos.x, headPosZ - enemypos.z);
	float cmul = unit.x * front.x + unit.y * front.y;
	float lenfront = sqrt(front.x * front.x + front.y * front.y);
	enemyang = acos(cmul / lenfront / 1) / (2 * M_PI) * 360;
	float xmul = unit.x * front.y - unit.y * front.x;
	if (xmul > 0)
		enemyang = -enemyang;
	if (lenfront >= 3) //移动到一定距离不动
		enemypos = vec3(enemypos.x + enemyspeed * front.x, enemypos.y, enemypos.z + enemyspeed * front.y);

	// 判断是否后退
	if (enemycntjump == 2)
		enemypos.x -= enemyspeed * 7 * front.x, enemypos.z -= enemyspeed * 7 * front.y;
	else
		enemyx = enemyz = 0;
}
void drawenemy()
{
	mat4 modelMatrix = mat4(1.0);
	// 保持变换矩阵的栈
	MatrixStack mstack;
	// 躯干（这里我们希望机器人的躯干只绕Y轴旋转，所以只计算了RotateY）
	calenemyang();

	if (enemy.theta[enemy.Torso] > enemyang + 0.05)
		enemy.theta[enemy.Torso] -= 0.1;
	if (enemy.theta[enemy.Torso] < enemyang - 0.05)
		enemy.theta[enemy.Torso] += 0.1;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(enemypos.x + enemyx, 0.25 + enemyjump, enemypos.z + enemyz));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(enemy.theta[enemy.Torso]), glm::vec3(0.0, 1.0, 0.0));
	// modelMatrix = glm::rotate(modelMatrix, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::scale(modelMatrix, vec3(0.5));
	enemy.mv[enemy.Torso] = modelMatrix; //存储当前变换矩阵
	enemy_torso(modelMatrix);
	mstack.push(modelMatrix); // 保存躯干变换矩阵
							  // 头部（这里我们希望机器人的头部只绕Y轴旋转，所以只计算了RotateY）
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, enemy.TORSO_HEIGHT, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(enemy.theta[enemy.Head]), glm::vec3(0.0, 1.0, 0.0));
	enemy.mv[enemy.Head] = modelMatrix;
	enemy_head(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵

	// =========== 腿 ===========
	mstack.push(modelMatrix); // 保存躯干变换矩阵
							  // 前腿（这里我们希望enemy的前腿跟着上面转就好)
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0, -fbas));
	// modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	enemy.mv[enemy.FrontFoot] = modelMatrix;
	enemy_behind_foot(modelMatrix);
	modelMatrix = mstack.pop(); // 恢复躯干变换矩阵
								// 后腿（这里我们希望enemy的后腿跟着上面转就好)
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0, fbas));
	// modelMatrix = glm::rotate(modelMatrix, glm::radians(robot.theta[robot.LeftUpperArm]), glm::vec3(1.0, 0.0, 0.0));
	enemy.mv[enemy.BehindFoot] = modelMatrix;
	enemy_front_foot(modelMatrix);
}
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawhuman();
	// drawshootmodel();
	drawenemy();

	drawplanemodel();
	drawbulletplane();
	painter->drawMeshes(light, camera, 2, 7);
	if (cntbullet)
		drawbullet();
}

void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the light position (drag)." << std::endl;
	std::cout << "================================================" << std::endl
			  << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout << "[Window]" << std::endl
			  << "ESC:		Exit" << std::endl
			  << "h:		Print help message" << std::endl
			  <<

		std::endl
			  << "[Camera]" << std::endl
			  << "SPACE:		Reset camera parameters" << std::endl
			  << "u/U:		Increase/Decrease the rotate angle" << std::endl
			  << "i/I:		Increase/Decrease the up angle" << std::endl
			  << "o/O:		Increase/Decrease the camera radius" << std::endl
			  << std::endl;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		generatebullet();
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		cntsword = 2;
	}
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			exit(EXIT_SUCCESS);
			break;
		case GLFW_KEY_H:
			printHelp();
			break;
		case GLFW_KEY_Q:
			switch_perspective ^= true;
			break; //切换视角
		default:
			camera->keyboard(key, action, mode);
			break;
		}
	}
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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

	if (_pitch > maxUpAngle)
		_pitch = maxUpAngle;
	if (_pitch < minUpangle)
		_pitch = minUpangle;
	if (switch_perspective)
	{									 //切换视角
		robot.theta[robot.Torso] = _yaw; //设置躯干的转角
		if (svis[A])
			robot.theta[robot.Torso] -= 45;
		if (svis[D])
			robot.theta[robot.Torso] += 45;
	}

	camera->upAngle = _pitch;
	camera->rotateAngle = _yaw;
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	if (fov >= minFOV && fov <= maxFOV)
	{
		fov -= yoffset * scrSEN;
		_pitch -= yoffset * yCPI * 20;
	}

	fov = max(fov, minFOV);
	fov = min(fov, maxFOV);

	if (_pitch > maxUpAngle)
		_pitch = maxUpAngle;
	if (_pitch < minUpangle)
		_pitch = minUpangle;

	camera->radius = fov;
	camera->upAngle = _pitch;
}
void processinput(GLFWwindow *window)
{
	bool ok = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		ok = true;
		headPosX -= sin(radians(_yaw)) * moveStep;
		headPosZ -= cos(radians(_yaw)) * moveStep;
		if (abs(robot.theta[robot.LeftUpperLeg]) >= 45)
			s[W] *= -1;
		robot.theta[robot.LeftUpperLeg] -= s[W];
		robot.theta[robot.RightUpperArm] = robot.theta[robot.LeftUpperLeg];
		robot.theta[robot.LeftUpperArm] = robot.theta[robot.RightUpperLeg] = robot.theta[robot.LeftUpperLeg] * -1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		ok = true;
		headPosX += sin(radians(_yaw)) * moveStep;
		headPosZ += cos(radians(_yaw)) * moveStep;

		if (abs(robot.theta[robot.LeftUpperLeg]) >= 45)
			s[S] *= -1;
		robot.theta[robot.LeftUpperLeg] -= s[S];
		robot.theta[robot.RightUpperArm] = robot.theta[robot.LeftUpperLeg];
		robot.theta[robot.LeftUpperArm] = robot.theta[robot.RightUpperLeg] = robot.theta[robot.LeftUpperLeg] * -1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		ok = true;
		headPosX -= sin(radians(_yaw + 90)) * moveStep;
		headPosZ -= cos(radians(_yaw + 90)) * moveStep;
		if (svis[D])
			svis[D] = false, robot.theta[robot.Torso] -= 45, robot.theta[robot.Head] += 45;
		if (!svis[A])
			svis[A] = true, robot.theta[robot.Torso] -= 45, robot.theta[robot.Head] += 45;

		if (abs(robot.theta[robot.LeftUpperLeg]) >= 45)
			s[A] *= -1;
		robot.theta[robot.LeftUpperLeg] -= s[A];
		robot.theta[robot.RightUpperArm] = robot.theta[robot.LeftUpperLeg];
		robot.theta[robot.LeftUpperArm] = robot.theta[robot.RightUpperLeg] = robot.theta[robot.LeftUpperLeg] * -1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		ok = true;
		headPosX += sin(radians(_yaw + 90)) * moveStep;
		headPosZ += cos(radians(_yaw + 90)) * moveStep;
		if (svis[A])
			svis[A] = false, robot.theta[robot.Torso] += 45, robot.theta[robot.Head] -= 45;
		if (!svis[D])
			svis[D] = true, robot.theta[robot.Torso] += 45, robot.theta[robot.Head] -= 45;

		if (abs(robot.theta[robot.LeftUpperLeg]) >= 45)
			s[D] *= -1;
		robot.theta[robot.LeftUpperLeg] -= s[D];
		robot.theta[robot.RightUpperArm] = robot.theta[robot.LeftUpperLeg];
		robot.theta[robot.LeftUpperArm] = robot.theta[robot.RightUpperLeg] = robot.theta[robot.LeftUpperLeg] * -1;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !cntjump)
		cntjump = 2;
	if (!ok)
	{
		if (svis[A])
			robot.theta[robot.Torso] += 45, robot.theta[robot.Head] -= 45;
		if (svis[D])
			robot.theta[robot.Torso] -= 45, robot.theta[robot.Head] += 45;
		svis[0] = svis[1] = svis[2] = svis[3] = false;
		s[0] = s[1] = s[2] = s[3] = basS;
		robot.theta[robot.LeftUpperLeg] = robot.theta[robot.RightUpperArm] =
			robot.theta[robot.LeftUpperArm] = robot.theta[robot.RightUpperLeg] = 0;
	}
	if (cntjump)
	{
		if (cntjump == 2)
			jump += jumpspeed; // TODO速率
		if (jump >= 0.3)
			cntjump--;
		if (cntjump == 1)
			jump -= jumpspeed; // 0.002
		if (jump <= 0)
			cntjump = 0, jump = 0.0;
	}
	if (enemycntjump)
	{
		if (enemycntjump == 2)
			enemyjump += enemyjumpspeed; // TODO速率
		if (enemyjump >= 0.3)
			enemycntjump--;
		if (enemycntjump == 1)
			enemyjump -= enemyjumpspeed; // 0.002
		if (enemyjump <= 0)
			enemycntjump = 0, enemyjump = 0.0;
	}
	// 绑定摄像机位置就是人物位置，但是又是怎么弄成第三人称的呢？
	camera->cameraPos.x = headPosX;
	camera->cameraPos.z = headPosZ;
}
// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData()
{
	// 释放内存

	delete camera;
	camera = NULL;

	delete light;
	light = NULL;

	painter->cleanMeshes();

	delete painter;
	painter = NULL;

	for (int i = 0; i < meshList.size(); i++)
	{
		delete meshList[i];
	}
	meshList.clear();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
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
	GLFWwindow *window = glfwCreateWindow(600, 600, "2019192043_dxj_shootgame", NULL, NULL);
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
	initSkyBox("./shaders/vshader.glsl", "./shaders/fshader.glsl");
	inithuman();
	initenemy();
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
		// reshape();

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
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}
