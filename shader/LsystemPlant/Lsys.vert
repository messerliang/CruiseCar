#version 330 core

layout(location=0) in vec4 Position;
layout(location=1) in vec4 HeadDir;
layout(location=2) in vec4 UpDir;
layout(location=3) in vec4 LeftDir;
layout(location=4) in float Radius;
layout(location=5) in int IsLeaf;

uniform mat4 model;
uniform mat4 view;

out VS_OUT
{
	vec4 position;
	vec4 headDir;
	vec4 upDir;
	vec4 leftDir;
	float radius;
	int isLeaf;
} vs_out;

out vec4 vertColor;

void main(){
	vec3 offset = vec3(0.0f);
	offset.x = (gl_InstanceID % 10) * 1.0f;
	vertColor = vec4(0.545, 0.271, 0.075, 1.0);
	gl_Position = view* model * vec4((Position.xyz + offset), 1.0f);
	vs_out.position = gl_Position;
	vs_out.headDir = view * model * vec4(HeadDir.xyz, 1.0f);
	vs_out.upDir = view * model * vec4(UpDir.xyz, 1.0f);
	vs_out.leftDir = view * model * vec4(LeftDir.xyz, 1.0f);
	vs_out.radius = Radius;
	vs_out.isLeaf = IsLeaf;
}
