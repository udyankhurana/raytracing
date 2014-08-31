//Include Header Guard 
#ifndef __SHAPES_H_INCLUDED__
#define __SHAPES_H_INCLUDED__

#include<stdio.h>
#include<math.h>
#include<vector>
#include<queue>
#include<stack>
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
	virtual bool intersect(ray &r, float& hit_t, int& hit_id) = 0;	//virtual function to get intersection
	virtual vec get_vertex(int num) = 0;
	virtual void set_vertex(int num, vec x) = 0;
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
	bool intersect(ray &r, float& hit_t, int& hit_id)	//Moller-Trumbore algorithm
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
		if(param > r.get_tmin() && param < r.get_tmax())
		{	//r.set_tmax(param); //the 't' value assigned if intersection found
			//r.set_rayid(shapeid);
			hit_t = param; 
			hit_id = shapeid;
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
	bool intersect(ray &r, float& hit_t, int& hit_id)
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
		{	//r.set_tmax(ans);
			//r.set_rayid(shapeid);
			hit_t = ans;
			hit_id = shapeid;
			return true;
		}
	}
	vec get_vertex(int num)
	{	if(num == 0) return centre;	}
	void set_vertex(int num, vec x)
	{	if(num == 0) centre=x;	}
	int get_radius()
	{	return radius;		}
	void set_radius(float r)
	{	radius = r;		}
};

class aabb
{	protected:
	vec min,max;
	int id;

	public:
	//aabb *left, *right;
	aabb(): min(vec(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),std::numeric_limits<float>::max())), max(vec(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max())),id(-1) {}
	aabb(const vec& x,const vec& y): min(x), max(y), id(-1) {}
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
	aabb group(aabb a, aabb b) 
	{	aabb x = aabb();
		vec a1,a2,b1,b2;
		a1 = a.get_min(); a2 = a.get_max();
		b1 = b.get_min(); b2 = b.get_max();
		return aabb(vec(MIN(a1.x,b1.x),MIN(a1.y,b1.y),MIN(a1.z,b1.z)),vec(MAX(a2.x,b2.x),MAX(a2.y,b2.y),MAX(a2.z,b2.z)));
	}
	bool intersect(ray &r, float hit_t)
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
		hit_t = tmin;
		return true;
	}
};

class bvh_node
{	protected:
	int flag; 	//flag=1(leaf node),0(internal node)
	
	public:
/**/	std::vector <aabb> boxes;	//>=1 nodes per child possible
	bvh_node *left, *right;
	bvh_node(std::vector <aabb> b): box(b), flag(0), left(NULL), right(NULL) {}
	int get_flag() const
	{	return flag;		}
	int get_id() const 
	{	return id;		}
	void set_flag(const int& m)
	{	flag = m;		}
	void set_id(int m)
	{	id = m;		} 

	void split(std::vector<aabb>& a, std::vector<shape*> s)
	{	if(a.size()==1) 
		return;

		std::vector <aabb> l,r;
		std::vector <shape*> sl,sr;
		int i,j,k;
/**/		int no_buckets=5;	//no_buckets-1 split planes (=4)			
/**/		float cost_rbtest, cost_rttest, cost_trav;	//TO SET
		float sa_parent;	
		float cost_int[3][no_buckets],cost_leaf;
		int split[3][no_buckets];
		vec range,bmax,bmin;
		bmax=boxes[0].get_max();
		bmin=boxes[0].get_min();
		range=bmax-bmin;
		sa_parent=2*(range.x*range.y+range.z*range.y+range.x*range.z);
		for(j=0;j<3;j++)
		{	float r;
			if(j==0) r=range.x;	
			else if(j==1) r=range.y;	
			else r=range.z;	
			for(i=0;i<no_buckets-1;i++)
				split[j][i]=bmin.x+((i+1)/no_buckets)*r;
		}
		std::vector <vec> midpts;
		aabb cent_bbox;
		for(i=0;i<a.size();i++)
		{	vec mid = (a[i]->get_max() + a[i]->get_min())*0.5;
			midpts.push_back(mid);
		}		
		//calculating internal node costs
		for(k=0;k<3;k++)
		{	for(i=0;i<no_buckets-1;i++)
			{	int num_tri1=0,num_tri2=0;
				float sa_1,sa_2;
				aabb c1,c2;
				vec range1,range2;
				for(j=0;j<midpts.size();j++)
				{	if(midpts[j].x<split[k][i])
					{	num_tri1++;
						c1=c1.group(c1,a[j]);
					}
					else
					{	num_tri2++;
						c2=c2.group(c2,a[j]);
					}
				}
				range1=c1.get_max() - c1.get_min();
				sa_1=2*(range1.x*range1.y+range1.z*range1.y+range1.x*range1.z);
				range2=c2.get_max() - c2.get_min();
				sa_2=2*(range2.x*range2.y+range2.z*range2.y+range2.x*range2.z);
				cost_int[k][i] = (sa_1*num_tri1 + sa_2*num_tri2)/sa_parent + cost_trav; 
			}	
		}
		//leaf node cost
		cost_leaf = cost_rbtest*(a.size()+1) + cost_rttest*a.size();
		float min_cost=std::numeric_limits<float>::max();
		int dim=0,split_no=0;
		for(k=0;k<3;k++)
		{	for(i=0;i<no_buckets-1;i++)
			{	if(min_cost>=cost_int[k][i])
				{	min_cost=cost_int[k][i];
					dim=k;
					split_no=i;
				}
			}	
		}
		if(cost_leaf < min_cost)
		{	boxes=a;
			flag=1;
			return;
		}
		else
		{	for(j=0;j<a.size();j++)
			{	if(midpts[j].x<split[dim][split_no])
				{	l.push_back(a[j]);
					sl.push_back(s[j]);
				}
				else
				{	r.push_back(a[j]);
					sr.push_back(s[j]);
				}
			}	
		}			
		aabb b,n;
		std::vector<aabb>b1,n1;
		if(l.size())
		{	for(i=0;i<l.size();i++)
				b = b.group(b,l[i]);
			b1.push_back(b);
			left = new bvh_node(b1);	//left of node grouped
		}
		if(r.size())
		{	for(i=0;i<r.size();i++)
				n = n.group(n,r[i]);
			n1.push_back(n);
			right = new bvh_node(n1);
		}

		if(left != NULL) 
			left->split(l,sl);
		if(right != NULL) 
			right->split(r,sr);
	}
};

class bvh
{	public:
	bvh_node *root;
	bvh(bvh_node *a): root(a) {}

	void traverse(ray &r, std::vector<shape*>& a, std::stack<bvh_node*> &st)
	{	bvh_node *x = st.top();
		float min_t = std::numeric_limits<float>::max();
		if(!(x->box).intersect(r, min_t)) return;
		
		while(!st.empty())
		{	bvh_node *x = st.top();
			st.pop();
			// INTERNAL NODE
			if(x->get_flag() == 0)
			{	//intersected_nonleaf++;
				float left_min = std::numeric_limits<float>::max();
				float right_min = std::numeric_limits<float>::max();
				bool left_intersect = false, right_intersect = false;
				if(x->left != NULL)  left_intersect = (x->left->box).intersect(r, left_min);
				if(x->right != NULL) right_intersect = (x->right->box).intersect(r, right_min);
				if(left_intersect && right_intersect) 
	                    	{	if(left_min < right_min) 
					{	st.push(x->right);
				                st.push(x->left);
			               	}
		                	else 
					{	st.push(x->left);
					        st.push(x->right);
			             	}
		                }
		                else if(left_intersect)  st.push(x->left);	
		                else if(right_intersect) st.push(x->right); 
			}
			// LEAF NODE
		        else 
			{	//intersected_leaf++;
				float hit_t = std::numeric_limits<float>::max();
				int hit_id = -1;
				if(a[x->get_id()]->intersect(r, hit_t, hit_id))
				{	//intersected_prims++;
					if(hit_t < r.get_tmax())
					{
						r.set_tmax(hit_t);
						r.set_rayid(hit_id);
					}
				}
			}
		}
		
	}
};

#endif
