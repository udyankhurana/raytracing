#include<iostream>
#include<stdio.h>
#include<time.h>
#include<math.h>
#define INT_MIN -2147483647
#define EPSILON 0.000001

class vector
{	public:
	float x,y,z;
	vector(): x(0.0),y(0.0),z(0.0) {}
	vector(float a,float  b,float c): x(a),y(b),z(c) {}	//constructor
	vector operator * (const float &a) { return vector(x*a,y*a,z*a) ; }	//scalar multiplication
	vector operator * (const vector &a) { return vector(x*a.x,y*a.y,z*a.z) ; }	//vector multiplication
	vector operator + (const vector &a) { return vector(x+a.x,y+a.y,z+a.z) ; }	// vector addition
	vector operator - (const vector &a) { return vector(x-a.x,y-a.y,z-a.z) ; }	//vector subtraction
	float mag()	//magnitude of vector
	{	return sqrt(x*x + y*y + z*z);	}
	vector normalize()
	{	float m=mag();
		return vector(x/m,y/m,z/m);
	}
	float dot(vector v)	//dot product with another vector
	{	float d = x*v.x+ y*v.y+ z*v.z;
		return d;
	}
	vector cross(vector v)	//cross product with another vector
	{	float newx,newy,newz;
		newx= y*v.z  - z*v.y;
		newy= z*v.x  - x*v.z;
		newz= x*v.y  - y*v.x;
		return vector(newx,newy,newz);	
	}
	friend std::ostream & operator << (std::ostream &os, const vector &v) //print in vector form
	{	os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
};

struct tri
{	vector a,b,c;	//for the 3 vertices
	int color;	//value 0-255
};

struct sphere
{	vector centre;
	float radius;
	int color;
};

class ray
{	public: 
	vector rayorig,raydir;	//origin and direction of ray
	float t;	// as ray R = origin + t * direction
	ray(vector x,vector y): rayorig(x), raydir(y), t(0.0) {}
	float triangle_intersect(tri t)	//Moller-Trumbore algorithm
	{	vector v1,v2;	//calculate vectors for side edges
		v1 = t.b - t.a;
		v2 = t.c - t.a;
		vector p = raydir.cross(v2);
		float det = p.dot(v1);
		if(det > -EPSILON && det < EPSILON) //means vector is parallel to the plane (int. at inifinity)
			return 0;
		float inv_det = 1.0/det;
		vector T = rayorig - t.a;
		float U = inv_det * T.dot(p);	//U,V: barycentric coordinates
		if (U <0.0 || U > 1.0) //the intersection is outside the triangle
			return 0;
		vector q = T.cross(v1);
		float V = raydir.dot(q) * inv_det;
		if (V <0.0 || U+V > 1.0) 
			return 0;
		float param = v2.dot(q) * inv_det;
		if(param > EPSILON)
			return param;	//the 't' value returned if intersection found
		return 0;
	}
	float sphere_intersect(sphere s)
	{	float a,b,c;
		vector diff = rayorig - s.centre;
		a=raydir.dot(raydir);
		b=2*(raydir.dot(diff));
		c=diff.dot(diff) - s.radius*s.radius;
		float disc = b*b - 4*a*c;
		if (disc == 0)
			return (-(b/(2*a)));
		else if(disc < 0)
			return 0;
		else
		{	float r1,r2;
			r1 = (-b + sqrt(disc))/(2*a);
			r2 = (-b - sqrt(disc))/(2*a);
			if(r1 < r2)
				return r1;
			else
				return r2;
		}
	}
};

int main() 
{	
	int width, height;
	int numtris,numspheres,i,j,k,color;
	int bgcolor=10;
	// Allocate storage for output image and triangle array
	unsigned int pixelgrid[500][500]={0};	//so width,height limit = 250
	tri triarray[50];
	sphere spherearray[50];

	struct timespec t1, t2;
	clock_gettime(CLOCK_MONOTONIC, &t1);
	float xx,yy,zz;

	//Read Input
	scanf("%d %d",&height,&width);	//height=rows, width=cols
	scanf("%d",&numtris);
	for(i=0;i<numtris;i++)
	{	scanf("%d",&triarray[i].color);
		scanf("%f %f %f",&xx,&yy,&zz);	//pt 1
		triarray[i].a = vector(xx,yy,zz);
		scanf("%f %f %f",&xx,&yy,&zz); //pt 2
		triarray[i].b = vector(xx,yy,zz);
		scanf("%f %f %f",&xx,&yy,&zz); //pt 3
		triarray[i].c = vector(xx,yy,zz);
	}
	scanf("%d",&numspheres);
	for(i=0;i<numspheres;i++)
	{	scanf("%d",&spherearray[i].color);
		scanf("%f %f %f",&xx,&yy,&zz);	//centre
		spherearray[i].centre = vector(xx,yy,zz);
		scanf("%f",&spherearray[i].radius); //radius
	}
	vector orig = vector(height/2,width/2,25);	//Fixed Origin: Simulating a Pin-hole camera
	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{	vector dir = vector(float(i),float(j),float(0));
			dir = dir - orig; //direction vector
			dir = dir.normalize();
			ray r = ray(orig,dir);
			for(k=0;k<numtris;k++)
			{	float x = r.triangle_intersect(triarray[k]);	//this is the 't' parameter
				if(x!=0)
				{	if(r.t!=0.0 && x < r.t || r.t==0.0) //either no pvs. intersection or closer intersection found 
					{	pixelgrid[i][j] = triarray[k].color;
						r.t = x;
					}
				}
			}
			for(k=0;k<numspheres;k++)
			{	float x = r.sphere_intersect(spherearray[k]);	//this is the 't' parameter
				if(x!=0)
				{	if(r.t!=0.0 && x < r.t || r.t==0.0) //either no pvs. intersection or closer intersection found 
					{	pixelgrid[i][j] = spherearray[k].color;
						r.t = x;
					}
				}
			}
			if(pixelgrid[i][j]==0)
				pixelgrid[i][j] = bgcolor;
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &t2);	//calculating time for the above process
	double time = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	printf("Time: %lf ms\n", time);

	FILE *fo = fopen("output.pgm", "w");	//writing final pixel grid to output file
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);
	return 0;
}
