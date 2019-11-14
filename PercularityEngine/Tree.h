#ifndef _TREE_H__
#define _TREE_H__

#include "MathGeoLib/include/MathGeoLib.h"
#include <vector>

class GameObject;
class TreeNode;

enum class TREE_TYPE {
	NONE=-1,
	QUADTREE,
	OCTREE
	//KDTREE  -> Might be implemented in the future
};


class Tree {

public:

	//Constructor & destructor
	Tree(TREE_TYPE type, AABB aabb);
	Tree(TREE_TYPE type, float3 minPoint, float3 maxPoint);
	~Tree();

	void Draw();
	//bool Insert(GameObject* gameObject);

	
public:

	TREE_TYPE type = TREE_TYPE::NONE;

	TreeNode* rootNode;

};

class TreeNode {
public:
	
	//Constructor & destructor
	TreeNode();
	TreeNode(AABB aabb, TREE_TYPE type);
	~TreeNode();

	void Split();

	void Draw();

private:

	void QuadSplit();
	void OctSplit();

public:

	bool isLeave = true;

	TREE_TYPE treeType = TREE_TYPE::NONE;
	AABB aabb;

	TreeNode *nodes;

	std::vector<GameObject*> objects;

private:
	int nodesAmount=0;
};

#endif
