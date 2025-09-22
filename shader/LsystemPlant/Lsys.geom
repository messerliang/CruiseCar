#version 330 core

//  对线条的几何着色器，输入一个线段，用两个三角形的构成的矩形替代

layout (lines) in;
layout (triangle_strip, max_vertices=64) out;

in VS_OUT
{
	vec4 position;
	vec4 headDir;
	vec4 upDir;
	vec4 leftDir;
	float radius;
	int isLeaf;
} gs_in[];

in vec4 vertColor[];

out vec4 fColor;

uniform float lineWidth;		// 线宽
uniform int  segmentsNumber;	// 圆柱体面数

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


void Cylinder()
{
	fColor = vertColor[0];
	float segAngle = 2.0f * PI / segmentsNumber; // 每个扇区的角度（弧度为单位）
	// 两个点
	vec3 p0 = gs_in[0].position.xyz;
	vec3 p1 = gs_in[1].position.xyz;
	float radius0 = gs_in[0].radius;
	float radius1 = gs_in[1].radius;


	vec3 forward = normalize(p1-p0);
	vec3 helper = abs(forward.x) < 0.9f ? vec3(1.0f, 0.0f,0.0f) : vec3(0.0f, 1.0f, 0.0f);

	// 计算正交基
	vec3 u = normalize(cross(helper, forward));
	vec3 w = normalize(cross(u, forward));

	
	vec3 axis = forward; // 线段起点的切线作为旋转轴
	vec3 offset = u;
	mat3 rotateMatBottom = rotationMatrix(axis, segAngle);

	vec3 stPb, stPt;

	for(int i=0; i<segmentsNumber; ++i)
	{
		vec3 pb = p0 + 0.5 * radius0 * offset;	gl_Position = projection * vec4(pb, 1.0f);	EmitVertex();
		vec3 pt = p1 + 0.5 * radius1 * offset;	gl_Position = projection * vec4(pt, 1.0f);	EmitVertex();
		offset = rotateMatBottom * offset;
		if(0==i)
		{
			stPb = pb;
			stPt = pt;
		}
	}
	gl_Position = projection * vec4(stPb, 1.0f);	EmitVertex();
	gl_Position = projection * vec4(stPt, 1.0f);	EmitVertex();
	EndPrimitive();

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
	// 两个点
	vec3 p0 = gs_in[0].position.xyz;
	vec3 p1 = gs_in[1].position.xyz;
	float radius = gs_in[1].radius;
	vec3 forward = 4*normalize(gs_in[1].headDir.xyz);


	vec3 leftDir = gs_in[1].leftDir.xyz;
	vec3 upDir = gs_in[1].upDir.xyz;

	float angle = PI / 3;
	mat3 m1 = rotationMatrix(upDir, angle);
	mat3 m2 = rotationMatrix(upDir, -angle);
	
	mat3 rotate1 = rotationMatrix(leftDir, 0.7*PI);
	mat3 rotate2 = rotationMatrix(forward, 2.0f*PI/3.0f);

	// 叶片1
	vec3 leafp0 =  p1 + (0.0f * forward);
	vec3 leafp1 =  p1 + (0.0f + m1 * forward * 0.18);
	vec3 leafp2 =  p1 + (0.0f + m2 * forward * 0.18);
	vec3 leafp3 =  p1 + (0.0f + forward*0.63);

	gl_Position = projection * vec4(leafp0, 1.0f);	EmitVertex();
	gl_Position = projection * vec4(leafp1, 1.0f);	EmitVertex();
	gl_Position = projection * vec4(leafp2, 1.0f);	EmitVertex();
	gl_Position = projection * vec4(leafp3, 1.0f);	EmitVertex();
	
	EndPrimitive();

}



void main(){
	
	Cylinder();

}

