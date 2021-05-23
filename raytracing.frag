#version 430

in vec3 interpolatedVertex;
out vec4 FragColor;

struct Camera
{
	vec3 position;
	vec3 view;
	vec3 up;
	vec3 side;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Sphere
{
	vec3 center;
	float radius;
	vec3 color;
};

struct Triangle
{
	vec3 v1;
	vec3 v2;
	vec3 v3;
	vec3 color;
};

struct Material
{
	float ambient;
	float diffuse;
	float specular;
	float specularPower;
	float ReflectionCoef;
	int MaterialType;
};

struct Intersection
{
	float time;
	vec3 point;
	vec3 normal;
	vec3 color;
	float ReflectionCoef;
	int MaterialType;
};

struct TracingRay
{
	Ray ray;
	float contribution;
	int depth;
};

int currentpos = 0;
TracingRay stack[200];

	void pushRay(TracingRay newRay){
		stack[currentpos] = newRay;
		++currentpos;
	}

	TracingRay popRay(){
		--currentpos;
		return stack[currentpos];
	}

	bool isEmpty(){
		return currentpos == 0;
	}

const int countTriangles = 14;
const int countSpheres = 2;

const int DIFFUSE_REFLECTION = 1;
const int MIRROR_REFLECTION = 2;

Material material = {0.4, 0.5, 0.0, 512.0, 0.5, DIFFUSE_REFLECTION };
vec3 lightPos = vec3(4.5, 4.5, -4.5);

uniform Camera camera;
uniform vec2 scale;
uniform Sphere spherein[countSpheres];
uniform Triangle trianglein[countTriangles];

bool IntersectSphere (Sphere sphere, Ray ray, float start, float final, out float time)
{
	ray.origin -= sphere.center;
	float A = dot (ray.direction, ray.direction);
	float B = dot (ray.direction, ray.origin);
	float C = dot (ray.origin, ray.origin) - sphere.radius * sphere.radius;
	float D = B * B - A * C;
	if (D > 0.0)
	{
		D = sqrt(D);
		float t1 = (-B - D) / A;
		float t2 = (-B + D) / A;
		
		if (t1 < 0 && t2 < 0) 
			return false;
		
		if (min(t1, t2) < 0)
		{
			time = max(t1, t2);
			return true;
		}
		
		time = min(t1, t2);
		
		return true;
	}
	
	return false;
};

bool IntersectTriangle(Ray ray, vec3 v1, vec3 v2, vec3 v3, out float time)
{
	time = -1;
	vec3 edge1 = v2 - v1;
	vec3 edge2 = v3 - v1; 
	vec3 normal = cross(edge1, edge2);
	float dotRayDir = dot(normal, ray.direction);
	
	if (abs(dotRayDir) < 0.001)
		return false;
	
	float d = dot(normal, v1);
	float t = -(dot(normal, ray.origin) - d) / dotRayDir;
	
	if (t < 0) 
		return false; 
		
	vec3 p = ray.origin + t * ray.direction;
	vec3 pv1 = p - v1;
	vec3 c = cross(edge1, pv1);
	
	if (dot(normal, c) < 0)
		return false;
	
	edge2 = v3 - v2;
	vec3 pv2 = p - v2;
	c = cross(edge2, pv2);
	
	if (dot(normal, c) < 0)
		return false;
	
	vec3 edge3 = v1 - v3;
	vec3 pv3 = p - v3;
	c = cross(edge3, pv3);
	
	if (dot(normal, c) < 0)
		return false;
		
	time = t;
	
	return true; 
};

bool Intersect(Ray ray, float start, float final, inout Intersection intersect)
{
	bool result = false;
	float time = start;
	intersect.time = final;
	
	for (int i = 0; i < countSpheres; i++)
	{
		if (IntersectSphere(spherein[i], ray, start, final, time) && time < intersect.time)
		{
			intersect.time = time;
			intersect.point = ray.origin + ray.direction * time;
			intersect.normal = normalize(intersect.point - spherein[i].center);
			intersect.color = spherein[i].color;
			intersect.ReflectionCoef = 1.5;
			intersect.MaterialType = DIFFUSE_REFLECTION;
			result = true;
		}
	}

	for (int i = 0; i < countTriangles; i++)
	{
		if (IntersectTriangle(ray, trianglein[i].v1, trianglein[i].v2, trianglein[i].v3, time) && time < intersect.time)
		{
			intersect.time = time;
			intersect.point = ray.origin + ray.direction * time;
			vec3 edge1 = trianglein[i].v1 - trianglein[i].v2;
			vec3 edge2 = trianglein[i].v1 - trianglein[i].v3;
			intersect.normal = normalize(cross(edge1, edge2));
			intersect.color = trianglein[i].color;
			intersect.ReflectionCoef = 0.1;
			intersect.MaterialType = DIFFUSE_REFLECTION;
			result = true;
		}
	}
	return result;
};

vec3 Phong(Intersection intersect, float shadow)
{
	vec3 light = normalize(lightPos - intersect.point);
	float diffuse = max(dot(light, intersect.normal), 0.0);
	vec3 view = normalize(camera.position - intersect.point);
	vec3 reflected = reflect(view, -intersect.normal);
	float specular = pow(max(dot(reflected, light), 0.0), material.specularPower);
	
	return material.ambient * intersect.color + shadow * 
		   (material.diffuse * diffuse * intersect.color +
		   material.specular * specular * vec3(1,1,1));
};

float Shadow(vec3 point)
{
	float light = 1.0;
	vec3 direction = normalize(lightPos - point);
	Ray shadowRay = Ray(point + direction * 0.001, direction);
	Intersection intersect;
	
	intersect.time = distance(lightPos, point);
	
	if (Intersect(shadowRay, 0, intersect.time, intersect))
		light = 0.0;
	
	return light;
};

vec4 Raytrace(Ray primary_ray)
{
	vec4 resultColor = vec4(0, 0, 0, 0);
	Ray ray = primary_ray;
	
	Intersection intersect;
	intersect.time = 10000.0;
	float start = 0;
	float final = 10000.0;
	
	if (Intersect(ray, start, final, intersect))
	{
		float shadowing = Shadow(intersect.point);
		resultColor += vec4(Phong(intersect, shadowing), 0.0);
	}
	return resultColor;
};

Ray GenerateRay(Camera camera)
{
	vec2 coords = interpolatedVertex.xy * normalize(scale);
	vec3 direction = camera.view + camera.side * coords.x + camera.up * coords.y;
	return Ray(camera.position, normalize(direction));
};

void main(void){

	vec3 resultColor = vec3(0,0,0);
	Ray ray = GenerateRay(camera);
	TracingRay trRay = TracingRay(ray, 1, 0);
	pushRay(trRay);

	while(!isEmpty())
	{
		TracingRay trRays = popRay();
		ray = trRay.ray;
		Intersection intersect;
		intersect.time = 10000.0;
		float start = 0;
		float final = 10000.0;
		if (Intersect(ray, start, final, intersect) && trRays.depth < 10)
		{
			switch(intersect.MaterialType)
			{
				case DIFFUSE_REFLECTION:
				{
					float shadowing = Shadow(intersect.point);
					resultColor += trRay.contribution * Phong(intersect, shadowing);
					break;
				}
				case MIRROR_REFLECTION:
				{
					if(intersect.ReflectionCoef < 1)
					{
						float contribution = trRay.contribution * (1 -
						intersect.ReflectionCoef);
						float shadowing = Shadow(intersect.point);
						resultColor += contribution * Phong(intersect, shadowing);
					}
					vec3 reflectDirection = reflect(ray.direction, intersect.normal);
					// creare reflection ray
					float contribution = trRay.contribution * intersect.ReflectionCoef;
					Ray ray_local;
					ray_local.origin = intersect.point + reflectDirection * 0.001;
					ray_local.direction =reflectDirection;

					TracingRay reflectRay = TracingRay(
					ray_local,
					contribution, trRay.depth + 1);
					pushRay(reflectRay);
					break;
				}
			} // switch
		} // if (Raytrace(ray, start, final, intersect))
	} // while(!isEmpty())
	

	FragColor = vec4 (resultColor, 0.0);
};