#include "Loader.h"

#define MAX_LINE_LENGTH 1024

/* Helper functions to create structures */
inline float3 make_float3(float x, float y, float z) { return float3(x, y, z); }
inline float2 make_float2(float x, float y) { return float2(x, y); }
inline uint3 make_uint3(unsigned int x, unsigned int y, unsigned int z) { return uint3(x, y, z); }
inline int3 make_int3(int x, int y, int z) { return int3(x, y, z); }

bool CustomFileFormat::isComment(const std::string& line) {
	size_t position = line.find("#");
	if(position == std::string::npos) return false;
	else return true;
}

Tokens CustomFileFormat::tokenize(const std::string& line) {
	Tokens ret;
	char delimiters[4] = " ";
	char* str = const_cast<char*>(line.c_str());	
	char * pch;
	pch = strtok (str, delimiters);
	while (pch != NULL) {
		ret.m_tokens.push_back(std::string(pch));
		pch = strtok (NULL, delimiters);
	}	
	return ret;
}

int3 CustomFileFormat::parseFace(const std::string& line, bool& hasVtx, bool& hasNormal, bool& hasTex) {
	int ret[3] = {-1, -1, -1};		// default indices
	char delimiters[4] = "/";
	char* pch;
	char* str = const_cast<char*>(line.c_str());
	pch = strtok(str, delimiters);
	int times = 0;
	while(pch != NULL) {
		ret[times] = atoi(pch);
		pch = strtok(NULL, delimiters);
		times++;
	}
	// check for the existence of a // string in the line
	// if so, its a v//vn format. 
	if(times == 1) { hasVtx = true; hasNormal = false; hasTex = false; }
	else if(times == 2) {
		size_t position = line.find("//");
		if(position == std::string::npos) {
			// we have v//vn format
			hasVtx = true;
			hasNormal = true;
			hasTex = false;
			std::swap(ret[2], ret[1]);		// swap the results

		} else {
			// else we have v/vt format
			hasVtx = true;
			hasNormal = false;
			hasTex = true;
		}
	}
	else if(times == 3) { hasVtx = true; hasNormal = true; hasTex = true; }
	return make_int3(ret[0], ret[1], ret[2]);
}

void CustomFileFormat::process() {
	double start = 0.0, end = 0.0;
	std::ifstream ifs;
	try {
		ifs.open(m_meshfile_path);
		//	if(ifs.fail()) throw std::runtime_error((std::string("Unable to open file :") + m_meshfile_path).c_str());
		char line[MAX_LINE_LENGTH];
		char delimiters[4] = " ";
		float fx, fy, fz;		// floats required
		char* val = NULL;

		// used for providing local indices wrto only submesh vertices and not global vertex list
		int vtxOffset = 0;
		int norOffset = 0;
		int texOffset = 0;


		std::vector<SubMesh>::pointer current_mesh = NULL;

		while(ifs.peek() != EOF) {
			ifs.getline(line, sizeof(line), '\n');
			std::string inputString(line);
			if(!isComment(inputString) && inputString.length() > 1) {
				// now the line might be the start of an object, or might contain data for the object
				// if it contains an object 'o' then create a new object
				Tokens tokens = tokenize(inputString);
				if(isObject(tokens)) { 
					// add a new mesh

					// first update the offsets for currentmesh
					// reset current count
					vtxOffset = 0;
					norOffset = 0;
					texOffset = 0;
					for(size_t i = 0; i < m_meshes.size(); i++) {
						vtxOffset += m_meshes[i].m_vertices.size();
						norOffset += m_meshes[i].m_normals.size();
						texOffset += m_meshes[i].m_texcoords.size();
					}				

					SubMesh mesh;
					mesh.m_name = tokens.m_tokens[1];		// the second token is the mesh name
					m_meshes.push_back(mesh);
					// set the iterator to point to the current mesh
					current_mesh = &m_meshes.back();
				} else if(isVertex(tokens)) {
					assert(tokens.m_tokens.size() == 4);
					fx = float(atof(tokens.m_tokens[1].c_str()));
					fy = float(atof(tokens.m_tokens[2].c_str()));
					fz = float(atof(tokens.m_tokens[3].c_str()));
					current_mesh->m_vertices.push_back(make_float3(fx, fy, fz));
				} else if(isNormal(tokens)) {
					assert(tokens.m_tokens.size() == 4);				
					fx = float(atof(tokens.m_tokens[1].c_str()));
					fy = float(atof(tokens.m_tokens[2].c_str()));
					fz = float(atof(tokens.m_tokens[3].c_str()));
					current_mesh->m_normals.push_back(make_float3(fx, fy, fz));
				} else if(isTex(tokens)) {
					assert(tokens.m_tokens.size() == 3);
					fx = float(atof(tokens.m_tokens[1].c_str()));
					fy = float(atof(tokens.m_tokens[2].c_str()));
					current_mesh->m_texcoords.push_back(make_float2(fx, fy));
				} else if(isFace(tokens)) {
					// we do a relative ordering of mesh indexing.
					// that is each mesh has its indices with respect to its own vertex, index, list
					// each token of face is of the format v/vt/vn
					bool hasVtx, hasNor, hasTex;
					int3 v1 = parseFace(tokens.m_tokens[1], hasVtx, hasNor, hasTex);
					int3 v2 = parseFace(tokens.m_tokens[2], hasVtx, hasNor, hasTex);
					int3 v3 = parseFace(tokens.m_tokens[3], hasVtx, hasNor, hasTex);

					// subtract offsets from indices to provide indices wrto local mesh values only
					if(hasVtx) {
						current_mesh->m_vtxIndices.push_back(make_uint3(v1.x - vtxOffset - 1, v2.x - vtxOffset - 1, v3.x - vtxOffset - 1));
					}
					if(hasTex) {
						current_mesh->m_texIndices.push_back(make_uint3(v1.y - texOffset - 1, v2.y - texOffset - 1, v3.y - texOffset - 1));
					}
					if(hasNor) {
						current_mesh->m_norIndices.push_back(make_uint3(v1.z - norOffset - 1, v2.z - norOffset - 1, v3.z - norOffset - 1));
					}					
				}
			}
		}
	} catch (const std::exception e) {
		std::cerr<<e.what()<<"\n";
	}
}

float min(float a, float b) {	if(a < b) return a; return b;	}
float max(float a, float b) {	if(a > b) return a; return b;	}
