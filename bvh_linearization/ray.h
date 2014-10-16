//Include Header Guard
#ifndef __RAY_H_INCLUDED__
#define __RAY_H_INCLUDED__

#include<limits>
#include<math.h>

class vec
{	public:
	float x,y,z;	
	vec(): x(0.0),y(0.0),z(0.0) {}
	vec(float a,float  b,float c): x(a),y(b),z(c) {}	//constructor
	vec(const vec& v):x(v.x), y(v.y), z(v.z) {}
	vec operator * (const float &a) const { return vec(x*a,y*a,z*a) ; }	//scalar multiplication
	vec operator * (const vec &a) const { return vec(x*a.x,y*a.y,z*a.z) ; }	//vec multiplication
	vec operator + (const vec &a) const { return vec(x+a.x,y+a.y,z+a.z) ; }	// vec addition
	vec operator - (const vec &a) const { return vec(x-a.x,y-a.y,z-a.z) ; }	//vec subtraction
	float mag() const	//magnitude of vec
	{	
		return sqrt(x*x + y*y + z*z);	
	}
	vec normalize() const
	{	float m=mag();
		return vec(x/m,y/m,z/m);
	}
	float dot(vec v) const	//dot product with another vec
	{	float d = x*v.x+ y*v.y+ z*v.z;
		return d;
	}
	vec cross(vec v) const	//cross product with another vec
	{	float newx,newy,newz;
		newx= y*v.z  - z*v.y;
		newy= z*v.x  - x*v.z;
		newz= x*v.y  - y*v.x;
		return vec(newx,newy,newz);	
	}
	friend std::ostream & operator << (std::ostream &os, const vec &v) //print in vec form
	{	os << "[" << v.x << ", " << v.y << ", " << v.z << "]\n";
		return os;
	}
};

class ray
{	protected: 
	vec rayorig,raydir;	//origin and direction of ray
	float tmin,tmax;	// as ray R = origin + t * direction
	int id;	// to check if intersection happens or not (if yes, with which shape number it happens)

	public:	
	ray(vec x,vec y): rayorig(x), raydir(y), tmin(0.0), tmax(std::numeric_limits<float>::max()), id(-1) {}
	vec get_origin()
	{	return rayorig;		}
	vec get_direction()
	{	return raydir;		}
	float get_tmin()
	{	return tmin;		}
	float get_tmax()
	{	return tmax;		}
	int get_rayid()
	{	return id;		}
	void set_origin(vec o)
	{	rayorig = o;		}
	void set_direction(vec d)
	{	raydir = d;		}
	void set_tmin(float min)
	{	tmin = min;		}
	void set_tmax(float max)
	{	tmax = max;		}
	void set_rayid(int rayid)
	{	id = rayid;		}
};

#endif
