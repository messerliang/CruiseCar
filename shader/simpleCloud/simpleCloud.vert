#version 410 core

layout(location=0) in vec4 aPosition;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec4 QuadPosition;


void main(){

	gl_Position = projection* view * model * aPosition;
	QuadPosition = model * aPosition;

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