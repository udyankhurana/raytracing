//Include Header Guard
#ifndef __RAY_H_INCLUDED__
#define __RAY_H_INCLUDED__

#include<limits>
#include<math.h>
#define EPSILON1 1e-7f

class vec
{	public:
	double x,y,z;	
	vec(): x(0.0),y(0.0),z(0.0) {}
	vec(double a, double  b, double c): x(a),y(b),z(c) {}	//constructor
	vec(const vec& v):x(v.x), y(v.y), z(v.z) {}
	vec operator * (const double &a) const { return vec(x*a,y*a,z*a) ; }	//scalar multiplication
	vec operator * (const vec &a) const { return vec(x*a.x,y*a.y,z*a.z) ; }	//vec multiplication
	vec operator + (const vec &a) const { return vec(x+a.x,y+a.y,z+a.z) ; }	// vec addition
	vec operator - () const { return vec(-x,-y,-z) ; }	//unary subtraction
	vec operator - (const vec &a) const { return vec(x-a.x,y-a.y,z-a.z) ; }	//vec subtraction
	double mag() const	//magnitude of vec
	{	
		return sqrt(x*x + y*y + z*z);	
	}
	double distance(vec v)
	{	
		return sqrt((v.x-x)*(v.x-x) + (v.y-y)*(v.y-y) + (v.z-z)*(v.z-z));	
	}
	vec normalize() const
	{	double m=mag();
		return vec(x/m,y/m,z/m);
	}
	double dot(vec v) const	//dot product with another vec
	{	double d = x*v.x+ y*v.y+ z*v.z;
		return d;
	}
	vec cross(vec v) const	//cross product with another vec
	{	double newx,newy,newz;
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
	double tmin,tmax;	// as ray R = origin + t * direction
	int id;	// to check if intersection happens or not (if yes, with which shape number it happens)

	public:	
	ray(vec x,vec y): rayorig(x), raydir(y), tmin(EPSILON1), tmax(std::numeric_limits<double>::max()), id(-1) {}
	ray(vec x,vec y, double min, double max): rayorig(x), raydir(y), tmin(min), tmax(max), id(-1) {}
	vec get_origin()
	{	return rayorig;		}
	vec get_direction()
	{	return raydir;		}
	double get_tmin()
	{	return tmin;		}
	double get_tmax()
	{	return tmax;		}
	int get_rayid()
	{	return id;		}
	void set_origin(vec o)
	{	rayorig = o;		}
	void set_direction(vec d)
	{	raydir = d;		}
	void set_tmin(double min)
	{	tmin = min;		}
	void set_tmax(double max)
	{	tmax = max;		}
	void set_rayid(int rayid)
	{	id = rayid;		}
};

#endif
