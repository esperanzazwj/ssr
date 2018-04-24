#version 430 core
#define FLT_MAX 100000
#define TMIN 0.001
in vec2 TexCoord_FS_in;
layout(location = 0) out vec4 FragColor;

struct Camera
{
	vec3 pos;
	float nearz;
	float aspect;
	vec3 direction;
	vec3 horizontal;
	vec3 up;
};
struct Ray
{
	vec3 org;
	vec3 dir;
};
struct Sphere
{
	vec3 center;
	float radius;
};
struct Hit
{
	float t;
	vec3 normal;
};
uniform float index;//折射率
uniform Camera camera;
uniform samplerCube skybox;//环境贴图
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

vec3 localShading(vec3 normal, vec3 position, vec3 eye)
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
			l = lights[i].position - position;
			attenuation = 1.0 / (1 + lights[i].fade*length(l));
			l = normalize(l);
		}

		vec3 n = normalize(normal);
		vec3 v = normalize(eye - position);
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
layout(std430, binding = 0) buffer verticesData
{
	vec4 vertices[];
};
layout(std430, binding = 1) buffer normalsData
{
	vec4 normals[];
};
layout(std430, binding = 2) buffer indicesData
{
	uvec4 indices[];
};
Ray generateRay(vec2 point)
{
	point = point * 2 - 1.0;
	vec3 dir = camera.nearz * camera.direction + point.x * camera.horizontal * camera.aspect + point.y*camera.up;
	dir = normalize(dir);
	return Ray(camera.pos,dir);
}
vec3 pointAtParameterOnRay(Ray ray, float t)
{
	return ray.org + ray.dir*t;
}
Sphere generateSphere(vec3 center, float radius)
{
	return Sphere(center, radius);
}
Hit generateHit()
{
	return Hit(FLT_MAX, vec3(0, 0, 0));
}
bool intersectSphere(Sphere sphere, Ray ray, inout Hit hit)
{
	float delta = pow(dot(ray.dir, ray.org - sphere.center), 2) - dot(ray.dir,ray.dir)*(dot(ray.org - sphere.center, ray.org - sphere.center) - sphere.radius*sphere.radius);
	if (delta<0) return false;
	delta = sqrt(delta);
	float t = FLT_MAX;
	float t1 = (-dot(ray.dir, ray.org - sphere.center) + delta) / dot(ray.dir, ray.dir);
	if (t1>TMIN && t1<t) t = t1;
	float t2 = (-dot(ray.dir, ray.org - sphere.center) - delta) / dot(ray.dir, ray.dir);
	if (t2>TMIN && t2<t) t = t2;
	if (t == FLT_MAX) return false;
	if (t<hit.t)
	{
		vec3 normal = normalize(pointAtParameterOnRay(ray,t) - sphere.center);
		hit.t = t; hit.normal = normal;
	}
	return true;
}

bool intersectMesh(Ray ray, inout Hit hit)
{
	bool flag = false;
	for (uint idx = 0; idx < indices.length(); idx++)
	{
		vec3 org = ray.org;
		vec3 dir = ray.dir;
		vec3 d1 = vertices[indices[idx].x].xyz;
		vec3 d2 = vertices[indices[idx].y].xyz;
		vec3 d3 = vertices[indices[idx].z].xyz;
		mat3 A = mat3(d1.x - d2.x, d1.y - d2.y, d1.z - d2.z, d1.x - d3.x, d1.y - d3.y, d1.z - d3.z, dir.x, dir.y, dir.z);
		vec3 b = vec3(d1.x - org.x, d1.y - org.y, d1.z - org.z);
		if (abs(determinant(A)) < 1e-08)
			continue;
		mat3 Ainv = inverse(A);
		vec3 v = Ainv*b;
		if (0 <= v.x && 0 <= v.y && v.x + v.y <= 1 && v.z >= TMIN)
		{
			if (v.z < hit.t)
			{
				hit.t = v.z;
				hit.normal =  (1 - v.x - v.y)*normals[indices[idx].x].xyz + v.x * normals[indices[idx].y].xyz + v.y * normals[indices[idx].z].xyz;
			}
			flag = true;
		}
	}
	return flag;
}

void main()
{
	float ratio = 1.00 / index;
	//float F = ((1.0 - ratio) * (1.0 - ratio)) / ((1.0 + ratio) * (1.0 + ratio));
	//float FresnelPower = 5.0;
	Ray ray = generateRay(TexCoord_FS_in);
	//Sphere sphere = generateSphere(vec3(10, 0, 0), 1);
	Hit hit = generateHit();

	if (intersectMesh(ray, hit))
	{
		vec3 hitp1 = pointAtParameterOnRay(ray, hit.t);
		vec3 localshade1 = localShading(hit.normal, hitp1, camera.pos);

		vec3 Rf1 = reflect(ray.dir, hit.normal);
		Rf1 = normalize(Rf1);
		//for reflect 1  not tracing
		vec4 colorRf1 = texture(skybox, Rf1);
		vec4 colorR1 = vec4(0);
		//for refract 1 tracing
		vec3 R1 = refract(ray.dir, hit.normal, ratio);
		R1 = normalize(R1);
		//FragColor=vec4(R1, 1.0); return;
		Ray ray1 = Ray(hitp1, R1);
		Hit hit1 = generateHit();
		if (intersectMesh(ray1, hit1))//first intersect
		{
			
			vec3 hitp2 = pointAtParameterOnRay(ray1, hit1.t);
			vec3 localshade2 = localShading(hit1.normal, hitp2, camera.pos);

			float ratio1 = ratio;
			if (dot(ray1.dir, hit1.normal) > 0)
			{
				hit1.normal = -hit1.normal;
				ratio1 = index;
			}
			vec3 Rf2 = reflect(ray1.dir, hit1.normal);
			Rf2 = normalize(Rf2);
			vec4 colorRf2 = texture(skybox, Rf2); //for reflect2 no tracing

			/*Ray ray2 = Ray(hitp2, Rf2);
			Hit hit2 = generateHit();
			if (intersectMesh(ray2, hit2))
			{
				vec3 hitp3 = pointAtParameterOnRay(ray2, hit2.t);
				vec3 localshade3 = localShading(hit2.normal, hitp3, camera.pos);

				float ratio2 = ratio;
				if (dot(ray2.dir, hit2.normal) > 0)
				{
					hit2.normal = -hit2.normal;
					ratio2 = index;
				}
				vec3 Rf3 = reflect(ray2.dir, hit2.normal); 
				Rf3 = normalize(Rf3);
				vec4 colorRf3 = texture(skybox, Rf3); //for reflect3 no tracing

				vec4 colorR3 = vec4(0);
				float r2DotH2N = dot(ray2.dir, hit2.normal);
				float sqrr = 1 - ratio2*ratio2*(1 - r2DotH2N*r2DotH2N);
				if (sqrr >= 0)
				{
					vec3 R3 = refract(ray2.dir, hit2.normal, ratio2);
					R3 = normalize(R3);
					colorR3 = texture(skybox, R3); //for refract3 no tracing
				}
				if (hasLocalShading)
					colorRf2 = vec4(localshade3, 0) + mix(colorR3, colorRf3, 0.5);
				else
					colorRf2 = mix(colorR3, colorRf3, 0.5);

			}
			else
			{
				colorRf2 = texture(skybox, Rf2);
			}
			*/
			
			vec4 colorR2 = vec4(0);
			float r1DotH1N = dot(-ray1.dir, hit1.normal);
			float sqr = 1 - ratio1*ratio1*(1 - r1DotH1N*r1DotH1N);
			if (sqr >= 0)
			{
				vec3 R2 = refract(ray1.dir, hit1.normal, ratio1);
				R2 = normalize(R2);
				/*colorR2 = vec4(0);
				Ray ray2 = Ray(hitp2, R2);
				Hit hit2 = generateHit();
				if (intersectMesh(ray2, hit2))
				{
					vec3 hitp3 = pointAtParameterOnRay(ray2, hit2.t);
					vec3 localshade3 = localShading(hit2.normal, hitp3, camera.pos);

					float ratio2 = ratio;
					if (dot(ray2.dir, hit2.normal) > 0)
					{
						hit2.normal = -hit2.normal;
						ratio2 = index;
					}
					vec3 Rf3 = reflect(ray2.dir, hit2.normal);
					Rf3 = normalize(Rf3);
					vec4 colorRf3 = texture(skybox, Rf3); //for reflect3 no tracing

					vec4 colorR3 = vec4(0);
					float r2DotH2N = dot(ray2.dir, hit2.normal);
					float sqrr = 1 - ratio2 * ratio2 * (1 - r2DotH2N*r2DotH2N);
					if (sqrr >= 0)
					{
						vec3 R3 = refract(ray2.dir, hit2.normal, ratio2);
						R3 = normalize(R3);
						colorR3 = texture(skybox, R3); //for refract3 no tracing
					}
					if (hasLocalShading)
						colorR2 = vec4(localshade3, 0) + mix(colorR3, colorRf3, 0.5);
					else
						colorR2 = mix(colorR3, colorRf3, 0.5);
				}
				else
				{
					colorR2 = texture(skybox, R2);
				}
				*/
				colorR2 = texture(skybox, R2); //for reflect2 no tracing
			}

			if (hasLocalShading)
				colorR1 = vec4(localshade2, 0) + mix(colorR2, colorRf2, 0.5);
			else
				colorR1 = mix(colorR2, colorRf2, 0.5);
		}
		else
		{
			colorR1 = texture(skybox, R1);
		}
		if (hasLocalShading)
			FragColor = vec4(localshade1, 0) + mix(colorR1, colorRf1, 0.5);
		else
			FragColor = mix(colorR1, colorRf1, 0.5);
	}
	else
	{
		FragColor = texture(skybox, ray.dir);
	}

}


