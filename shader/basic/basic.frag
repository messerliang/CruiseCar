#version 410 core

//in vec2 TexCoord;
//
//
//out vec4 FragColor;
//
//void main()
//{
//	FragColor = vec4(1,1,0, 1);
//}

in float Height;

out vec4 FragColor;


void main(){
	float h = (Height + 16) / 64.0f;
	h = clamp(h, 0.1f,1.0f);
	FragColor = vec4(h, h, h, 1.0f);
	
}

