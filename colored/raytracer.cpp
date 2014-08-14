#include<iostream>
#include<limits>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#define EPSILON 0.000001
#define MAX_DEPTH 1

#include "ray.h"
#include "shapes.h"
using namespace std;

int n=5; 
vec orig = vec(0.0f,0.0f,5.0f);
vec bgcolor = vec(0.15,0.15,0.15);
vec lightpos = vec(0.0f,0.9f,0.0f);
vec lightcol = vec(1,1,1);
vec ambientcol = vec(1,1,1);

double clamp(double val, double _min, double _max)
{
	if(val < _min) return _min;
	else if(val > _max) return _max;
	else return val;
}

vec raytrace(ray &r, vector< shape *> &shapes, int depth)
{	
	int k;
	vec col;
	for(k=0;k<shapes.size();k++)
		shapes[k]->intersect(r);

	int rid = r.get_rayid();
	if(rid != -1)	//if there is an intersection
	{	vec color=shapes[rid]->get_shapecolor();
		double tmax = r.get_tmax();
		vec pt = orig + (r.get_direction() * tmax);
		vec normal = shapes[rid]->get_normal(pt);
		vec srdir = lightpos - pt;
		srdir = srdir.normalize();
		ray shadowray = ray(pt,srdir,EPSILON,pt.distance(lightpos));
		int srid;
		for(k=0;k<shapes.size();k++)
		{
			shapes[k]->intersect(shadowray);
			srid = shadowray.get_rayid();
			if(srid != -1)
				break;
		}

		if(shapes[rid]->get_shapetype() == 1)
		{	vec refldir = (normal * 2) - srdir;
			refldir = refldir.normalize();
			ray reflray = ray(pt,refldir);
			if(srid == -1)	//if shadow ray intersects
			{	vec halfvec = srdir - r.get_direction();
				halfvec = halfvec.normalize();
				vec specularcolor = (color * lightcol) * clamp(pow(halfvec.dot(normal),n), 0.0f, 1.0f); 
				col = specularcolor;	
			}
			else
				col = ambientcol * bgcolor;

			if(depth < MAX_DEPTH)
				col = col + raytrace(reflray, shapes, depth+1);
			else
				return col; 
		}
		else
		{	if(srid == -1)	//if shadow ray intersects
			{	vec diffusecolor = (color * lightcol) * clamp(srdir.dot(normal), 0.0f, 1.0f); 
				col = diffusecolor;	
			}
			else
				col = ambientcol * bgcolor;
		}
	}
	return col;
}
int main(int argc, char *argv[])
{	
	int width = atoi(argv[2]), height = atoi(argv[3]);
	int numshapes,i=0,j=0,k=0;
	unsigned int pixelgrid[805][805][3]={0};	//output pixel grid: width,height limit = 500
	vector < shape* > shapes;

	struct timespec t1, t2, t3, t4;
	double time1,time2,time3;
	clock_gettime(CLOCK_MONOTONIC, &t1);	

	//Read Input
	numshapes=12;
	shapes.push_back(new triangle(vec(-1,1,1),vec(-1,1,-1),vec(1,1,-1),vec(1,1,1),0,0));
	shapes.push_back(new triangle(vec(-1,1,1),vec(1,1,-1),vec(1,1,1),vec(1,1,1),0,1));
	shapes.push_back(new triangle(vec(-1,1,1),vec(-1,-1,1),vec(-1,-1,-1),vec(1,0,0),0,2));
	shapes.push_back(new triangle(vec(-1,1,1),vec(-1,-1,-1),vec(-1,1,-1),vec(1,0,0),0,3));
	shapes.push_back(new triangle(vec(-1,1,-1),vec(-1,-1,-1),vec(1,-1,-1),vec(0.75f,0.75f,0.75f),0,4));
	shapes.push_back(new triangle(vec(-1,1,-1),vec(1,-1,-1),vec(1,1,-1),vec(0.75f,0.75f,0.75f),0,5));
	shapes.push_back(new triangle(vec(-1,-1,-1),vec(-1,-1,1),vec(1,-1,1),vec(0,1,0),0,6));
	shapes.push_back(new triangle(vec(-1,-1,-1),vec(1,-1,1),vec(1,-1,-1),vec(0,1,0),0,7));
	shapes.push_back(new triangle(vec(1,1,-1),vec(1,-1,1),vec(1,1,1),vec(0.4f,0.4f,0.4f),0,8));
	shapes.push_back(new triangle(vec(1,1,-1),vec(1,-1,-1),vec(1,-1,1),vec(0.4f,0.4f,0.4f),0,9));
	shapes.push_back(new sphere(vec(-0.5f,-0.8f,0.0f),0.2f,vec(0,0,1),1,10));
	shapes.push_back(new sphere(vec(0.5f,-0.8f,-0.5f),0.2f,vec(0,1,1),1,11));
	
	clock_gettime(CLOCK_MONOTONIC, &t2);	//calculating time for the above process

	//Fixed Origin: Simulating a Pin-hole camera
	double x_init, y_init,unit;
	x_init = -1.5f;
	y_init = 1.5f;
	unit = 3.0f;
	
	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{	double _x = x_init + (unit/width) * j;
			double _y = y_init - (unit/height) * i;
			vec dir = vec(_x, _y, 0.0f);
			dir = dir - orig;	//direction vec
			dir = dir.normalize();
			ray r = ray(orig,dir);
			
			vec col = raytrace(r, shapes, 1);
			//assign color to pixel
			pixelgrid[i][j][0] = int(col.x *255);
			pixelgrid[i][j][1] = int(col.y *255);
			pixelgrid[i][j][2] = int(col.z *255);
		}
	}

	for(i=0;i<numshapes;i++)	
		delete shapes[i];	//Freeing up memory to avoid leakage

	clock_gettime(CLOCK_MONOTONIC, &t3);	//calculating time for the above process
	
	FILE *fo = fopen(argv[1], "w");	//writing final pixel grid to output file
	fprintf(fo, "P3\n%d %d\n255\n",width, height);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u %u %u\n", pixelgrid[i][j][0], pixelgrid[i][j][1], pixelgrid[i][j][2]);

	clock_gettime(CLOCK_MONOTONIC, &t4);	//calculating time for the above process
	time1 = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	time2 = ((t3.tv_sec - t2.tv_sec)*1000) + (((double)(t3.tv_nsec - t2.tv_nsec))/1000000.0);
	time3 = ((t4.tv_sec - t3.tv_sec)*1000) + (((double)(t4.tv_nsec - t3.tv_nsec))/1000000.0);
	printf("No. of Shapes: %d\n", numshapes);
	printf("Time For Reading Input: %lf ms\n", time1);
	printf("Time For Intersection Tests: %lf ms\n", time2);
	printf("Time For Writing To Output: %lf ms\n", time3);
	return 0;
}
