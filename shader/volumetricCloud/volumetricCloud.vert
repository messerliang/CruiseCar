#version 330 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec2 aTexUv;




uniform mat4 model;
uniform mat4 TIModel;
uniform mat4 view;
uniform mat4 projection;


out vec3 TexCoord;


void main(){

	vec4 Pos = aPosition;
	gl_Position = projection * view * model * (Pos);

	TexCoord = vec3(0.0f, aTexUv);
	
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