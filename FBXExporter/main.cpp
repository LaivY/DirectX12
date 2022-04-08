#include "FBXExporter.h"

int main()
{
	FBXExporter fbxExporter{};
	for (const string& fileName : { "temp" })
	{
		std::cout << fileName + ".fbx : PROCESS";
		fbxExporter.Process("target/" + fileName + ".fbx", true, true);
		std::cout << "\r" << fileName + ".fbx : COMPLETE" << std::endl;
	}
}