#version 330 core

in vec4 fColor;
in vec4 fragNormal;

uniform sampler2D TrunkTex;

out vec4 FragColor;

uniform vec3 LightDir; // 光的方向
uniform vec3 LightColor; // 光的颜色
uniform vec3 ViewPos; // 视角位置


void main(){

	//FragColor = vec4(1.0f, 0.7f, 0.34f, 1.0f);
	FragColor = fColor;
}

