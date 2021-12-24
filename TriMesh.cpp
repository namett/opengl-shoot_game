#include "TriMesh.h"
using namespace glm;

// 一些基础颜色
const glm::vec3 basic_colors[8] = {
	glm::vec3(1.0, 1.0, 1.0), // White
	glm::vec3(1.0, 1.0, 0.0), // Yellow
	glm::vec3(0.0, 1.0, 0.0), // Green
	glm::vec3(0.0, 1.0, 1.0), // Cyan
	glm::vec3(1.0, 0.0, 1.0), // Magenta
	glm::vec3(1.0, 0.0, 0.0), // Red
	glm::vec3(0.0, 0.0, 0.0), // Black
	glm::vec3(0.0, 0.0, 1.0)	 // Blue
};

// 立方体的各个点
const glm::vec3 cube_vertices[8] = {
	glm::vec3(-0.5, -0.5, -0.5),
	glm::vec3(0.5, -0.5, -0.5),
	glm::vec3(-0.5, 0.5, -0.5),
	glm::vec3(0.5, 0.5, -0.5),
	glm::vec3(-0.5, -0.5, 0.5),
	glm::vec3(0.5, -0.5, 0.5),
	glm::vec3(-0.5, 0.5, 0.5),
	glm::vec3(0.5, 0.5, 0.5)};


// 三角形的点
const glm::vec3 triangle_vertices[3] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.0, 0.5, 0.0)};

// 正方形平面
const glm::vec3 square_vertices[4] = {
	glm::vec3(-0.5, -0.5, 0.0),
	glm::vec3(0.5, -0.5, 0.0),
	glm::vec3(0.5, 0.5, 0.0),
	glm::vec3(-0.5, 0.5, 0.0),
};

TriMesh::TriMesh()
{
	do_normalize_size = true;
	diagonal_length = 1.0;
	scale = glm::vec3(1.0);
	rotation = glm::vec3(0.0);
	translation = glm::vec3(0.0);
}

TriMesh::~TriMesh(){}

std::vector<glm::vec3> TriMesh::getVertexPositions() { return vertex_positions; }
std::vector<glm::vec3> TriMesh::getVertexColors(){ return vertex_colors; }
std::vector<glm::vec3> TriMesh::getVertexNormals(){ return vertex_normals; }
std::vector<glm::vec2> TriMesh::getVertexTextures(){ return vertex_textures; }

std::vector<vec3i> TriMesh::getFaces(){ return faces; }
std::vector<glm::vec3> TriMesh::getPoints(){ return points;}
std::vector<glm::vec3> TriMesh::getColors(){ return colors; }
std::vector<glm::vec3> TriMesh::getNormals(){ return normals;}
std::vector<glm::vec2> TriMesh::getTextures(){ return textures; }
std::vector<vec3i> TriMesh::getTexturesindex() {return texture_index;}

void TriMesh::computeTriangleNormals()
{
	face_normals.resize(faces.size());
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		glm::vec3 v01 = vertex_positions[face.y] - vertex_positions[face.x];
		glm::vec3 v02 = vertex_positions[face.z] - vertex_positions[face.x];
		face_normals[i] = normalize(cross(v01, v02));
	}
}

void TriMesh::computeVertexNormals()
{
	// 计算面片的法向量
	if (face_normals.size() == 0 && faces.size() > 0)
	{
		computeTriangleNormals();
	}
	// 初始化法向量为0
	vertex_normals.resize(vertex_positions.size(), glm::vec3(0, 0, 0));
	for (size_t i = 0; i < faces.size(); i++)
	{
		auto &face = faces[i];
		vertex_normals[face.x] += face_normals[i];
		vertex_normals[face.y] += face_normals[i];
		vertex_normals[face.z] += face_normals[i];
	}
	for (size_t i = 0; i < vertex_normals.size(); i++)
	{
		vertex_normals[i] = normalize(vertex_normals[i]);
	}
	// 球心在原点的球法向量为坐标
	// for (int i = 0; i < vertex_positions.size(); i++)
	// 	vertex_normals.push_back(vertex_positions[i] - vec3(0.0, 0.0, 0.0));
}

void TriMesh::setNormalize(bool do_norm) { do_normalize_size = do_norm; }
bool TriMesh::getNormalize() { return do_normalize_size; }
float TriMesh::getDiagonalLength() { return diagonal_length; }

glm::vec3 TriMesh::getTranslation(){ return translation;}
glm::vec3 TriMesh::getRotation(){ return rotation;}
glm::vec3 TriMesh::getScale(){ return scale; }

glm::mat4 TriMesh::getModelMatrix( ) {
	
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 trans = getTranslation();
	model = glm::translate(model, getTranslation());
	model = glm::rotate(model, glm::radians(getRotation()[2]), glm::vec3(0.0, 0.0, 1.0));
	model = glm::rotate(model, glm::radians(getRotation()[1]), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(getRotation()[0]), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, getScale());
	return model;
}

void TriMesh::setTranslation(glm::vec3 translation){this->translation = translation; }
void TriMesh::setRotation(glm::vec3 rotation){ this->rotation = rotation; }
void TriMesh::setScale(glm::vec3 scale) { this->scale = scale; }

glm::vec4 TriMesh::getAmbient() { return ambient; };
glm::vec4 TriMesh::getDiffuse() { return diffuse; };
glm::vec4 TriMesh::getSpecular() { return specular; };
float TriMesh::getShininess() { return shininess; };

void TriMesh::setAmbient(glm::vec4 _ambient) { ambient = _ambient; };
void TriMesh::setDiffuse(glm::vec4 _diffuse) { diffuse = _diffuse; };
void TriMesh::setSpecular(glm::vec4 _specular) { specular = _specular; };
void TriMesh::setShininess(float _shininess) { shininess = _shininess; };

void TriMesh::cleanData()
{
	vertex_positions.clear();
	vertex_colors.clear();
	vertex_normals.clear();
	vertex_textures.clear();

	faces.clear();
	normal_index.clear();
	color_index.clear();
	texture_index.clear();

	face_normals.clear();

	points.clear();
	colors.clear();
	normals.clear();
	textures.clear();
}

void TriMesh::storeFacesPoints()
{
	// 将读取的顶点根据三角面片上的顶点下标逐个加入
	// 要传递给GPU的points等容器内

	// 看是否归一化物体大小，是的话，这里将物体顶点缩放到对角线长度为1的包围盒内
	if (do_normalize_size) {
		// 记录物体包围盒大小，可以用于大小的归一化
		// 先获得包围盒的对角顶点
		float max_x = -FLT_MAX;
		float max_y = -FLT_MAX;
		float max_z = -FLT_MAX;
		float min_x = FLT_MAX;
		float min_y = FLT_MAX;
		float min_z = FLT_MAX;
		for (int i = 0; i < vertex_positions.size(); i++) {
			auto& position = vertex_positions[i];
			if (position.x > max_x) max_x = position.x;
			if (position.y > max_y) max_y = position.y;
			if (position.z > max_z) max_z = position.z;
			if (position.x < min_x) min_x = position.x;
			if (position.y < min_y) min_y = position.y;
			if (position.z < min_z) min_z = position.z;
		}
		up_corner = glm::vec3(max_x, max_y, max_z);
		down_corner = glm::vec3(min_x, min_y, min_z);
		center = glm::vec3((min_x + max_x) / 2.0, (min_y + max_y) / 2.0, (min_z + max_z) / 2.0);

		diagonal_length = length(up_corner - down_corner);

		for (int i = 0; i < vertex_positions.size(); i++) {
			vertex_positions[i] = (vertex_positions[i] - center) / diagonal_length;
		}
	}

	// 计算法向量
	if (vertex_normals.size() == 0)
		computeVertexNormals();

	// @TODO Task1 根据每个三角面片的顶点下标存储要传入GPU的数据
	for (int i = 0; i < faces.size(); i++)
	{
		//std::cout << faces[i].x << " " << color_index[i].y << " " << normal_index[i].z << "\n";
		// 坐标
		points.push_back(vertex_positions[faces[i].x]);
		points.push_back(vertex_positions[faces[i].y]);
		points.push_back(vertex_positions[faces[i].z]);
		// 颜色
		colors.push_back(vertex_colors[color_index[i].x]);
		colors.push_back(vertex_colors[color_index[i].y]);
		colors.push_back(vertex_colors[color_index[i].z]);
		// 法向量
		if (vertex_normals.size() != 0)	{
			normals.push_back(vertex_normals[normal_index[i].x]);
			normals.push_back(vertex_normals[normal_index[i].y]);
			normals.push_back(vertex_normals[normal_index[i].z]);
		}
		// 纹理
		if (vertex_textures.size() != 0) {
			textures.push_back(vertex_textures[texture_index[i].x]);
			textures.push_back(vertex_textures[texture_index[i].y]);
			textures.push_back(vertex_textures[texture_index[i].z]);
		}
	}
}

// 立方体生成12个三角形的顶点索引
void TriMesh::generateCube(glm::vec3 _color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 8; i++)
	{
		vertex_positions.push_back(cube_vertices[i]);
		
		if (_color[0] == -1){
			vertex_colors.push_back(basic_colors[i]);
		}
		else{
			vertex_colors.push_back( _color );
		}
	}

	// 每个三角面片的顶点下标

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 3, 1));
	faces.push_back(vec3i(0, 2, 3));
	faces.push_back(vec3i(1, 5, 4));
	faces.push_back(vec3i(1, 4, 0));
	faces.push_back(vec3i(4, 2, 0));
	faces.push_back(vec3i(4, 6, 2));
	faces.push_back(vec3i(5, 6, 4));
	faces.push_back(vec3i(5, 7, 6));
	faces.push_back(vec3i(2, 6, 7));
	faces.push_back(vec3i(2, 7, 3));
	faces.push_back(vec3i(1, 7, 5));
	faces.push_back(vec3i(1, 3, 7));

	// 顶点纹理坐标，只是自己想的一种贴图方式而已
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(1, 1));


	// 颜色下标，让一个面的颜色都一样
	for (int i=0; i<6; i++) {
		color_index.push_back(vec3i(i, i, i));
		color_index.push_back(vec3i(i, i, i));
	}

	texture_index = faces;
	normal_index = faces;

	storeFacesPoints();
	
	normals.clear();
	// 正方形的法向量不能靠之前顶点法向量的方法直接计算，因为每个四边形平面是正交的，不是连续曲面
	for (int i = 0; i < faces.size(); i++)
	{
		normals.push_back( face_normals[i] );
		normals.push_back( face_normals[i] );
		normals.push_back( face_normals[i] );
	}
}

void TriMesh::generateSquare(glm::vec3 color)
{
	// 创建顶点前要先把那些vector清空
	cleanData();

	for (int i = 0; i < 4; i++)
	{
		vertex_positions.push_back(square_vertices[i]);
		vertex_colors.push_back(color);
	}

	// 每个三角面片的顶点下标
	faces.push_back(vec3i(0, 1, 2));
	faces.push_back(vec3i(2, 3, 0));

	// 顶点纹理坐标
	vertex_textures.push_back(glm::vec2(0, 0));
	vertex_textures.push_back(glm::vec2(1, 0));
	vertex_textures.push_back(glm::vec2(1, 1));

	vertex_textures.push_back(glm::vec2(1, 1));
	vertex_textures.push_back(glm::vec2(0, 1));
	vertex_textures.push_back(glm::vec2(0, 0));

	texture_index.push_back(vec3i(0, 1, 2));
	texture_index.push_back(vec3i(3, 4, 5));

	normal_index = faces;
	color_index = faces;
	// texture_index = faces;

	storeFacesPoints();
	int a = 0;
}
void TriMesh::readOff(const std::string &filename)
{
	// fin打开文件读取文件信息
	if (filename.empty())
	{
		return;
	}
	std::ifstream fin;
	fin.open(filename);
	if (!fin)
	{
		printf("File on error\n");
		return;
	}
	else
	{
		printf("File open success\n");

		cleanData();

		int nVertices, nFaces, nEdges;

		// 读取OFF字符串
		std::string str;
		fin >> str;
		// 读取文件中顶点数、面片数、边数
		fin >> nVertices >> nFaces >> nEdges;
		// 根据顶点数，循环读取每个顶点坐标
		for (int i = 0; i < nVertices; i++)
		{
			glm::vec3 tmp_node;
			fin >> tmp_node.x >> tmp_node.y >> tmp_node.z;
			vertex_positions.push_back(tmp_node);
			vertex_colors.push_back(tmp_node);
		}
		// 根据面片数，循环读取每个面片信息，并用构建的vec3i结构体保存
		for (int i = 0; i < nFaces; i++)
		{
			int num, a, b, c;
			// num记录此面片由几个顶点构成，a、b、c为构成该面片顶点序号
			fin >> num >> a >> b >> c;
			faces.push_back(vec3i(a, b, c));
		}
	}
	fin.close();

	normal_index = faces;
	color_index = faces;
	texture_index = faces;

	storeFacesPoints();
};

void TriMesh::readObj(const std::string& filename)
{
	std::ifstream fin(filename);
	std::string line;

	if (!fin)
	{
		std::cout << "ERROR: cannot open the file: " << filename << std::endl;
		exit(0);	// 退出程序
	}

	cleanData();

	while (std::getline(fin, line))
	{
		std::istringstream sin(line);
		std::string type;
		GLfloat _x, _y, _z;
		int a0, b0, c0;
		int a1, b1, c1;
		int a2, b2, c2;
		char slash;

		fin >> type;
		// @TODO: Task2 读取obj文件，记录里面的这些数据，可以参考readOff的写法
		if (type == "v" || type == "vn") {
			//std::cout << type<< "" <<filename << "\n";
			fin >> _x >> _y >> _z;
			// vertex_positions
			if (type == "v") vertex_positions.push_back(vec3(_x, _y, _z));
			// vertex_normals
			if (type == "vn") {
				vertex_normals.push_back(vec3(_x, _y, _z));
				vertex_colors.push_back(vec3(_x, _y, _z));
			}
			
		}
		if (type == "vt") {
			fin >> _x >> _y;
			// vertex_textures		
			if (type == "vt") vertex_textures.push_back(vec3(_x, _y, 0.0));
		}
		if (type == "f") {
			fin >> a0 >> slash >> b0 >> slash >> c0
				>> a1 >> slash >> b1 >> slash >> c1
				>> a2 >> slash >> b2 >> slash >> c2;
			// faces
			faces.push_back(vec3i(a0 - 1, a1 - 1, a2 - 1));
			// texture_index
			texture_index.push_back(vec3i(b0 - 1, b1 - 1, b2 - 1));
			// normal_index
			normal_index.push_back(vec3i(c0 - 1, c1 - 1, c2 - 1));
			color_index.push_back(vec3i(c0 - 1, c1 - 1, c2 - 1));
		}
		// 其中vertex_color和color_index可以用法向量的数值赋值
	}

	storeFacesPoints();
}
void TriMesh::readMtl(const std::string& filename)
{
	std::ifstream fin(filename);
	std::string line;

	if (!fin)
	{
		std::cout << "ERROR: cannot open the file: " << filename << std::endl;
		exit(0);	// 退出程序
	}

	while (std::getline(fin, line))
	{
		std::istringstream sin(line);
		std::string type;
		GLfloat _x, _y, _z;

		fin >> type;
		if (type == "Ka" || type == "Kd" || type == "Ks") {
			fin >> _x >> _y >> _z;
			if (type == "Ka") this->setAmbient(vec4(_x, _y, _z, 1.0f));
			if (type == "Kd") this->setDiffuse(vec4(_x, _y, _z, 1.0f));
			if (type == "Ks") this->setSpecular(vec4(_x, _y, _z, 1.0f));
		}

	}

}


// Light
glm::mat4 Light::getShadowProjectionMatrix()
{
	// 这里只实现了Y=0平面上的阴影投影矩阵，其他情况自己补充
	float lx, ly, lz;

	//glm::mat4 modelMatrix = this->getModelMatrix();
	//glm::vec4 light_position = modelMatrix * glm::vec4(this->translation, 1.0);
	glm::vec4 light_position = glm::vec4(this->translation, 1.0);

	lx = light_position[0];
	ly = light_position[1];
	lz = light_position[2];

	return glm::mat4(
		-ly, 0.0, 0.0, 0.0,
		lx, 0.0, lz, 1.0,
		0.0, 0.0, -ly, 0.0,
		0.0, 0.0, 0.0, -ly);
}

// 设置衰减系数的参数
void Light::setAttenuation(float _constant, float _linear, float _quadratic)
{
	constant = _constant;
	linear = _linear;
	quadratic = _quadratic;
}

float Light::getConstant() { return constant; };
float Light::getLinear() { return linear; };
float Light::getQuadratic() { return quadratic; };
