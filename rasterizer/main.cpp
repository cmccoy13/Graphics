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

/*
Helper function you will want all quarter
Given a vector of shapes which has already been read from an obj file
resize all vertices to the range [-1, 1]
*/
void resize_obj(std::vector<tinyobj::shape_t> &shapes) {
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
			if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
			if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

			if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
			if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

			if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
			if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
		}
	}

	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0 / maxExtent;
	shiftX = minX + (xExtent / 2.0);
	scaleY = 2.0 / maxExtent;
	shiftY = minY + (yExtent / 2.0);
	scaleZ = 2.0 / maxExtent;
	shiftZ = minZ + (zExtent) / 2.0;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
		}
	}
}

bool triangleContains(float ax, float ay, float bx, float by, float cx, float cy, float x, float y)
{

	ax = (ax + 1) * 50;
	ay = (ay + 1) * 50;
	bx = (bx + 1) * 50;
	by = (by + 1) * 50;
	cx = (cx + 1) * 50;
	cy = (cy + 1) * 50;

	float det = (bx - ax) * (cy - ay) - (by - ay) * (cy - ay);

	return  det * ((bx - ax) * (y - ay) - (by - ay) * (x - ax)) > 0 &&
		det * ((cx - bx) * (y - by) - (cy - by) * (x - bx)) > 0 &&
		det * ((ax - cx) * (y - cy) - (ay - cy) * (x - cx)) > 0;

}

int main(int argc, char **argv)
{
	/*if (argc < 3) {
	cout << "Usage: Assignment1 meshfile imagefile" << endl;
	return 0;
	}*/
	// OBJ filename
	//string meshName(argv[1]);
	//string imgName(argv[2]);

	string meshName("resources/tri.obj");
	string imgName("image.png");

	//set g_width and g_height appropriately!
	g_width = g_height = 100;

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
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		//keep this code to resize your object to be within -1 -> 1
		resize_obj(shapes);
		posBuf = shapes[0].mesh.positions;
		triBuf = shapes[0].mesh.indices;
	}
	cout << "Number of vertices: " << posBuf.size() / 3 << endl;
	cout << "Number of triangles: " << triBuf.size() / 3 << endl;

	//TODO add code to iterate through each triangle and rasterize it 

	float xMin = posBuf[0];
	float xMax = posBuf[0];
	float yMin = posBuf[1];
	float yMax = posBuf[1];

	for (int i = 3; i < posBuf.size(); i++)
	{
		if (posBuf[i] > xMax)
		{
			xMax = posBuf[i];
		}
		if (posBuf[i] < xMin)
		{
			xMin = posBuf[i];
		}

		i++;

		if (posBuf[i] > yMax)
		{
			yMax = posBuf[i];
		}
		if (posBuf[i] < yMin)
		{
			yMin = posBuf[i];
		}

		i++;
	}

	//Bounding box contained in xMin/Max, yMin/Max
	xMin = (xMin + 1) * 50;
	xMax = (xMax + 1) * 50;
	yMin = (yMin + 1) * 50;
	yMax = (yMax + 1) * 50;

	for (float i = xMin; i <= xMax; i++) {
		for (float j = yMin; j <= yMax; j++) {
			for (int k = 0; k < triBuf.size(); k += 3) {
				float ax = posBuf[triBuf[k] * 3]; //v1, x
				float ay = posBuf[triBuf[k] * 3 + 1]; //v1, y
				float bx = posBuf[triBuf[k + 1] * 3]; //v2, x
				float by = posBuf[triBuf[k + 1] * 3 + 1]; //v2, y
				float cx = posBuf[triBuf[k + 2] * 3]; //v2, x
				float cy = posBuf[triBuf[k + 2] * 3 + 1]; //v2, y

				if (triangleContains(ax, ay, bx, by, cx, cy, i, j))
				{
					image->setPixel((int)i, (int)j, 255, 0, 0);
				}
				else if (triangleContains(bx, by, cx, cy, ax, ay, i, j))
				{
					image->setPixel((int)i, (int)j, 255, 0, 0);
				}
				else if (triangleContains(cx, cy, ax, ay, bx, by, i, j))
				{
					image->setPixel((int)i, (int)j, 255, 0, 0);
				}
			}
		}
	}

	//image->setPixel(50, 50, 255, 0, 0);
	//image->setPixel(49, 50, 255, 0, 0);

	//write out the image
	image->writeToFile(imgName);

	return 0;
}