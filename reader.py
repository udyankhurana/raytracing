import math
f= open('bunny.ply','r')
content = f.readlines()
f.close()

numverts = int(content[4].split(' ')[2])
numfaces = int(content[10].split(' ')[2])
verts = []
faces = []
ver = []
xmax = -2
ymax = -2
zmax = -2
xmin = 2
ymin = 2
zmin = 2

for i in range(13,13+numverts):
	a = float(content[i].split(' ')[0])
	b = float(content[i].split(' ')[1])
	c = float(content[i].split(' ')[2])
	verts.append([a,b,c])
	xmax=max(xmax,a)
	ymax=max(ymax,b)
	zmax=max(zmax,c)
	xmin=min(xmin,a)
	ymin=min(ymin,b)
	zmin=min(zmin,c)

for i in range(13+numverts,13+numverts+numfaces):
	a = int(content[i].split(' ')[1])
	b = int(content[i].split(' ')[2])
	c = int(content[i].split(' ')[3])
	faces.append([a,b,c])

gmax = max(xmax,ymax,zmax)
gmin = min(xmin,ymin,zmin)
ratio = 500/(gmax-gmin)
print (xmax-gmin)*ratio, (xmin-gmin)*ratio
print (ymax-gmin)*ratio, (ymin-gmin)*ratio
print (zmax-gmin)*ratio, (zmin-gmin)*ratio

for i in verts:
	i[0] = (i[0]-gmin)*ratio
	i[0] = math.ceil(i[0]*1000)/1000
	i[0] = i[0] + 200
	i[1] = (i[1]-gmin)*ratio
	i[1] = math.ceil(i[1]*1000)/1000
	i[2] = (i[2]-gmin)*ratio
	i[2] = math.ceil(i[2]*1000)/1000


for i in faces:
	x = verts[i[0]-1]
	y = verts[i[1]-1]
	z = verts[i[2]-1]
	ver.append([x,y,z])

f1 = open('bunny.txt','w')
f1.write('800 800\n')
f1.write('%d\n' %(numfaces))
for i in ver:
	f1.write('Triangle\n')
	f1.write('254\n')
	f1.write('%.3f %.3f %.3f\n' %(i[0][0],i[0][1],i[0][2]))
	f1.write('%.3f %.3f %.3f\n' %(i[1][0],i[1][1],i[1][2]))
	f1.write('%.3f %.3f %.3f\n' %(i[2][0],i[2][1],i[2][2]))

f1.close()
