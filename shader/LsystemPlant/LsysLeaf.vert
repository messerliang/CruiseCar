#version 330 core

layout(location=0) in vec4 Position;
layout(location=1) in vec4 HeadDir;
layout(location=2) in vec4 UpDir;
layout(location=3) in vec4 LeftDir;
layout(location=4) in float Scale;

uniform mat4 model;
uniform mat4 TIModel;



out VS_OUT
{
	vec4 position;
	vec4 headDir;
	vec4 upDir;
	vec4 leftDir;
	float scale;
} vs_out;

out vec4 vertColor;
out vec3 vertNormal;
out vec3 vertPos;

void main(){
	vec3 offset = vec3(0.0f);
	offset.x = (gl_InstanceID % 10) * 1.0f;
	vertColor = vec4(0.545, 0.271, 0.075, 1.0);

	gl_Position = model * vec4((Position.xyz), 1.0f);
	vs_out.position = gl_Position;
	vs_out.headDir = model * vec4(HeadDir.xyz, 1.0f);
	vs_out.upDir =  model * vec4(UpDir.xyz, 1.0f);
	vs_out.leftDir =  model * vec4(LeftDir.xyz, 1.0f);
	vs_out.scale = Scale;

	vertPos = vec3(model * Position);
	vertNormal = normalize(vec3(TIModel * UpDir));
}
