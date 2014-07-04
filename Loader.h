#ifndef __HEADER_H__
#define __HEADER_H__

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <cassert>
#include <cstring>

struct float3
{
    float x, y, z;
    float3() {}
    float3(float a, float b, float c):x(a), y(b), z(c) {}
};

struct float2
{
    float x, y;
    float2() {}
    float2(float a, float b):x(a), y(b) {}
};

struct uint2
{
    unsigned int x, y;
    uint2() {}
    uint2(unsigned int a, unsigned int b):x(a), y(b) {}
};

struct uint3
{
    unsigned int x, y, z;
    uint3() {}
    uint3(unsigned int a, unsigned int b, unsigned int c):x(a), y(b), z(c) {}
};

struct int3
{
    int x, y, z;
    int3() {}
    int3(int a, int b, int c):x(a), y(b), z(c) {}
};


struct SubMesh
{
	std::string         m_name;
	std::vector<float3> m_vertices;
	std::vector<float3> m_normals;
	std::vector<float2> m_texcoords;
	std::vector<uint3>  m_vtxIndices;
	std::vector<uint3>  m_norIndices;
	std::vector<uint3>  m_texIndices;
};

struct Tokens
{
	std::vector<std::string> m_tokens;
};

class CustomFileFormat
{
public:
    
    CustomFileFormat(char * meshfile):m_meshfile_path(meshfile) {
//		std::cout<<"Loading Custom File Format From Mesh File : "<<m_meshfile_path<<"\n";
	}

	CustomFileFormat(char* meshfile,char* materialfile):m_meshfile_path(meshfile), m_materialfile_path(materialfile) {
		std::cout<<"Loading Custom File Format From Mesh File : "<<m_meshfile_path<<" Using Material File : "<<m_materialfile_path<<"\n";
	}
	void process();
private:

	bool   isComment(const std::string& line);
	bool   isFace(const Tokens& tokens)   { if(!strcmp(tokens.m_tokens[0].c_str(), "f"))  return true; else return false; }
	bool   isVertex(const Tokens& tokens) { if(!strcmp(tokens.m_tokens[0].c_str(), "v"))  return true; else return false; }
	bool   isNormal(const Tokens& tokens) { if(!strcmp(tokens.m_tokens[0].c_str(), "vn")) return true; else return false; }
	bool   isTex(const Tokens& tokens)    { if(!strcmp(tokens.m_tokens[0].c_str(), "vt")) return true; else return false; }
	bool   isObject(const Tokens&tokens)  { if(!strcmp(tokens.m_tokens[0].c_str(), "o"))  return true; else return false; }
	int3   parseFace(const std::string& line, bool& hasVtx, bool& hasNormal, bool& hasTex);

	Tokens tokenize(const std::string& line);
	

	char*          m_meshfile_path;
	char*          m_materialfile_path;
public:
	std::vector<SubMesh> m_meshes;
};

#endif
