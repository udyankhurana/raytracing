#include<stdio.h>
#include<time.h>
#include<math.h>
#define INT_MIN -2147483647

struct point
{	int x,y;
};

struct tri
{	point a,b,c;
	int color;	//value 0-255
	int depth;	//considering 1 tri || to Z plane so 1 depth value and higher value means nearer (0=closest,-ve vals=far away)
};

int min(int x,int y) {	return ((x<y)?x:y);	}
int max(int x,int y) {	return ((x>y)?x:y);	}

/*float area_triangle(point a,point b,point c)
{	float x;
	x=float((a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y))/2);
	x=copysign(x,1.0);
	return x;
}*/

float sign(point p1, point p2, point p3)
{	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);	}

bool pointintriangle(point pt, point v1, point v2, point v3)
{	bool b1, b2, b3;
	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;
	return ((b1 == b2) && (b2 == b3));
}

int main() 
{	
	int colorLevels; 
	int width, height;
	int numtris,i,j,depth,color;
	int bgcolor=10;
	int l,b;
	// Allocate storage
	unsigned int pixelgrid[250][250]={0};	//so width,height limit = 100
	int depthgrid[250][250];
	tri triarray[50];

	struct timespec t1, t2;
	clock_gettime(CLOCK_MONOTONIC, &t1);

	//Read Input
//	printf("Enter width and height: ");
	scanf("%d %d",&width,&height);
//	printf("Enter number of tris: ");
	scanf("%d",&numtris);
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			depthgrid[i][j]=INT_MIN;	//initialise depth grid
	for(i=0;i<numtris;i++)
	{//	printf("tri %d:\n",i+1);
	//	printf("tri Depth: ");
		scanf("%d",&triarray[i].depth);
	//	printf("tri Color: ");
		scanf("%d",&triarray[i].color);
	//	printf("Pt. 1: \n");
		scanf("%d %d",&triarray[i].a.x,&triarray[i].a.y);
	//	printf("Pt. 2: \n");
		scanf("%d %d",&triarray[i].b.x,&triarray[i].b.y);
	//	printf("Pt. 3: \n");
		scanf("%d %d",&triarray[i].c.x,&triarray[i].c.y);
	}

	for(i=0;i<numtris;i++)
	{	int minx,maxx,miny,maxy;
		minx=min(min(triarray[i].a.x,triarray[i].b.x),triarray[i].c.x);
		miny=min(min(triarray[i].a.y,triarray[i].b.y),triarray[i].c.y);
		maxx=max(max(triarray[i].a.x,triarray[i].b.x),triarray[i].c.x);
		maxy=max(max(triarray[i].a.y,triarray[i].b.y),triarray[i].c.y);
		for(l=minx;l<=maxx;l++)
		{	for(b=miny;b<=maxy;b++)
			{	point pt;
				pt.x=l;pt.y=b;
				if(pointintriangle(pt,triarray[i].a,triarray[i].b,triarray[i].c))
				{	if(triarray[i].depth > depthgrid[l][b])
					{	depthgrid[l][b] = triarray[i].depth;
						pixelgrid[l][b] = triarray[i].color;
					}
				}
			}
		}
	}

	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			if(pixelgrid[i][j]==0)
				pixelgrid[i][j]=bgcolor;
		
	clock_gettime(CLOCK_MONOTONIC, &t2);
	double time = ((t2.tv_sec - t1.tv_sec)*1000) + (((double)(t2.tv_nsec - t1.tv_nsec))/1000000.0);
	
	printf("Time (in milliseconds): %lf\n", time);

	FILE *fo = fopen("output.pgm", "w");
	fprintf(fo, "P2\n%d %d\n255\n",height, width);
	for(i = 0; i < height; i++)  
		for(j = 0; j < width; j++) 
			fprintf(fo, "%u\n", pixelgrid[i][j]);
}
