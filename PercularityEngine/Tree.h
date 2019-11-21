#ifndef _TREE_H__
#define _TREE_H__

#include <vector>
#include "Globals.h"

class GameObject;
class TreeNode;

enum class TREE_TYPE {
	NONE=-1,
	QUADTREE,
	OCTREE
	//KDTREE  -> Might be implemented in the future
};

enum class NODE_TYPE {
	NONE=-1,
	ROOT, 
	BRANCH,
	LEAF
};

class Tree {

public:

	//Constructor & destructor
	Tree(TREE_TYPE type, AABB aabb, int capacity);
	Tree(TREE_TYPE type, float3 minPoint, float3 maxPoint,int capacity);
	~Tree();

	void Draw();
	void Clear();
	
	bool Insert(GameObject* gameObject);
	
	std::vector<GameObject*> CollectChilldren(Frustum frustum);
	std::vector<GameObject*> CollectChilldren(AABB aabb_);
	//bool Insert(GameObject* gameObject);

	
public:

	TREE_TYPE type = TREE_TYPE::NONE;

	TreeNode* rootNode;

};

class TreeNode {
	friend class Tree;
public:
	
	//Constructor & destructor
	TreeNode();
	TreeNode(AABB aabb, TREE_TYPE type, NODE_TYPE ntype,int capacity);
	~TreeNode();

	void Clear();
	void Split();

	void Draw();
	bool Insert(GameObject* gameObject);

	std::vector<GameObject*> CollectChilldren(Frustum frustum);
	std::vector<GameObject*> CollectChilldren(AABB aabb_);

private:

	void QuadSplit();
	void OctSplit();

public:

	bool isLeaf = true;

	NODE_TYPE nodeType = NODE_TYPE::NONE;
	TREE_TYPE treeType = TREE_TYPE::NONE;
	AABB aabb;

	TreeNode *nodes=nullptr;

	std::vector<GameObject*> objects;

private:
	int nodesAmount=0;
	int capacity = 0;
};

#endif
