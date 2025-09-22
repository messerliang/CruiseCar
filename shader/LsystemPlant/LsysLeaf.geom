#version 330 core

//  对线条的几何着色器，输入一个线段，用两个三角形的构成的矩形替代

layout (points) in;
layout (triangle_strip, max_vertices=64) out;

in VS_OUT
{
	vec4 position;
	vec4 headDir;
	vec4 upDir;
	vec4 leftDir;
	float scale;
} gs_in[];

in vec4 vertColor[];
in vec3 vertNormal[];
in vec3 vertPos[];

out vec4 fColor;
out vec2 texCoord;
out vec3 fragNormal;
out vec3 fragPosition;


uniform mat4 view;
uniform mat4 projection;


const float PI = 3.14159265359;



// 绕某根轴旋转的矩阵

mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float c = cos(angle);
    float s = sin(angle);
    float t = 1.0 - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return mat3(
        t*x*x + c,     t*x*y - s*z,  t*x*z + s*y,
        t*x*y + s*z,   t*y*y + c,    t*y*z - s*x,
        t*x*z - s*y,   t*y*z + s*x,  t*z*z + c
    );
}

// 伪随机函数
float rand(float n) {
    return fract(sin(n) * 43758.5453123);
}

vec3 randomLeafColor(float seed) {
    float g = 0.6 + 0.4 * rand(seed);      // 主体绿色，范围 [0.6, 1.0]
    float r = 0.0 + 0.2 * rand(seed + 1.0); // 少量红色，范围 [0.0, 0.2]
    float b = 0.0 + 0.2 * rand(seed + 2.0); // 少量蓝色，范围 [0.0, 0.2]
    return vec3(r, g, b);
}
void DrawLeaf()
{	
	fColor = vec4(0.0f, 0.87f, 0.0f, 0.5f);
	fragNormal = vertNormal[0];
	fragPosition = vertPos[0];
	// 两个点
	float scale = gs_in[0].scale;
	vec3 p0 = gs_in[0].position.xyz;

	vec3 leftDir = normalize(gs_in[0].leftDir.xyz);
	vec3 headDir = normalize(gs_in[0].headDir.xyz);
	float L = 0.631f * scale;
	float H = 1.024f * scale;
	vec3 leafp0 = p0 - L* 0.5 * leftDir;
	vec3 leafp1 = p0 - L* 0.5 * leftDir + H * headDir;
	vec3 leafp2 = p0 + L* 0.5 * leftDir;
	vec3 leafp3 = p0 + L* 0.5 * leftDir + H * headDir;


	gl_Position = projection * view * vec4(leafp0, 1.0f);texCoord = vec2(0.0f, 1.0f);	EmitVertex();
	gl_Position = projection * view * vec4(leafp1, 1.0f);texCoord = vec2(1.0f, 1.0f);	EmitVertex();
	gl_Position = projection * view * vec4(leafp2, 1.0f);texCoord = vec2(0.0f, 0.0f);	EmitVertex();
	gl_Position = projection * view * vec4(leafp3, 1.0f);texCoord = vec2(1.0f, 0.0f);	EmitVertex();
	
	EndPrimitive();

}



void main(){
	
	//fColor = vec4(0.0f, 0.87f, 0.0f, 0.5f);
	//gl_Position = projection * view * vec4(gs_in[0].position.xyz, 1.0f);	EmitVertex();
	DrawLeaf();

}

