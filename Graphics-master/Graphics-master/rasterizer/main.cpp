/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int g_width, g_height;

class Vertex{
      public:
      float x,y,z;
      void set_values (float,float, float);
   } v1,v2,v3,v;
   void Vertex::set_values (float a, float b, float c) {
      x = a;
      y = b;
      z = c;
   }



/*
   Helper function you will want all quarter
   Given a vector of shapes which has already been read from an obj file
   resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      }
   }

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}

void getMin(Vertex v1, Vertex v2, Vertex v3) {
	float minx = fmin(v1.x, v2.x);
	minx = fmin(minx, v3.x);
	float miny = fmin(v1.y, v2.y);
	miny = fmin(miny, v3.y);

	float maxx = fmax(v1.x, v2.x);
	maxx = fmax(maxx, v3.x);
	float maxy = fmax(v1.y, v2.y);
	maxy = fmax(maxy, v3.y);
}

float area(Vertex a, Vertex b, Vertex c) {
	return abs((1.0*a.x*(b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y)) / 2);
}

int main(int argc, char **argv)
{
	/*if(argc < 6) {
      cout << "Usage: Assignment1 meshfile imagefile width height coloringMode" << endl;
      return 0;
   }*/
	// OBJ filename

	//string meshName(argv[1]);
	//string imgName(argv[2]);
	string meshName("resources/bunny.obj");
	string imgName("output.png");

	//set g_width and g_height appropriately!
   	g_width = g_height = 400;
   //g_width=atoi(argv[3]);
   //g_height=atoi(argv[4]);

	int colorMode = atoi("1");

   //int colorMode=atoi(argv[5]);

   //create an image
	auto image = make_shared<Image>(g_width, g_height);

	// triangle buffer
	vector<unsigned int> triBuf;
	// position buffer
	vector<float> posBuf;
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes; // geometry
	vector<tinyobj::material_t> objMaterials; // material
	string errStr;
	
   bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	/* error checking on read */
	if(!rc) {
		cerr << errStr << endl;
	} else {
      //keep this code to resize your object to be within -1 -> 1
      resize_obj(shapes); 
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	cout << "Number of vertices: " << posBuf.size()/3 << endl;
	cout << "Number of triangles: " << triBuf.size()/3 << endl;

 	
   
   //set bounding box of camera based on output size
   float l,r,b,t,pos;
   
   if (g_width>g_height){
      l=-1.0*g_width/g_height;
      r=1.0*g_width/g_height;
      b=-1.0;
      t=1.0;
   } else {
      l=-1.0;
      r=1.0;
      b=-1.0*g_height/g_width;
      t=1.0*g_height/g_width;
   }
   int cx=1.0*(g_width-1)/(r-l);
   int dx=-1.0*l*cx;
   int cy=1.0*(g_height-1)/(t-b);
   int dy=-1.0*b*cy;
   
   float areaTri, curZ, alpha, beta, gamma;

   vector<float> zbuffer(g_width*g_height);
   for (int i=0;i<g_width*g_height;i++){
      zbuffer[i]=-2;
   }

   //iterate throught faces
   for (int i = 0; i < triBuf.size() / 3; i++) {
      
      pos=triBuf[3*i+0];
      v1.set_values(posBuf[pos*3], posBuf[pos*3+1], posBuf[pos*3+2]);
      
      pos=triBuf[3*i+1];
      v2.set_values(posBuf[pos*3], posBuf[pos*3+1], posBuf[pos*3+2]);
      
      pos=triBuf[3*i+2];
      v3.set_values(posBuf[pos*3], posBuf[pos*3+1], posBuf[pos*3+2]);
      
      //TODO? compute xy coordinate in pixel space, save z in world space
      
      v1.set_values(cx*v1.x+dx, cy*v1.y+dy, v1.z);
      v2.set_values(cx*v2.x+dx, cy*v2.y+dy, v2.z);
      v3.set_values(cx*v3.x+dx, cy*v3.y+dy, v3.z);
      
      areaTri=area(v1,v2,v3);
      
      float minx=fmin(v1.x,v2.x);
      minx=fmin(minx,v3.x);
      float miny=fmin(v1.y,v2.y);
      miny=fmin(miny,v3.y);
   
      float maxx=fmax(v1.x,v2.x);
      maxx=fmax(maxx,v3.x);
      float maxy=fmax(v1.y,v2.y);
      maxy=fmax(maxy,v3.y);
      
      
      // iterate through points in bounding box
      for(int x = minx; x < maxx; x++) {
         for(int y = miny; y < maxy; y++) {
            //calculate barycentric coordinates of pixel
            v.set_values(x,y,0);
            alpha=area(v1,v2,v)/areaTri;
            beta=area(v1,v3,v)/areaTri;
            gamma=area(v2,v3,v)/areaTri;
         
            if (alpha+gamma+beta<=1.01 && -0.01<=alpha && alpha<=1.01 && -0.01<=beta && beta<=1.01 && -0.01<=gamma && gamma<=1.01 ){
               //draw
               if(colorMode==1){
                  curZ=alpha*v3.z+beta*v2.z+gamma*v1.z;
                  
                  if(curZ>zbuffer[x+y*g_width]){
                     unsigned char r = (curZ+1.0)/2*0;
                     unsigned char g = (curZ+1.0)/2*22;
                     unsigned char b = (curZ+1.0)/2*205;
                     
                     image->setPixel(x, y, r, g, b);
                     zbuffer[x+y*g_width]=curZ;
                  } 
               } else{
                  float percent=y*1.0/g_height;
                  unsigned char r = percent*4+(1-percent)*0;
                  unsigned char g = percent*1+(1-percent)*255;
                  unsigned char b = percent*232+(1-percent)*226;
                  image->setPixel(x, y, r, g, b);
               }
            }
         }
		}	
	}
	//write out the image
   image->writeToFile(imgName);

	return 0;
}
