#include<iostream>
#include<limits>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#define EPSILON 0.000001
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "ray.h"
#include "shapes.h"
using namespace std;

void traverse(aabb *root)
{	if(root==NULL)
		return;
	traverse(root->left);
	root->print();
	traverse(root->right);
}

int main(int argc, char *argv[])
{	
	int width = 200, height = 200;
	int numshapes,i=0,j=0,k=0,color,bgcolor=10;
	unsigned int pixelgrid[805][805]={0};	//output pixel grid: width,height limit = 500
	float xx,yy,zz,min = 10, max = -10;
	char type[25];
	vector < shape* > shapes;
	vector < aabb > a;

	//Read Input
	aabb b1;
	numshapes=4;
	vec x,y,z;
	x = vec(-1,2,0);
	y = vec(-3,2,0);
	z = vec(-3,1,0);
	shapes.push_back(new triangle(x,y,z,col,0));
	b1.insert(x); b1.insert(y); b1.insert(z);
	b1.set_id(0);
	b = b.group(b,b1);
	a.push_back(b1);
	x = vec(1,2,0);
	y = vec(3,2,0);
	z = vec(3,1,0);
	shapes.push_back(new triangle(x,y,z,col,1));
	b2.insert(x); b2.insert(y); b2.insert(z);
	b2.set_id(1);
	a.push_back(b2);
	b = b.group(b,b2);
	x = vec(-1,-2,0);
	y = vec(-3,-2,0);
	z = vec(-3,-1,0);
	shapes.push_back(new triangle(x,y,z,col,2));
	b3.insert(x); b3.insert(y); b3.insert(z);
	b3.set_id(2);
	a.push_back(b3);
	b = b.group(b,b3);
	x = vec(1,-2,0);
	y = vec(3,-2,0);
	z = vec(3,-1,0);
	shapes.push_back(new triangle(x,y,z,col,3));
	b4.insert(x); b4.insert(y); b4.insert(z);
	b4.set_id(3);
	a.push_back(b4);
	b = b.group(b,b4);
/*
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
			aabb b;
			b.insert(x); b.insert(y); b.insert(z);
			b.set_id(i);
			a.push_back(b);
			b1 = b1.group(b1,b);
		}
	}*/
	aabb *root = new aabb(b1.get_min(),b1.get_max());
//	root->print();
	root->split(a);
	vec orig = vec(height/2,width/2,1000);
//	vec orig = vec(0.0f,0.0f,-2.0f);	//Keep at -0.5f for bunny.obj
//	float x_init = -5.0f, y_init = 5.0f;	//PUT as -0.15 and 0.15 for bunny.obj	
	for(i=0;i<height;i++)
	{	for(j=0;j<width;j++)
		{//	float _x = x_init + (10.0f/width) * j;
		//	float _y = y_init - (10.0f/height) * i;
			vec dir = vec(float(i), float(j), 0.0f);
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

	FILE *fo = fopen(argv[1], "w");	//writing final pixel grid to output file
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);
	
	return 0;
}
