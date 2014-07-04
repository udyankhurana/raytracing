#include<iostream>
#include<limits>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#define EPSILON 0.000001

#include "ray.h"
#include "shapes.h"
#include "Loader.h"
using namespace std;

aabb *root = NULL;
int main(int argc, char *argv[])
{	
	int width = atoi(argv[3]), height = atoi(argv[4]);
	if(argc < 5)
	{	printf("Insufficient Arguments ->\nUsage: ./a.out <inputfilename> <outputfilename> <width> <height>\n");
		return 0;
	}
	if(width > 800 || height > 800)	
	{	printf("Enter Value of Height and Width <=800 pixels\n");
		return 0;
	}
	int numshapes,i=0,j=0,k=0,color,bgcolor=10;
	unsigned int pixelgrid[805][805]={0};	//output pixel grid: width,height limit = 800
	float xx,yy,zz,min = 10, max = -10;
	vector < shape* > shapes;
	vector < aabb > a;

	struct timespec t1, t2, t3, t4;
	double time1,time2,time3;
	clock_gettime(CLOCK_MONOTONIC, &t1);	

	//Read Input
	aabb x1;
	CustomFileFormat fmt(argv[1]);
	fmt.process();
	numshapes=fmt.m_meshes[0].m_vtxIndices.size();
	for(i=0;i<numshapes;i++)
	{	vec x,y,z;
		int col = 254;	
		uint3 index = fmt.m_meshes[0].m_vtxIndices[i];
		x = vec(fmt.m_meshes[0].m_vertices[index.x].x,fmt.m_meshes[0].m_vertices[index.x].y,fmt.m_meshes[0].m_vertices[index.x].z);
		y = vec(fmt.m_meshes[0].m_vertices[index.y].x,fmt.m_meshes[0].m_vertices[index.y].y,fmt.m_meshes[0].m_vertices[index.y].z);
		z = vec(fmt.m_meshes[0].m_vertices[index.z].x,fmt.m_meshes[0].m_vertices[index.z].y,fmt.m_meshes[0].m_vertices[index.z].z);
		shapes.push_back(new triangle(x,y,z,col,i));
		aabb b;
		b.insert(x); b.insert(y); b.insert(z);
		b.set_id(i); 
		a.push_back(b);
		x1 = x1.group(x1,b);
	}
	root = new aabb(x1.get_min(),x1.get_max());
	root->split(a);	//Building the BVH
	clock_gettime(CLOCK_MONOTONIC, &t2);	//calculating time for the above process

	//Fixed Origin: Simulating a Pin-hole camera
	float x_init, y_init,unit;	//For bunny.obj	
	vec orig;
	if(!strcmp("bunny.obj",argv[1]))
	{	orig = vec(0.0f,0.0f,-0.5f);	//For bunny.obj
		x_init = -0.15f;
		y_init = 0.15f;
		unit = 0.3f;
	}
	else if(!strcmp("cow.obj",argv[1]))
	{	orig = vec(0.0f,0.0f,-2.0f);	//For bunny.obj
		x_init = -1.2f;
		y_init = 1.8f;
		unit = 2.4f;
	}

	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{	float _x = x_init + (unit/width) * j;
			float _y = y_init - (unit/height) * i;
			vec dir = vec(_x, _y, 0.0f);
			dir = dir - orig;	//direction vec
			dir = dir.normalize();
			ray r = ray(orig,dir);
			int col = bgcolor;
			std::queue<aabb*> q;
			q.push(root);
			root->traverse(r,shapes,q);
			int rid = r.get_rayid();
			if(rid != -1)
				col=shapes[rid]->get_shapecolor();
			pixelgrid[i][j] = col;
		}
	}

	for(i=0;i<numshapes;i++)	
		delete shapes[i];	//Freeing up memory to avoid leakage

	clock_gettime(CLOCK_MONOTONIC, &t3);	//calculating time for the above process
	
	FILE *fo = fopen(argv[2], "w");	//writing final pixel grid to output file
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);

	clock_gettime(CLOCK_MONOTONIC, &t4);	//calculating time for the above process
	time1 = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	time2 = ((t3.tv_sec - t2.tv_sec)*1000) + (((double)(t3.tv_nsec - t2.tv_nsec))/1000000.0);
	time3 = ((t4.tv_sec - t3.tv_sec)*1000) + (((double)(t4.tv_nsec - t3.tv_nsec))/1000000.0);
	printf("No. of Shapes: %d\n", numshapes);
	printf("Time For Reading Input + Building BVH: %lf ms\n", time1);
	printf("Time For Intersection Tests: %lf ms\n", time2);
	printf("Time For Writing To Output: %lf ms\n", time3);
	return 0;
}
