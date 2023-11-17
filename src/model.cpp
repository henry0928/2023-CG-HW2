#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/vec3.hpp>

using namespace std ;

void parse_face(string str, vector<int>& vertex, vector<int>& texture, vector<int>& normal) { 
  string index = "";
  vector<int> vec;
  int count = 0;
  // cout << "str:" << str << endl;
  for (int i = 0; i < str.length(); i++) {
    if ( str[i] == '/'  || i == str.length()-1 ) {
      count++;
      if (i == str.length() - 1) 
        index = index + str[i];
      vec.push_back(atoi( index.c_str() ) );
      index = "";
    } // if
    else
      index = index + str[i];
  } // for 

  vertex.push_back(vec[0]);
  texture.push_back(vec[1]);
  normal.push_back(vec[2]);

}  // parse_face()

vector<int> parse_face_v2(string str) {
  string index = "";
  int count = 0;
  vector<int> vec;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '/' || i == str.length() - 1) {
      count++;
      if (i == str.length() - 1) 
        index = index + str[i];
      vec.push_back(atoi(index.c_str()));
      index = "";
    }  // if
    else
      index = index + str[i];
  }  // for

  return vec;

}  // parse_face()

Model* Model::fromObjectFile(const char* obj_file) {
  Model* m = new Model();

  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return NULL;
  }

  string str, temp, ignore;
  stringstream ss("");
  string info; 
  int count ; 
  count = 0;
  float f_temp;
  float u, w;
  vector<int>index_vec ; 
  vector<float> all_vertex;
  vector<float> all_normal;
  vector<float> all_texture;
  while (getline(ObjFile, str)) {
    ss.str("");
    ss.clear();
    ss << str;
    ss >> temp;
    if (temp == "v") {
      f_temp = 0.0;
      while (ss >> f_temp) {
        all_vertex.push_back(f_temp);
        f_temp = 0.0; 
      } // while
    }   // if
    else if (temp == "vt") {
      u = 0.0;
      w = 0.0;
      while (ss >> u >> w ) {
        all_texture.push_back(u);
        all_texture.push_back(w);
        u = 0.0;
        w = 0.0;
      } // while
    }  // else if
    else if (temp == "vn") {
      f_temp = 0.0;
      while (ss >> f_temp) {
        all_normal.push_back(f_temp);
        f_temp = 0.0; 
      } // while 
    }  // else if
    else if (temp == "f") {
      while (ss >> info) {
        count++;
        index_vec = parse_face_v2(info);
        m->positions.push_back( all_vertex[ (index_vec[0] - 1) *3 ] );
        m->positions.push_back(all_vertex[(index_vec[0] - 1) * 3 + 1]);
        m->positions.push_back(all_vertex[(index_vec[0] - 1) * 3 + 2]);
        m->texcoords.push_back(all_texture[(index_vec[1] - 1) * 2]);
        m->texcoords.push_back(all_texture[(index_vec[1] - 1) * 2 + 1]);
        m->normals.push_back(all_normal[(index_vec[2] - 1) * 3]);
        m->normals.push_back(all_normal[(index_vec[2] - 1) * 3 + 1]);
        m->normals.push_back(all_normal[(index_vec[2] - 1) * 3 + 2]);
      } // while       

    }  // else if
    else {
      while (ss >> ignore) ;
    } // else 

    str = "";
    temp = "";
  } // while

   m->numVertex = m->positions.size() / 3 ;
   all_vertex.clear(); 
   all_texture.clear();
   all_normal.clear();
  /* TODO#1: Load model data from OBJ file
   *         You only need to handle v, vt, vn, f
   *         Other fields you can directly ignore
   *         Fill data into m->positions, m->texcoords m->normals and m->numVertex
   *         Data format:
   *           For positions and normals
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | ...
   *         | face 1                                                       | face 2               ...
   *         | v1x  | v1y  | v1z  | v2x  | v2y  | v2z  | v3x  | v3y  | v3z  | v1x  | v1y  | v1z  | ...
   *         | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | vn1x | vn1y | vn1z | ...
   *           For texcoords
   *         | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | ...
   *         | face 1                                  | face 2        ...
   *         | v1x  | v1y  | v2x  | v2y  | v3x  | v3y  | v1x  | v1y  | ...
   * Note:
   *        OBJ File Format (https://en.wikipedia.org/wiki/Wavefront_.obj_file)
   *        Vertex per face = 3 or 4
   */


  if (ObjFile.is_open()) ObjFile.close();

  return m;
}
