#version 330 core

in vec3 TexCoord;

out vec4 FragColor;

uniform sampler3D WorleyNoise;

void main(){

	vec4 color = texture(WorleyNoise, TexCoord);
	//FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	FragColor = color;
}

