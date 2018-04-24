#version 430 core
centroid in vec3 Position_FS_in;
centroid in vec3 Normal_FS_in;
centroid in vec2 Texcoord_FS_in;
layout(location = 0) out vec4 FragColor;
uniform float index;
uniform vec3 EyePosition;
uniform samplerCube skybox;
//para
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
	float ratio = 1.00 / index;
	//float F = ((1.0 - ratio) * (1.0 - ratio)) / ((1.0 + ratio) * (1.0 + ratio));
	//float FresnelPower = 20.0;
	vec3 N = normalize(Normal_FS_in);
	vec3 I = normalize(Position_FS_in - EyePosition);
	//float fresnelCoe = F + (1.0 - F) * pow((1.0 - dot(-I, N)), FresnelPower);
	//fresnelCoe = clamp(fresnelCoe, 0, 1); 

	vec3 Rf = reflect(I, N);
	vec4 colorRf = texture(skybox, Rf);
	vec3 R = refract(I, N, ratio);
	vec4 colorR = texture(skybox, R);

	if (hasLocalShading)
	{
		FragColor = vec4(localShading(), 0.0) + mix(colorR, colorRf, 0.5);
	}
	else
		FragColor = mix(colorR, colorRf, 0.5);
}