#include "FBXExporter.h"

int main()
{
	FBXExporter fbxExporter{};
	fbxExporter.Process("../Resource/CUBE.fbx", "result.bin");
}