#version 330 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec4 aNormal;
layout(location=2) in vec2 aTexCoord;


uniform mat4 model;
uniform mat4 TIModel;
uniform mat4 view;
uniform mat4 projection;

uniform float ScaleH;

out vec4 vertColor;
out vec2 texCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main(){
	vertColor = vec4(1.0f, 0.7f, 0.34f, 1.0f);
	vec4 Pos = aPosition;
	Pos.y = Pos.y * ScaleH;
	gl_Position = projection * view * model * (Pos);
	texCoord = aTexCoord;
	fragPosition = vec3(model * Pos);
	fragNormal = normalize(vec3(TIModel*aNormal));
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