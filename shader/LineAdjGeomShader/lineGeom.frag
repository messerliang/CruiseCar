#version 330 core

in vec4 fColor;

out vec4 FragColor;

void main(){

//	FragColor = vec4(1.0f, 0.7f, 0.34f, 1.0f);
	FragColor = fColor;
}

