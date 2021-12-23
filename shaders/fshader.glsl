#version 330 core

// 给光源数据一个结构体
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	vec3 position;

    // 光源衰减系数的三个参数
    float constant; // 常数项
    float linear;	// 一次项
    float quadratic;// 二次项

};

// 给物体材质数据一个结构体
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float shininess;
};

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;


// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;

uniform int isShadow;
uniform int isColor;
// 纹理数据
uniform sampler2D texture;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		fColor = vec4(0.051, 0.051, 0.051, 1.0);
	}
	else {
		// vec3 norm = (vec4(normal, 0.0)).xyz;

		// // 计算四个归一化的向量 N,V,L,R(或半角向量H)
		// vec3 N = normalize(norm);
		// vec3 V = normalize(eye_position - position);
		// vec3 L = normalize(light.position - position);
		// vec3 R = normalize(reflect(-L, N));
		

		// // 环境光分量I_a
		// vec4 I_a = light.ambient * material.ambient;

		// // 计算系数和漫反射分量I_d
		// float diffuse_dot = max(dot(L,N), 0);
		// vec4 I_d = diffuse_dot * light.diffuse * material.diffuse;

		// // 计算系数和镜面反射分量I_s
		// float specular_dot_pow = pow(max(dot(R,V), 0), material.shininess);
		// vec4 I_s = specular_dot_pow * light.specular * material.specular;

		// // 计算衰减参数
		// float d = distance(light.position, position);
		// float c = 1 / (light.constant + light.linear*d + light.quadratic*d*d);
		// // c = 1.0; //不考虑衰减
		// // 结合衰减系数，合并三个分量的颜色，修正透明度
		// fColor = I_a + c*(I_d + I_s);
		// fColor.a = 1.0;

		
		// // if(isTextrue == 1){
		// //	fColor = texture( texture, texCoord );
		// //  }
		// // else if (isColor == 1){
		// if (isColor ==1) {
		// 	tmp = vec4(color, 1.0);
		// }
		// // fColor = vec4(normal, 1.0);
		// // fColor = vec4(normalize(position) , 1.0);
		// // fColor = vec4(1.0/position.z, position.z, position.z , 1.0);


		fColor = vec4(color, 1.0);
		// 将光照和贴图合成
		if (isColor == 1) {
			fColor = texture2D( texture, texCoord );
			// fColor.r*=tmp.r;
			// fColor.g*=tmp.g;
			// fColor.b*=tmp.b;			
		}

		
	}
}
