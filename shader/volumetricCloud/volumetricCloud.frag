#version 330 core

in vec3 TexCoord;

in vec3 PixelPosition;
in vec3 PixelNormal;

out vec4 FragColor;

uniform float currentTime;
uniform mat4 projectionInverse;

uniform float cubeB;
uniform mat4 cubeDirAndPosition;
uniform mat4 cubeDirAndPositionInv;

uniform int screenWidth;
uniform int screenHeight;

uniform vec3 CameraPosition;

uniform sampler3D FbmPerlinTex3D;

// 将 3d 整数坐标映射为伪随机 vec3，用于 feature points 的生成 —— 各个网格点的随机数
vec3 hash3(vec3 p)
{
	p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
	p *= 17.0;
    return fract(p.x * p.y * p.z * (p + vec3(1.0, 2.0, 3.0)));
}

// 3D WorleyNoise，计算 3d worley noise 在 p 位置的取值
float worleyNoise3(vec3 p)
{
	vec3 cell = floor(p);	// 所在的 cell 网格
	float minDist = 1e9;
	// 遍历相邻的 cell
	for(int x=-1; x <= 1; ++x)
	{
		for(int y=-1; y <= 1; ++y)
		{
			for(int z=-1; z <= 1; ++z)
			{
				vec3 neighbor = cell + vec3(x, y, z); // 相邻点的真正位置
				vec3 fp = hash3(neighbor);
				// feature point 的世界坐标
				vec3 featurePos = neighbor + fp;

				// 距离
				float dist = length(featurePos - p);
				minDist = min(minDist, dist);
			}
		}
	}
    float maxDist = 0.8660254;
    return clamp(minDist / maxDist, 0.0, 1.0);
	//return minDist;
}

// // --- FBM (fractal sum) convenience wrapper
float fbmWorley3(vec3 p, int octaves, float lacunarity, float gain) {
    float amp = 1.0;
    float freq = 1.0;
    float sum = 0.0;
    float maxAmp = 0.0;
    for (int i = 0; i < octaves; ++i) {
        sum += amp * worleyNoise3(p * freq);
        maxAmp += amp;
        amp *= gain;
        freq *= lacunarity;
    }
    return sum / maxAmp; // normalized to [-1,1]
}

// 3d perlin noise

// 伪随机数
float hash1(vec3 p)
{
	p = fract(p * vec3(0.3183099, 0.3678794, 0.7071067) + 0.1f);
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

vec3 grad3(vec3 p)
{
	// 
	float h1 = hash1(p + vec3(0.0, 0.0, 0.0));
	float h2 = hash1(p + vec3(1.3, 7.1, 2.4));
	float h3 = hash1(p + vec3(8.2, 3.7, 5.6));

	vec3 g = normalize(vec3(h1 * 2.0 - 1.0, h2 * 2.0 - 1.0, h3 * 2.0 - 1.0));

	return g;
}

// classic fade function 6t^5 - 15t^4 + 10t^3
vec3 fade(vec3 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// perlin-like 3D noise returning in [-1,1]
float perlin3(vec3 p) {
    // lattice cell coordinates and local (0..1) position
    vec3 Pi = floor(p);
    vec3 Pf = fract(p);

    // compute fade weights
    vec3 w = fade(Pf);

    // gather dot products at 8 corners
    // corners: (0,0,0) .. (1,1,1)
    float n000 = dot(grad3(Pi + vec3(0.0,0.0,0.0)), Pf - vec3(0.0,0.0,0.0));
    float n100 = dot(grad3(Pi + vec3(1.0,0.0,0.0)), Pf - vec3(1.0,0.0,0.0));
    float n010 = dot(grad3(Pi + vec3(0.0,1.0,0.0)), Pf - vec3(0.0,1.0,0.0));
    float n110 = dot(grad3(Pi + vec3(1.0,1.0,0.0)), Pf - vec3(1.0,1.0,0.0));
    float n001 = dot(grad3(Pi + vec3(0.0,0.0,1.0)), Pf - vec3(0.0,0.0,1.0));
    float n101 = dot(grad3(Pi + vec3(1.0,0.0,1.0)), Pf - vec3(1.0,0.0,1.0));
    float n011 = dot(grad3(Pi + vec3(0.0,1.0,1.0)), Pf - vec3(0.0,1.0,1.0));
    float n111 = dot(grad3(Pi + vec3(1.0,1.0,1.0)), Pf - vec3(1.0,1.0,1.0));

    // trilinear interpolation using fade weights
    float nx00 = mix(n000, n100, w.x);
    float nx10 = mix(n010, n110, w.x);
    float nx01 = mix(n001, n101, w.x);
    float nx11 = mix(n011, n111, w.x);

    float nxy0 = mix(nx00, nx10, w.y);
    float nxy1 = mix(nx01, nx11, w.y);

    float nxyz = mix(nxy0, nxy1, w.z);

    // nxyz is roughly in [-sqrt(3)/2, sqrt(3)/2], scale to [-1,1] comfortably
    return clamp(nxyz * 1.6, 0.0, 1.0);
}

// --- FBM (fractal sum) convenience wrapper
float fbmPerlin3(vec3 p, int octaves, float lacunarity, float gain) {
    float amp = 1.0;
    float freq = 1.0;
    float sum = 0.0;
    float maxAmp = 0.0;
    for (int i = 0; i < octaves; ++i) {
        sum += amp * perlin3(p * freq);
        maxAmp += amp;
        amp *= gain;
        freq *= lacunarity;
    }
    return sum / maxAmp; // normalized to [-1,1]
}

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

    return sdBox(vec3(localPos.xyz), vec3(cubeB));
}

float cubeIntersectLength(vec3 ro, vec3 rd, float edgeLength, out float tMin, out float tMax)
{
    float txMin = -1e9; float txMax = 1e9;
    float tyMin = -1e9; float tyMax = 1e9;
    float tzMin = -1e9; float tzMax = 1e9;
    if(rd.x != 0)
    {
        float tx1 = ( edgeLength - ro.x) / rd.x;    
        float tx2 = (-edgeLength - ro.x) / rd.x;    
        txMin = min(tx1, tx2);
        txMax = max(tx1, tx2);
    }
    if(rd.y != 0)
    {
        float ty1 = ( edgeLength - ro.y) / rd.y;    
        float ty2 = (-edgeLength - ro.y) / rd.y;    
        tyMin = min(ty1, ty2);
        tyMax = max(ty1, ty2);
    }
    if(rd.z != 0)
    {
        float tz1 = ( edgeLength - ro.z) / rd.z;    
        float tz2 = (-edgeLength - ro.z) / rd.z;    
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


vec4 raymarch(vec3 rayOrigin, vec3 rayDirection, float MARCH_SIZE, float startDepth) {
  float depth = startDepth;
  vec3 p = rayOrigin + depth * rayDirection;
  
  vec4 res = vec4(0.0);

  for (int i = 0; i < 100; i++) {
    vec3 samplePos = (p + vec3(1)) / 2.0f;
    
    float density = texture(FbmPerlinTex3D,samplePos).r;

    // We only draw the density if it's greater than 0
    if (density > 0.0) {
      vec4 color = vec4(mix(vec3(1.0,1.0,1.0), vec3(0.0, 0.0, 0.0), density), density );
      color.rgb *= color.a;
      res += color*(1.0-res.a);
    }

    depth += MARCH_SIZE;
    p = rayOrigin + depth * rayDirection;
  }

  return res;
}


void main(){
    vec4 col = vec4(0.0f);

	vec3 pos = vec3(TexCoord.x + currentTime*0.0, TexCoord.y, TexCoord.z);
    
    
    vec2 resolution = vec2(screenWidth, screenHeight);
    vec2 ndc = (gl_FragCoord.xy / resolution) * 2  - 1.0f;
    vec4 clip = vec4(ndc, -1.0f, 1.0);      // [-1,1]的cube 空间里面， z= -1 就对应摄像机的近平面了
    
    // 在摄像机的视角下，使用逆投影恢复原来的坐标
    vec4 eye = projectionInverse * clip;
    
    // 以为此时是以摄像机为原点了，所以 eye ，也就是近平面上的点的位置，归一化后就可以作为 ray marching 的方向了
    vec3 rayDir = normalize(eye.xyz);
    
    
    // 在立方体的坐标下的光位置和光方向
    vec3 rayOrigin = vec3(cubeDirAndPositionInv * vec4(0,0,0,1));
    rayDir = normalize(vec3(cubeDirAndPositionInv * vec4(rayDir,0)));
    //

    
    
    float tMin = 0.0f;
    float tMax = 0.0f;
    
    float d = cubeIntersectLength(rayOrigin, rayDir, cubeB, tMin, tMax);
    
    int NumSteps = 30;
    float tStep = d / NumSteps;
    float transmittance = 1.0f;
    vec4 tmpCol = vec4(0);
    
    float scale = 0.5;
    for(int i=0; i < NumSteps; ++i)
    {
        // 取值范围是 [-1,1] 的cube
        vec3 samplePos = rayOrigin + rayDir * (tMin + i * tStep);
        // 变到 [0,1]的区间
        samplePos = scale * (samplePos + vec3(cubeB)) / 2.0f / cubeB;
        samplePos = fract(samplePos + vec3(currentTime*0.1,0,0));


        vec4 vals = texture(FbmPerlinTex3D,samplePos);
        float density = vals.r;
    
        if(density > 0.01)
        {
            float absorb = exp(-1.7 *tStep* density);
            transmittance *= absorb;
            
            tmpCol += transmittance * density * vec4(1);
    
            
        }
        
        if(transmittance < 0.01)
        {
            break;
        }
    }
    
    col = vec4(tmpCol.xyz, 1 - transmittance);



	gl_FragColor = col;

}

