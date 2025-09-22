#version 330 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec4 aNormal;



uniform mat4 model;
uniform mat4 TIModel;
uniform mat4 view;
uniform mat4 projection;

uniform float ScaleH;


out VS_OUT{
	vec3 Position;
	vec3 Normal;
}vs_out;



void main(){

	vec4 Pos = aPosition;
	Pos.y = Pos.y * ScaleH;
	gl_Position = projection * view * model * (Pos);


	vs_out.Position = vec3(model * Pos);
	vs_out.Normal = normalize(vec3(TIModel*aNormal));

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