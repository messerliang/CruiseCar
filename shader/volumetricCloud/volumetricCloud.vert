#version 330 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec4 aTexUv;
layout(location=2) in vec4 aNormal;




uniform mat4 model;
uniform mat4 TIModel;
uniform mat4 view;
uniform mat4 projection;


out vec3 TexCoord;

out vec3 pixelPosition;
out vec3 pixelNormal;

void main(){

	vec4 Pos = aPosition;
	gl_Position = projection * view * model * aPosition;

	TexCoord = aTexUv.xyz;
	
	pixelPosition = vec3(model * aPosition);
	pixelNormal = normalize(vec3(TIModel*aNormal));
}
