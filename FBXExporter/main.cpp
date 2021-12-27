#include "main.h"
#include "FBXExporter.h"

int main()
{
	FBXExporter fbxLoader{};
	fbxLoader.Process("../Resource/CUBE.fbx");
}