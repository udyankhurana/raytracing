#include<iostream>
#include<limits>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<vector>
#define EPSILON 0.000001
#define MIN(a,b) (((a)<(b))?(a):(b))

#include "ray.h"
#include "shapes.h"
using namespace std;

int main(int argc, char *argv[])
{	
	int width, height;
	int numshapes,i=0,j=0,k=0,color,bgcolor=10;
	unsigned int pixelgrid[805][805]={0};	//output pixel grid: width,height limit = 500
	float xx,yy,zz;
	char type[25];
	vector < shape* > shapes;

	struct timespec t1, t2, t3, t4;
	double time;
	clock_gettime(CLOCK_MONOTONIC, &t1);	

	//Read Input
	scanf("%d %d",&height,&width);	//height=rows, width=cols
	scanf("%d",&numshapes);
	for(i=0;i<numshapes;i++)
	{	scanf("%s",type);
		if(!strcmp(type,"Triangle"))
		{	int col;
			vec x,y,z;
			scanf("%d",&col);
			scanf("%f %f %f",&xx,&yy,&zz);	//pt 1
			x = vec(xx,yy,zz);
			scanf("%f %f %f",&xx,&yy,&zz); //pt 2
			y = vec(xx,yy,zz);
			scanf("%f %f %f",&xx,&yy,&zz); //pt 3
			z = vec(xx,yy,zz);
			shapes.push_back(new triangle(x,y,z,col,i));
		}
		else if(!strcmp(type,"Sphere"))
		{	int col; vec c; float r;
			scanf("%d",&col);
			scanf("%f %f %f",&xx,&yy,&zz);	//centre
			c = vec(xx,yy,zz);
			scanf("%f",&r); //radius
			shapes.push_back(new sphere(c,r,col,i));
		}
	}
	printf("No. of Shapes: %d\n", numshapes);
	clock_gettime(CLOCK_MONOTONIC, &t2);	//calculating time for the above process
	time = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	printf("Time For Reading Input: %lf ms\n", time);

	vec orig = vec(height/2,width/2,5000);	//Fixed Origin: Simulating a Pin-hole camera
	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{	vec dir = vec(float(i),float(j),float(0));
			dir = dir - orig; //direction vec
			dir = dir.normalize();
			ray r = ray(orig,dir);
			int col = bgcolor;
			for(k=0;k<numshapes;k++)
				shapes[k]->intersect(r);
			int rid = r.get_rayid();
			if(rid != -1)
				col=shapes[rid]->get_shapecolor();
			pixelgrid[i][j] = col;
		}
	}

	for(i=0;i<numshapes;i++)	
		delete shapes[i];	//Freeing up memory to avoid leakage

	clock_gettime(CLOCK_MONOTONIC, &t3);	//calculating time for the above process
	time = ((t3.tv_sec - t2.tv_sec)*1000) + (((double)(t3.tv_nsec - t2.tv_nsec))/1000000.0);
	printf("Time For Intersection Tests: %lf ms\n", time);

	FILE *fo = fopen(argv[1], "w");	//writing final pixel grid to output file
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);

	clock_gettime(CLOCK_MONOTONIC, &t4);	//calculating time for the above process
	time = ((t4.tv_sec - t3.tv_sec)*1000) + (((double)(t4.tv_nsec - t3.tv_nsec))/1000000.0);
	printf("Time For Writing To Output: %lf ms\n", time);
	return 0;
}
