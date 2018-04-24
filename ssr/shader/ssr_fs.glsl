#version 400 core

in vec3 Position_FS_in;
in vec3 Normal_FS_in;
in vec2 Texcoord_FS_in;
in float Distance_FS_in;
layout(location = 0) out vec4 FragColor;
uniform float index;
uniform vec3 EyePosition;
uniform mat4 gViewProjectMatrix;
uniform sampler2D backWorldPos;
uniform sampler2D backWorldNorm;
uniform samplerCube skybox;
//parameter
const int maxLights = 10;
struct light{
	int type;
	vec3 position;
	vec3 color;
	float fade;
};
uniform bool hasLocalShading;
uniform light lights[maxLights];
uniform int numLights;

vec3 localShading()
{
	float Kd = 0.1;
	float Ks = 0.6;
	float glossness = 100;

	vec3 scatteredLight = vec3(0);
	vec3 reflectedLight = vec3(0);

	for (int i = 0; i < numLights; i++)
	{
		vec3 l = vec3(0);
		float attenuation = 1.0;
		if (lights[i].type == 0)
		{
			l = normalize(-lights[i].position);
		}
		else
		{
			l = lights[i].position - Position_FS_in.xyz;
			attenuation = 1.0 / (1 + lights[i].fade*length(l));
			l = normalize(l);
		}

		vec3 n = normalize(Normal_FS_in);
		vec3 v = normalize(EyePosition - Position_FS_in.xyz);
		vec3 h = normalize(l + v);

		float diffuse = max(0.0, dot(n, l));
		float specular = max(0.0, dot(n, h));
		if (diffuse < 0)
			specular = 0.0;
		else
			specular = pow(specular, glossness);

		scatteredLight += lights[i].color * diffuse * attenuation;
		reflectedLight += lights[i].color * specular * attenuation;
	}
	return Kd * scatteredLight + Ks *reflectedLight;
}

void main(void)
{
	//
	vec4 ProjCoord = gViewProjectMatrix * vec4(Position_FS_in, 1.0);
	vec3 ProjCoord3 = ProjCoord.xyz / ProjCoord.w;
	vec2 texcoord = 0.5 * (ProjCoord3.xy + vec2(1.0, 1.0));

	float ratio = 1.00 / index;
	//float F = ((1.0 - ratio) * (1.0 - ratio)) / ((1.0 + ratio) * (1.0 + ratio));
	//float FresnelPower = 5.0;
	vec3 N = normalize(Normal_FS_in);
	vec3 I = normalize(Position_FS_in - EyePosition);
	//float fresnelCoe = F + (1.0 - F) * pow((1.0 - dot(-I, N)), FresnelPower);
	//fresnelCoe = clamp(fresnelCoe, 0, 1);

	vec3 Rf = reflect(I, N);
	Rf = normalize(Rf);
	vec4 colorRf = texture(skybox, Rf);
	//calc refract

	vec3 T1 = refract(I, N, ratio);
	T1 = normalize(T1);

	float T1sin_in = length(cross(I, N));
	float T1theta_in = abs(asin(T1sin_in));
	float T1sin_out = T1sin_in * ratio; 
	float T1theta_out = abs(asin(T1sin_out));

	//计算P2
	vec3 backPoint = texture(backWorldPos, texcoord).xyz;
	if (backPoint == vec3(0, 0, 0))
	{
		backPoint = Position_FS_in;
	}
	float dv = distance(backPoint, Position_FS_in);
	float dn = Distance_FS_in;
	float ratio_theta = T1theta_out / T1theta_in;
	float d = ratio_theta*dv + (1 - ratio_theta)*dn;
	vec3 P2 = Position_FS_in + d * T1;

	//投影P2到屏幕空间
	vec4 ProjCoord_P2 = gViewProjectMatrix * vec4(P2, 1.0);
	vec3 ProjCoord3_P2 = ProjCoord_P2.xyz / ProjCoord_P2.w;
	vec2 texcoord_P2 = 0.5 * (ProjCoord3_P2.xy + vec2(1.0, 1.0));
	vec3 N2 = texture(backWorldNorm, texcoord_P2).xyz;
	if (N2 == vec3(0, 0, 0))
	{
		N2 = normalize(T1 - dot(I, T1)* I);
	}

	N2 = -N2;

	vec3 Rf2 = reflect(T1, N2);
	Rf2 = normalize(Rf2);
	vec4 colorRf2 = texture(skybox, Rf2);

	vec4 colorR2=vec4(0);
	float t1DotN2 = dot(-T1, N2);
	float sqr = 1 - index*index*(1 - t1DotN2*t1DotN2);
	if (sqr > 0)
	{
		vec3 T2 = refract(T1, N2, index);
		colorR2 = texture(skybox, T2);
	}
	vec4 colorR1 = mix(colorR2, colorRf2, 0.5);
	if (hasLocalShading)
	{
		FragColor = vec4(localShading(), 0.0) + mix(colorR1, colorRf, 0.5);
	}
	else
		FragColor = mix(colorR1, colorRf, 0.5);
}