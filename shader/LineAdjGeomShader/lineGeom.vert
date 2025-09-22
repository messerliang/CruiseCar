#version 330 core

layout(location=0) in vec4 position;

uniform mat4 model;
uniform mat4 view;

out vec4 vertColor;

void main(){
	vertColor = vec4(1.0f, 0.7f, 0.34f, 1.0f);
	gl_Position = view* model * position;
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