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

	public:
	aabb(): min(vec(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),std::numeric_limits<float>::max())), max(vec(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max())) {}
	aabb(const vec& x,const vec& y): min(x), max(y) {}
	vec get_min() const
	{	return min;		}
	vec get_max() const
	{	return max;		}
	void set_min(const vec& m)
	{	min = m;		}
	void set_max(const vec& m)
	{	max = m;		}
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
	aabb box;	//>=1 nodes per child possible
	std::vector <int> ids;	//empty for internal nodes, filled for leaf nodes
	bvh_node *left, *right;
	bvh_node(aabb b, std::vector <int> id): box(b), ids(id), flag(0), left(NULL), right(NULL) {}
	int get_flag() const
	{	return flag;		}
	void set_flag(const int& m)
	{	flag = m;		}

	void split(aabb &bigbox, std::vector<aabb>& a,std::vector<int>& id)
	{	if(id.size()<=4) //if size<=4 directly make it a leaf node
		{	box=bigbox;
			ids=id;
			flag=1;
			return;
		}

		std::vector <int> idl,idr;
		int i,j,k;
		int no_buckets=5;	//no of split planes = no_buckets-1			
		float cost_rttest=1, cost_trav=0.125;
		float cost_int[3][no_buckets],cost_leaf;
		float split[3][no_buckets];
		vec range,bmax,bmin;
		bmax=box.get_max();
		bmin=box.get_min();
		range=bmax-bmin;
		float sa_parent=2*(range.x*range.y+range.z*range.y+range.x*range.z);
		for(j=0;j<3;j++)
		{	float r,b;
			if(j==0) 
			{	r=range.x;	
				b=bmin.x;
			}
			else if(j==1)
			{	r=range.y;	
				b=bmin.y;
			}
			else	
			{	r=range.z;	
				b=bmin.z;
			}
			for(i=0;i<no_buckets-1;i++)
				split[j][i]=b+float((float(i+1)/no_buckets)*r);
		}
		std::vector <vec> midpts;
		for(i=0;i<id.size();i++)
		{	vec mid = (a[id[i]].get_max() + a[id[i]].get_min())*0.5;
			midpts.push_back(mid);
		}		
		//leaf node cost
		cost_leaf = cost_rttest*id.size();
		//calculating internal node costs
		for(k=0;k<3;k++)
		{	for(i=0;i<no_buckets-1;i++)
			{	int num_tri1=0,num_tri2=0;
				float sa_1=0,sa_2=0,mid_dim=0;
				aabb c1,c2;
				vec range1,range2;
				for(j=0;j<id.size();j++)
				{	if(k==0) mid_dim=midpts[j].x;
					else if(k==1) mid_dim=midpts[j].y;
					else mid_dim=midpts[j].z;
					if(mid_dim<split[k][i])
					{	num_tri1++;
						c1=c1.group(c1,a[id[j]]);
					}
					else
					{	num_tri2++;
						c2=c2.group(c2,a[id[j]]);
					}
				}
				range1=c1.get_max() - c1.get_min();
				sa_1=2*(range1.x*range1.y+range1.z*range1.y+range1.x*range1.z);
				range2=c2.get_max() - c2.get_min();
				sa_2=2*(range2.x*range2.y+range2.z*range2.y+range2.x*range2.z);
				float split_cost;
				if(num_tri1 && num_tri2) split_cost = (sa_1*num_tri1 + sa_2*num_tri2);
				else if(num_tri1) split_cost = sa_1*num_tri1;
				else if(num_tri2) split_cost = sa_2*num_tri2;
				cost_int[k][i] = split_cost/sa_parent + cost_trav; 
			}	
		}	 
		float min_cost=std::numeric_limits<float>::max();
		int dim=0,split_no=0;
		for(k=0;k<3;k++)
		{	for(i=0;i<no_buckets-1;i++)
			{	//printf("cost[%d][%d] = %f\n",k,i,cost_int[k][i]);
				if(min_cost>=cost_int[k][i])
				{	min_cost=cost_int[k][i];
					dim=k;
					split_no=i;
				}
			}	
		}
		//printf("leaf cost = %f\n",cost_leaf);
		if(cost_leaf < min_cost)
		{	box=bigbox;
			ids=id;
			flag=1;
			return;
		}
		else
		{	float mid_dim;
			for(j=0;j<id.size();j++)
			{	if(dim==0) mid_dim=midpts[j].x;
				else if(dim==1) mid_dim=midpts[j].y;
				else mid_dim=midpts[j].z;

				if(mid_dim<split[dim][split_no])
					idl.push_back(id[j]);
				else
					idr.push_back(id[j]);
			}
			//printf("left size = %d right size = %d\n",idl.size(),idr.size());	
			aabb l,r;
			std::vector<int>x;
			if(idl.size())
			{	for(i=0;i<idl.size();i++)
					l = l.group(l,a[idl[i]]);
				left = new bvh_node(l,x);	//left of node grouped
			}
			if(idr.size())
			{	for(i=0;i<idr.size();i++)
					r = r.group(r,a[idr[i]]);
				right = new bvh_node(r,x);	//right of node grouped
			}
			if(left != NULL) 
				left->split(l,a,idl);
			if(right != NULL) 
				right->split(r,a,idr);
		}	
	}
};

union offs 
{	int leaf_off;
	int rchild_off;
	offs() { memset( this, -1, sizeof( offs ) ); }
};

struct linearbvhnode
{	aabb box;
	int size;
	offs offsets;
	linearbvhnode(aabb bbox,int s, offs o): box(bbox), size(s), offsets(o) {}
	linearbvhnode(): box(aabb()), size(-1), offsets(offs()) {}
};

class linearbvh
{
	public:
	std::vector< linearbvhnode > lbvh;
	void flatten(bvh_node *root, std::vector<int> &tri_ids)
	{	if(root==NULL) 
			return;
		int leaf = root->get_flag();
		if(leaf)
		{	offs o;
			o.leaf_off = tri_ids.size();
			linearbvhnode node(root->box, root->ids.size(), o);
			lbvh.push_back(node);
			for(int i=0;i<root->ids.size();i++)
				tri_ids.push_back(root->ids[i]);
			return;
		}
		else
		{	offs o;	
			linearbvhnode node(root->box, 0, o);
			int k=lbvh.size();
			lbvh.push_back(node);
			flatten(root->left,tri_ids);
			lbvh[k].offsets.rchild_off = lbvh.size();	//set roff of parent
			flatten(root->right,tri_ids);
		}
	}

	void traverse(ray &r, std::vector<shape*>& a, std::stack<linearbvhnode> &st, std::stack<int> &st2, std::vector<int> &tri_ids)
	{	linearbvhnode x = st.top();
		float min_t = std::numeric_limits<float>::max();
		if((x.size==0)&&(!(x.box).intersect(r, min_t))) return;
		
		while(!st.empty())
		{	linearbvhnode x = st.top();
			int pos = st2.top();
			int leaf_node = x.size;
			st.pop(); st2.pop();
			// INTERNAL NODE
			if(!leaf_node)
			{	//intersected_nonleaf++;
				float left_min = std::numeric_limits<float>::max();
				float right_min = std::numeric_limits<float>::max();
				bool left_intersect = false, right_intersect = false;
				int left_pos = pos+1, right_pos = x.offsets.rchild_off;
		//		printf("left_pos = %d, right_pos = %d\n",left_pos,right_pos);
				linearbvhnode left,right;
				if(left_pos < lbvh.size()) 
				{	left = lbvh[left_pos];
					left_intersect = (left.box).intersect(r, left_min);
				}
				if(right_pos < lbvh.size()) 
				{	right = lbvh[right_pos];
					right_intersect = (right.box).intersect(r, right_min);
				}

				if(left_intersect && right_intersect) 
	                    	{	if(left_min < right_min) 
					{	st.push(right);
						st2.push(right_pos);
				                st.push(left);
						st2.push(left_pos);
			               	}
		                	else 
					{	st.push(left);
						st2.push(left_pos);
					        st.push(right);
						st2.push(right_pos);
			             	}
		                }
		                else if(left_intersect)
				{	st.push(left);
					st2.push(left_pos);
				}
		                else if(right_intersect) 
				{	st.push(right); 
					st2.push(right_pos);
				}
			}
			// LEAF NODE
			else	//if node is a leaf node
			{	int i,leaf_offset = x.offsets.leaf_off;
				for(i=leaf_offset; i<leaf_offset+x.size; i++)
				{	float hit_t = std::numeric_limits<float>::max();
					int hit_id = -1;
					if(a[tri_ids[i]]->intersect(r, hit_t, hit_id)) //directly doing ray-triangle intersections
					{	if(hit_t < r.get_tmax())			//instead of using ray-box then triangle
						{
							r.set_tmax(hit_t);
							r.set_rayid(hit_id);
						}
					}
				}
			}
		}
	}
};

#endif
