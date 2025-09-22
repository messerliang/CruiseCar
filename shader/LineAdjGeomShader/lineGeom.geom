#version 330 core

//  对线条的几何着色器，输入一个线段，用两个三角形的构成的矩形替代

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices=64) out;

in vec4 vertColor[];

out vec4 fColor;

uniform float lineWidth; // 线宽
uniform int  segmentsNumber;	// 圆柱体面数

uniform mat4 projection;

const float PI = 3.14159265359;

void build_house(vec4 position){
	
	gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.2f, -0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4(-0.2f,  0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.2f,  0.2f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();
	gl_Position = position + vec4( 0.0f,  0.4f, 0.0f, 0.0f) * 5.0f;
	EmitVertex();

	EndPrimitive();
	
}

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
	float segAngle = 2.0f * PI / segmentsNumber; // 每个扇区的角度（弧度为单位）
	// 两个点
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;
	vec3 p3 = gl_in[3].gl_Position.xyz;

	vec3 forward = normalize(p2 - p1);

	vec3 dir0 = normalize(p0 - p1);
	vec3 dir1 = normalize(p2 - p1);
	vec3 tanDir1 = normalize(cross(dir0, dir1));
	vec3 norm1 = -1.0f * normalize(dir0 + dir1);
	//vec3 axisButtom = normalize(cross(norm1, tanDir1)); // 线段起点的切线作为旋转轴
	vec3 axisButtom = forward; // 线段起点的切线作为旋转轴
	vec3 offsetDirBottom = tanDir1;
	mat3 rotateMatBottom = rotationMatrix(axisButtom, segAngle);


	vec3 dir2 = normalize(p1 - p2);
	vec3 dir3 = normalize(p3 - p2);
	vec3 tanDir2 = normalize(cross(dir2, dir3));
	vec3 norm2 = -1.0f * normalize(dir2 + dir3);
	//vec3 axisTop = normalize(cross(norm2, tanDir2));
	vec3 axisTop = forward;
	vec3 offsetDirTop = tanDir2;
	mat3 rotateMatTop = rotationMatrix(axisTop, segAngle);

	vec3 stPb, stPt;

	for(int i=0; i<segmentsNumber; ++i)
	{
		vec3 pb = p1 + 0.5 * lineWidth * offsetDirBottom;	gl_Position = projection * vec4(pb, 1.0f);	EmitVertex();
		vec3 pt = p2 + 0.5 * lineWidth * offsetDirTop;		gl_Position = projection * vec4(pt, 1.0f);	EmitVertex();
		offsetDirBottom = rotateMatBottom * offsetDirBottom;
		offsetDirTop = rotateMatTop * offsetDirTop;
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

void main(){
	fColor = vertColor[0];
	Cylinder();

}

