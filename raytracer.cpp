#include<iostream>
#include<limits>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#define EPSILON 0.000001
#define MIN(a,b) (((a)<(b))?(a):(b))

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

class ray
{	public: 
	vector rayorig,raydir;	//origin and direction of ray
	float tmin,tmax;	// as ray R = origin + t * direction
	int id;	// to check if intersection happens or not (if yes, with which shape number it happens)
	ray(vector x,vector y): rayorig(x), raydir(y), tmin(0.0), tmax(std::numeric_limits<float>::max()), id(-1) {}
};

class shape
{	public:
	int shapeid,color;	//can be made public also
	virtual bool intersect(ray &r) = 0;	//virtual function to get intersection
	shape(int col,int id): color(col), shapeid(id) {}
};

class triangle: public shape
{	public:
	vector a,b,c;	//for the 3 vertices
	triangle(vector x,vector y,vector z,int col,int id): shape(col,id), a(x), b(y), c(z) {}
	~triangle() {};
	bool intersect(ray &r)	//Moller-Trumbore algorithm
	{	vector v1,v2;	//calculate vectors for side edges
		v1 = b - a;
		v2 = c - a;
		vector p = (r.raydir).cross(v2);
		float det = p.dot(v1);
		if(det > -EPSILON && det < EPSILON) //means vector is parallel to the plane (int. at inifinity)
			return false;
		float inv_det = 1.0/det;
		vector T = r.rayorig - a;
		float U = inv_det * T.dot(p);	//U,V: barycentric coordinates
		if (U <0.0 || U > 1.0) //the intersection is outside the triangle
			return false;
		vector q = T.cross(v1);
		float V = (r.raydir).dot(q) * inv_det;
		if (V <0.0 || U+V > 1.0) 
			return false;
		float param = v2.dot(q) * inv_det;
		if(param > EPSILON && param < r.tmax)
		{	r.tmax = param;	//the 't' value assigned if intersection found
			r.id = shapeid;
			return true;
		}
		return false;
	}
};

struct sphere: public shape
{	public:
	vector centre;
	float radius;
	sphere(vector c,float r,int col,int id): shape(col,id), centre(c), radius(r) {}
	~sphere() {};
	bool intersect(ray &r)
	{	float a,b,c,ans;
		vector diff = r.rayorig - centre;
		a=(r.raydir).dot(r.raydir);
		b=2*((r.raydir).dot(diff));
		c=diff.dot(diff) - radius*radius;
		float disc = b*b - 4*a*c;
		if (disc == 0)
			ans = (-(b/(2*a)));
		else if(disc < 0)
			return false;
		else
		{	float r1,r2;
			r1 = (-b + sqrt(disc))/(2*a);
			r2 = (-b - sqrt(disc))/(2*a);
			ans = MIN(r1,r2);
		}
		if(ans < r.tmax)
		{	r.tmax = ans;
			r.id = shapeid;
			return true;
		}
	}
};

int main() 
{	
	int width, height;
	int numshapes,i=0,j=0,k=0,color,bgcolor=10;
	// Allocate storage for output image and triangle array
	unsigned int pixelgrid[500][500]={0};	//width,height limit = 500
	char type[25];
	shape *shapes[50];
	vector a = vector(0,0,0);
	triangle t[10] = triangle(a,a,a,0,0);
	sphere s[10] = sphere(a,0,0,0);

	struct timespec t1, t2;
	clock_gettime(CLOCK_MONOTONIC, &t1);
	float xx,yy,zz;

	//Read Input
	scanf("%d %d",&height,&width);	//height=rows, width=cols
	scanf("%d",&numshapes);
	for(i=0;i<numshapes;i++)
	{	scanf("%s",type);
		if(!strcmp(type,"Triangle"))
		{	int col;
			vector x,y,z;
			scanf("%d",&col);
			scanf("%f %f %f",&xx,&yy,&zz);	//pt 1
			x = vector(xx,yy,zz);
			scanf("%f %f %f",&xx,&yy,&zz); //pt 2
			y = vector(xx,yy,zz);
			scanf("%f %f %f",&xx,&yy,&zz); //pt 3
			z = vector(xx,yy,zz);
			t[j] = triangle(x,y,z,col,i);
			shapes[i] = &t[j++]; 
			/* What we did earlier: 
			tiangle t = triangle(x,y,z,col,i);
			shapes[i] = &t; 
			*/   
		}
		else if(!strcmp(type,"Sphere"))
		{	int col; vector c; float r;
			scanf("%d",&col);
			scanf("%f %f %f",&xx,&yy,&zz);	//centre
			c = vector(xx,yy,zz);
			scanf("%f",&r); //radius
			s[k] = sphere(c,r,col,i);
			shapes[i] = &s[k++]; 
		}
	}
	for(k=0;k<numshapes;k++)
		printf("id of shape %d is %d\n",k,shapes[k]->shapeid);
	vector orig = vector(height/2,width/2,25);	//Fixed Origin: Simulating a Pin-hole camera
	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{	vector dir = vector(float(i),float(j),float(0));
			dir = dir - orig; //direction vector
			dir = dir.normalize();
			ray r = ray(orig,dir);
			int col = bgcolor;
			for(k=0;k<numshapes;k++)
				shapes[k]->intersect(r);
			if(r.id != -1)
				col=shapes[r.id]->color;	//should this be '->' or '.' ??
			pixelgrid[i][j] = col;
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
