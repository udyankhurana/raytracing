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
#include "ModelLoader.h"
using namespace std;

bvh_node *root = NULL;
int main(int argc, char *argv[])
{	
	int width = atoi(argv[3]), height = atoi(argv[4]);
	int numshapes,i=0,j=0,k=0,color,bgcolor=10;
	unsigned int pixelgrid[1024][1024]={0};	//output pixel grid: width,height limit = 500
	float xx,yy,zz,min = 10, max = -10;
	vector < shape* > shapes;
	vector < aabb > box_stack;

	struct timespec t1, t2, t3, t4;
	double read_time,test_time,write_time;
	clock_gettime(CLOCK_MONOTONIC, &t1);	

	//Read Input
	aabb world_boundingbox;
	Loader loader(argv[1]);
	loader.Load_Model(shapes, world_boundingbox, box_stack);
/**/	vector<aabb> wbb;
/**/	wbb.push_back(world_boundingbox);
	root= new bvh_node(wbb);
	bvh master = bvh(root);	
	master.root->split(box_stack, shapes);	//Building the aabb
	clock_gettime(CLOCK_MONOTONIC, &t2);	//calculating time for the above process

	//Fixed Origin: Simulating a Pin-hole camera
	float x_init, y_init,unit;
	vec orig;
	if(!strcmp("bunny.obj",argv[1]))
	{	orig = vec(0.0f,0.0f,-0.5f);	//For bunny.obj
		x_init = -0.15f;
		y_init = 0.15f;
		unit = 0.3f;
	}
	else if(!strcmp("cow.obj",argv[1]))
	{	orig = vec(0.0f,0.0f,-2.0f);	//For cow.obj
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
			std::stack<bvh_node*> st;
			st.push(root);
			master.traverse(r, shapes, st);	//BVH traversal to find object intersecting with ray
			int rid = r.get_rayid();
			if(rid != -1)
				col=shapes[rid]->get_shapecolor();
			pixelgrid[i][j] = col;
		}
	}
	//root->print_stats();

	for(i=0;i<numshapes;i++)	
		delete shapes[i];	//Freeing up memory to avoid leakage

	clock_gettime(CLOCK_MONOTONIC, &t3);	//calculating time for the above process
	
	FILE *fo = fopen(argv[2], "w");	//writing final pixel grid to output file
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);

	clock_gettime(CLOCK_MONOTONIC, &t4);	//calculating time for the above process
	read_time = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	test_time = ((t3.tv_sec - t2.tv_sec)*1000) + (((double)(t3.tv_nsec - t2.tv_nsec))/1000000.0);
	write_time = ((t4.tv_sec - t3.tv_sec)*1000) + (((double)(t4.tv_nsec - t3.tv_nsec))/1000000.0);
	printf("Time For Reading Input + Building BVH: %lf ms\n", read_time);
	printf("Time For Intersection Tests: %lf ms\n", test_time);
	printf("Time For Writing To Output: %lf ms\n", write_time);
	printf("TOTAL TIME: %lf ms\n", read_time+test_time+write_time);
	return 0;
}
