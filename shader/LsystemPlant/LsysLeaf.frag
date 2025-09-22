#version 330 core

in vec4 fColor;
in vec2 texCoord;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 FragColor;
uniform sampler2D leafTexture;

uniform vec3 LightColor;
uniform vec3 LightDir;
uniform vec3 ViewPos;


void main(){
	vec4 objectColor = texture(leafTexture, texCoord);

	if(objectColor.a < 0.001f)
	{
		discard;
	}

	// 环境光
	vec3 ambient = 0.01 * LightColor;

	// 漫反射
	float diff = max(dot(normalize(fragNormal), normalize(LightDir)), 0.0f);
	vec3 diffuse = diff * LightColor;

	// 高光
	vec3 viewDir = normalize(ViewPos - fragPosition);
	vec3 H = normalize(viewDir + LightDir);
	float spec = pow(max(dot(fragNormal, H),0), 32);
	vec3 specular = spec * LightColor;

	vec4 color = vec4((ambient + diffuse + specular), 1.0f) * objectColor;

	FragColor = color;//vec4(LightColor,1.0f);
}

