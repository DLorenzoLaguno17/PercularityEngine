#ifndef __DEBUG_H__
#define __DEBUG_H__

class Debug
{
public:
	static bool drawObjectsAABB;

	static bool drawSceneTree;
	static bool drawScenePlane;
	static bool drawSceneAxis;

	static bool frustumAccelerated;
	static bool frustumCullingActive;
	
	static bool drawColliders;
	static bool drawMouseLine;
};

#endif