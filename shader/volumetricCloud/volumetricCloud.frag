#version 330 core

in vec3 TexCoord;

in vec3 PixelPosition;
in vec3 PixelNormal;

out vec4 FragColor;

uniform float currentTime;
uniform mat4 projectionInverse;

//uniform float cubeB;
uniform vec3 cubeEdges;                 // 立方体的三个边长
uniform mat4 cubeDirAndPosition;
uniform mat4 cubeDirAndPositionInv;

uniform vec3 LightDir;

uniform int screenWidth;
uniform int screenHeight;

uniform vec3 CameraPosition;

uniform sampler3D FbmPerlinTex3D;
uniform sampler3D FbmWorleyTex3D;



// 计算一个点 p 到位于原点的，半边长为 b 的立方体表面的最近的距离
float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// 计算空间中某点 p （全局坐标下的位置）到 cube 的距离，cube 有一定的旋转，cubeInfo存的分别是 x、y、z方向轴以及中心位置
float sdfCube(mat4 cubeInfo, vec3 p)
{
    vec4 pos = vec4(p,1.0f);
    // 在 cube 的local 坐标系下的坐标为：
    vec4 localPos = cubeInfo * pos;

    return sdBox(vec3(localPos.xyz), cubeEdges);
}

// 从 ro 射出的 rd 的光射线，穿过长宽高为 edgeLength 中的 xyz 的值的长方体的长度
float cubeIntersectLength(vec3 ro, vec3 rd, vec3 edgeLength, out float tMin, out float tMax)
{
    float txMin = -1e9; float txMax = 1e9;
    float tyMin = -1e9; float tyMax = 1e9;
    float tzMin = -1e9; float tzMax = 1e9;
    if(rd.x != 0)
    {
        float tx1 = ( edgeLength.x - ro.x) / rd.x;    
        float tx2 = (-edgeLength.x - ro.x) / rd.x;    
        txMin = min(tx1, tx2);
        txMax = max(tx1, tx2);
    }
    if(rd.y != 0)
    {
        float ty1 = ( edgeLength.y - ro.y) / rd.y;    
        float ty2 = (-edgeLength.y - ro.y) / rd.y;    
        tyMin = min(ty1, ty2);
        tyMax = max(ty1, ty2);
    }
    if(rd.z != 0)
    {
        float tz1 = ( edgeLength.z - ro.z) / rd.z;    
        float tz2 = (-edgeLength.z - ro.z) / rd.z;    
        tzMin = min(tz1, tz2);
        tzMax = max(tz1, tz2);
    }
    
    tMin = max(txMin, max(tyMin, tzMin));
    tMax = min(txMax, min(tyMax, tzMax));

    return tMax - tMin;
}

// 一个光从一个 cube 里面穿过的长度
float crossLength(mat4 cubeInfo, vec3 ro, vec3 rd, float edgeLength)
{
    vec3 roLocal = vec3(cubeInfo * vec4(ro,1.0f));
    vec3 rdLocal = vec3(cubeInfo * vec4(rd,1.0f));
    float txMin = -1e9; float txMax = 1e9;
    float tyMin = -1e9; float tyMax = 1e9;
    float tzMin = -1e9; float tzMax = 1e9;
    if(rdLocal.x != 0)
    {
        float tx1 = ( edgeLength - roLocal.x) / rdLocal.x;    
        float tx2 = (-edgeLength - roLocal.x) / rdLocal.x;    
        txMin = min(tx1, tx2);
        txMax = max(tx1, tx2);
    }
    if(rdLocal.y != 0)
    {
        float ty1 = ( edgeLength - roLocal.y) / rdLocal.y;    
        float ty2 = (-edgeLength - roLocal.y) / rdLocal.y;    
        tyMin = min(ty1, ty2);
        tyMax = max(ty1, ty2);
    }
    if(rdLocal.z != 0)
    {
        float tz1 = ( edgeLength - roLocal.z) / rdLocal.z;    
        float tz2 = (-edgeLength - roLocal.z) / rdLocal.z;    
        tzMin = min(tz1, tz2);
        tzMax = max(tz1, tz2);
    }
    
    float tMin = max(txMin, max(tyMin, tzMin));
    float tMax = min(txMax, min(tyMax, tzMax));

    return tMax - tMin;
}

vec3 blinnPhong(vec3 lightColor, vec3 lightDir, vec3 pixelNormal,vec3 pixelPosition, vec3 objectColor, vec3 viewPos)
{
    // 环境光
	vec3 ambient = 0.251 * lightColor;

	// 漫反射
	float diff = max(dot(normalize(pixelNormal), normalize(lightDir)), 0.0f);
	vec3 diffuse = diff * lightColor;

	// 高光
	vec3 viewDir = normalize(viewPos - pixelPosition);
	vec3 H = normalize(viewDir + normalize(lightDir));
	float spec = pow(max(dot(pixelNormal, H),0), 32);
	vec3 specular = spec * lightColor;

	
	vec3 totalEffect = ambient + diffuse  +specular;
	vec3 factor = vec3(min(1.0f, totalEffect.x), min(1.0f, totalEffect.y), min(1.0f, totalEffect.z));

	vec3 color = factor * objectColor;

    return color;
}

float sampleDensity(vec3 position)
{
    vec3 uvw = position * 1;
    vec4 perlin = texture(FbmPerlinTex3D, uvw*0.5);
    vec4 worley = texture(FbmWorleyTex3D, uvw);

    float val = mix(perlin.r, worley.r, 0.7);

    float density=max(0, val - 0.393) * 255.1;

    return density;
}

float computeLightTransmittance(vec3 pos, vec3 lightDir )
{
    return 0.0f;
}

void main(){
    vec4 col = vec4(0.0f);


    vec2 resolution = vec2(screenWidth, screenHeight);
    vec2 ndc = (gl_FragCoord.xy / resolution) * 2 - 1.0f;
    //ndc.x *= resolution.x / resolution.y;

    vec4 clip = vec4(ndc, -1.0f, 1.0);      // [-1,1]的cube 空间里面， z= -1 就对应摄像机的近平面了
    
    // 在摄像机的视角下，使用逆投影恢复原来的坐标
    vec4 eye = projectionInverse * clip;
    
    // 以为此时是以摄像机为原点了，所以 eye ，也就是近平面上的点的位置，归一化后就可以作为 ray marching 的方向了
    vec3 rayDir = normalize(eye.xyz);
    vec3 rayOrigin = vec3(0);
    

    // 在立方体的坐标下的光位置和光方向
    rayOrigin = vec3(cubeDirAndPositionInv * vec4(0,0,0,1));
    rayDir = normalize(vec3(cubeDirAndPositionInv * vec4(rayDir,0)));
    //
    
    float tMin = 0.0f;
    float tMax = 0.0f;
    
    float distanceInBox = cubeIntersectLength(rayOrigin, rayDir, cubeEdges, tMin, tMax);
    
    int NumSteps = 64;
    float tStep = distanceInBox / NumSteps;
    float transmittance = 1.0f;
    vec4 tmpCol = vec4(0);
    
    float scale = 0.5;
    float totalDensity = 0.0f;
    vec3 lightDir = normalize(LightDir);
    for(int i=0; i < NumSteps; ++i)
    {
        // 取值范围是 [-1,1] 的cube
        vec3 samplePos = rayOrigin + rayDir * (tMin + i * tStep);
        // 变到 [0,1]的区间
        samplePos = scale * (samplePos + cubeEdges) / 2.0f / cubeEdges;
        samplePos = fract(samplePos + vec3(currentTime*0.01,0,0));
    
    
        float density = sampleDensity(samplePos);
        
        totalDensity += density * tStep;
    
    }
    transmittance = exp(-totalDensity*0.1);
    col = vec4(vec3(1), 1-transmittance);
    
    
    //col = texture(FbmPerlinTex3D,mod(TexCoord + vec3(currentTime*0.2,0,0), vec3(1)));
	gl_FragColor = col;


}

