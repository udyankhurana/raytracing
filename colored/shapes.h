//Include Header Guard 
#ifndef __SHAPES_H_INCLUDED__
#define __SHAPES_H_INCLUDED__

#include<stdio.h>
#include<math.h>
#include<vector>
#include<queue>
#include "ray.h"
#define EPSILON1 1e-7f
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void swap(double &a, double &b)
{	double temp = a;
	a = b;
	b = temp;
}

class shape
{	protected:
	int shapeid;
	int type;	//0=diffuse, 1=specular
	vec color;	//x=red, y=green, z=blue

	public:
	virtual bool intersect(ray &r) = 0;	//virtual function to get intersection
	virtual vec get_vertex(int num) = 0;
	virtual vec get_normal(vec pt) = 0;
	virtual void set_vertex(int num, vec a) = 0;
	shape(vec col,int t,int id): color(col), shapeid(id), type(t) {}
	int get_shapeid()
	{	return shapeid;		}
	vec get_shapecolor()
	{	return color;		}
	int get_shapetype()
	{	return type;		}
	void set_shapeid(int id)
	{	shapeid = id;		}
	void set_shapecolor(vec col)
	{	color = col;		}
	void set_shapetype(int t)
	{	type = t;		}
};

class triangle: public shape
{	protected:
	vec a,b,c;	//for the 3 vertices

	public:
	triangle(vec x,vec y,vec z,vec col,int t, int id): shape(col,t,id), a(x), b(y), c(z) {}
	bool intersect(ray &r)	//Moller-Trumbore algorithm
	{	vec v1,v2;	//calculate vecs for side edges
		vec orig = r.get_origin();
		vec dir = r.get_direction();
		v1 = b - a;
		v2 = c - a;
		vec p = dir.cross(v2);
		double det = p.dot(v1);
		if(det > -EPSILON1 && det < EPSILON1) //means vec is parallel to the plane (int. at inifinity)
			return false;
		double inv_det = 1.0/det;
		vec T = orig - a;
		double U = inv_det * T.dot(p);	//U,V: barycentric coordinates
		if (U <0.0 || U > 1.0) //the intersection is outside the triangle
			return false;
		vec q = T.cross(v1);
		double V = dir.dot(q) * inv_det;
		if (V <0.0 || U+V > 1.0) 
			return false;
		double param = v2.dot(q) * inv_det;
		if(param > r.get_tmin() && param < r.get_tmax())
		{	r.set_tmax(param); //the 't' value assigned if intersection found
			r.set_rayid(shapeid);
			return true;
		}
		return false;
	}
	vec get_vertex(int num)
	{	if(num == 1)
		return a;
		else if(num == 2)
			return b;
		else if(num == 3)
			return c;
	}
	void set_vertex(int num, vec x)
	{	if(num == 1)
		a = x;
		else if(num == 2)
			b = x;
		else if(num == 3)
			c = x;
	}
	vec get_normal(vec pt)
	{	vec x = (b-a).cross(c-a);
		x = x.normalize();
		return x;
	}
};

struct sphere: public shape
{	protected:
	vec centre;
	double radius;

	public:
	sphere(vec c,double r,vec col, int t, int id): shape(col,t,id), centre(c), radius(r) {}
	bool intersect(ray &r)
	{	double a,b,c,ans;
		vec orig = r.get_origin();
		vec dir = r.get_direction();
		vec diff = orig - centre;
		a=dir.dot(dir);
		b=2*(dir.dot(diff));
		c=diff.dot(diff) - radius*radius;
		double disc = b*b - 4*a*c;
		if (disc == 0)
			ans = (-(b/(2*a)));
		else if(disc < 0)
			return false;
		else
		{	double r1,r2;
			r1 = (-b + sqrt(disc))/(2*a);
			r2 = (-b - sqrt(disc))/(2*a);
			ans = MIN(r1,r2);
		}
		if(ans > r.get_tmin() && ans < r.get_tmax())
		{	r.set_tmax(ans);
			r.set_rayid(shapeid);
			return true;
		}
	}
	vec get_vertex(int num)
	{	if(num==0)
			return centre;		
	}
	void set_vertex(int num, vec c)
	{	if(num==0)
			centre = c;		
	}
	vec get_normal(vec pt)
	{	vec x = pt - centre;
		x = x.normalize();
		return x;
	}
	int get_radius()
	{	return radius;		}
	void set_radius(double r)
	{	radius = r;		}
};
#endif
