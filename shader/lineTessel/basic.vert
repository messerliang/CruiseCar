#version 410 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec2 aTex;


uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 TIModel;
uniform mat4 view;
uniform mat4 projection;



out vec2 TexCoord;


void main(){

	gl_Position = aPosition;
	TexCoord = aTex;

}
//out VS_OUT {
//    vec3 color;
//} vs_out;
//
//void main()
//{
//    gl_Position = projection * view * model * position;
//    vs_out.color = vec3(1.0f, 0.7f, 0.34f);
//}