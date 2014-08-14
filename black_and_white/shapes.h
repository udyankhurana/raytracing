//Include Header Guard 
#ifndef __SHAPES_H_INCLUDED__
#define __SHAPES_H_INCLUDED__

#include<stdio.h>
#include<math.h>
#include<vector>
#include<queue>
#include "ray.h"
#define EPSILON 0.000001
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void swap(float &a, float &b)
{	float temp = a;
	a = b;
	b = temp;
}

class shape
{	protected:
	int shapeid,color;

	public:
	virtual bool intersect(ray &r) = 0;	//virtual function to get intersection
	shape(int col,int id): color(col), shapeid(id) {}
	int get_shapeid()
	{	return shapeid;		}
	int get_shapecolor()
	{	return color;		}
	void set_shapeid(int id)
	{	shapeid = id;		}
	void set_shapecolor(int col)
	{	color = col;		}
};

class triangle: public shape
{	protected:
	vec a,b,c;	//for the 3 vertices

	public:
	triangle(vec x,vec y,vec z,int col,int id): shape(col,id), a(x), b(y), c(z) {}
	bool intersect(ray &r)	//Moller-Trumbore algorithm
	{	vec v1,v2;	//calculate vecs for side edges
		vec orig = r.get_origin();
		vec dir = r.get_direction();
		v1 = b - a;
		v2 = c - a;
		vec p = dir.cross(v2);
		float det = p.dot(v1);
		if(det > -EPSILON && det < EPSILON) //means vec is parallel to the plane (int. at inifinity)
			return false;
		float inv_det = 1.0/det;
		vec T = orig - a;
		float U = inv_det * T.dot(p);	//U,V: barycentric coordinates
		if (U <0.0 || U > 1.0) //the intersection is outside the triangle
			return false;
		vec q = T.cross(v1);
		float V = dir.dot(q) * inv_det;
		if (V <0.0 || U+V > 1.0) 
			return false;
		float param = v2.dot(q) * inv_det;
		if(param > EPSILON && param < r.get_tmax())
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
};

struct sphere: public shape
{	protected:
	vec centre;
	float radius;

	public:
	sphere(vec c,float r,int col,int id): shape(col,id), centre(c), radius(r) {}
	bool intersect(ray &r)
	{	float a,b,c,ans;
		vec orig = r.get_origin();
		vec dir = r.get_direction();
		vec diff = orig - centre;
		a=dir.dot(dir);
		b=2*(dir.dot(diff));
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
		if(ans < r.get_tmax())
		{	r.set_tmax(ans);
			r.set_rayid(shapeid);
			return true;
		}
	}
	vec get_centre()
	{	return centre;		}
	void set_centre(vec c)
	{	centre = c;		}
	int get_radius()
	{	return radius;		}
	void set_radius(float r)
	{	radius = r;		}
};

class bvh
{	protected:
	vec min,max;
	int id;

	public:
	bvh *left, *right;
	bvh(): min(vec(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),std::numeric_limits<float>::max())), max(vec(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max())),left(NULL),right(NULL),id(-1) {}
	bvh(const vec& x,const vec& y): min(x), max(y), left(NULL), right(NULL), id(-1) {}
	vec get_min() const
	{	return min;		}
	vec get_max() const
	{	return max;		}
	int get_id() const 
	{	return id;		}
	void set_min(const vec& m)
	{	min = m;		}
	void set_max(const vec& m)
	{	max = m;		}
	void set_id(int m)
	{	id = m;		}
	void insert(vec a)
	{	min.x = MIN(min.x,a.x); min.y = MIN(min.y,a.y); min.z = MIN(min.z,a.z);
		max.x = MAX(max.x,a.x); max.y = MAX(max.y,a.y); max.z = MAX(max.z,a.z);
	}
	bvh group(bvh a, bvh b) 
	{	bvh x = bvh();
		vec a1,a2,b1,b2;
		a1 = a.get_min(); a2 = a.get_max();
		b1 = b.get_min(); b2 = b.get_max();
		return bvh(vec(MIN(a1.x,b1.x),MIN(a1.y,b1.y),MIN(a1.z,b1.z)),vec(MAX(a2.x,b2.x),MAX(a2.y,b2.y),MAX(a2.z,b2.z)));
	}
	void split(std::vector<bvh>& a)
	{	if(a.size()==1) 
		return;

		int i;
		std::vector <bvh> l,r;
		//Mid-Point Splitting Algorithm (Not Working)
		/*	vec xx = max - min;
			float half, c, m = MAX(MAX(xx.x, xx.y), xx.z);
			if(m == xx.x)
			{	half = (max.x + min.x)/2;
			std::cout<<"half1\n";
			}
			else if(m == xx.y)
			{	half = (max.y + min.y)/2;
			std::cout<<"half2\n";
			}
			else
			{	half = (max.z + min.z)/2;
			std::cout<<"half3\n";
			}

			for(i=0;i<a.size();i++)
			{	if(m == xx.x)
			c = (a[i].get_max().x + a[i].get_min().x)/2;
			else if(m == xx.y)
			c = (a[i].get_max().y + a[i].get_min().y)/2;
			else
			c = (a[i].get_max().z + a[i].get_min().z)/2;
			if(c<half)
			l.push_back(a[i]);
			else
			r.push_back(a[i]);
			}
			std::cout<<"lsize= "<<l.size() << "\trsize= "<<r.size()<<"\n";
		 */
		//Naive Splitting Algorithm
		for(i=0;i<a.size()/2;i++)
			l.push_back(a[i]);

		for(;i<a.size();i++)
			r.push_back(a[i]);

		bvh b,n;
		if(l.size())
		{	for(i=0;i<l.size();i++)
			b = b.group(b,l[i]);
			left = new bvh(b.get_min(),b.get_max());
		}

		if(r.size())
		{	for(i=0;i<r.size();i++)
			n = n.group(n,r[i]);
			right = new bvh(n.get_min(),n.get_max());
		}

		if(l.size()==1)
			left->id=l[0].get_id();
		if(r.size()==1)
			right->id=r[0].get_id();

		if(left != NULL) 
			left->split(l);
		if(right != NULL) 
			right->split(r);
	}
	void print()
	{	std::cout<<"Min= "<<min<<"\n";
		std::cout<<"Max= "<<max<<"\n";
		std::cout<<"Id= "<<id<<"\n_________________________________\n";
	}
	bool intersect(ray &r)
	{	vec orig = r.get_origin();
		vec dir = r.get_direction();	
		float tmin = (min.x - orig.x) / dir.x;
		float tmax = (max.x - orig.x) / dir.x;
		if (tmin > tmax) 
			swap(tmin, tmax);
		float tymin = (min.y - orig.y) / dir.y;
		float tymax = (max.y - orig.y) / dir.y;
		if (tymin > tymax) 
			swap(tymin, tymax);
		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		float tzmin = (min.z - orig.z) / dir.z;
		float tzmax = (max.z - orig.z) / dir.z;
		if (tzmin > tzmax) 
			swap(tzmin, tzmax);
		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;
		if ((tmin > r.get_tmax()) || (tmax < r.get_tmin())) 
			return false;
		return true;
	}
	void traverse(ray &r, std::vector<shape*>& a, std::queue<bvh*>& q)
	{	while(!q.empty())
		{	bvh *x = q.front();
			q.pop();
			if(x->intersect(r))
			{	if(x->id == -1)
				{
					if(x->left != NULL) q.push(x->left);
					if(x->right != NULL) q.push(x->right);
				}
				else
					a[x->id]->intersect(r);
			}
		}
	}
};

#endif