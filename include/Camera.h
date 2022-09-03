#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Angel.h"

// 控制视角参数
#define DEFAULT_YAW 0.0 //默认偏航角
#define DEFAULT_PITCH 30.0f //默认俯仰角
#define DEFAULT_FOV 3.0f //默认视角半径 ？
// 控制视角移动参数
#define DEFAULT_HEAD_POS_X 0.0
#define DEFAULT_HEAD_POS_Z 0.0
#define DEFAULT_HEIGHT 0.15

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
	Camera();
	~Camera();

	// 获得观察矩阵
	glm::mat4 getViewMatrix();
	// 获得投影矩阵
	glm::mat4 getProjectionMatrix( bool isOrtho );
	// 根据视角参数计算观察矩阵
	glm::mat4 lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up);
	// 根据视角参数计算正交矩阵
	glm::mat4 ortho(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);
	// 根据视角参数计算透视矩阵
	glm::mat4 perspective(const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar);

	glm::mat4 frustum(const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar);

	// 每次更改相机参数后更新一下相关的数值
	void updateCamera();
	// 初始化相机参数
	void initCamera();
	// 处理相机的键盘操作
	void keyboard(int key, int action, int mode);
	
	
	// 模视矩阵
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	// 相机位置参数
	float radius;
	float rotateAngle;
	float upAngle;
	// 相机看着的点
	glm::vec3 cameraPos;
	// 相机位置
	glm::vec4 eye;
	glm::vec4 at;
	glm::vec4 up;

	// 投影参数
	float zNear;
	float zFar;
	// 透视投影参数
	float fovy;
	float aspect;
	// 正交投影参数
	float scale;

};
#endif