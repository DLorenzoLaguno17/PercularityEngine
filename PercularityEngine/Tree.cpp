#include "Tree.h"
#include "OpenGL.h"
#include "GameObject.h"
#include "Intersection.h"
#include "mmgr/mmgr.h"

Tree::Tree(TREE_TYPE type,AABB aabb,int capacity) :type(type)
{
	rootNode = new TreeNode(aabb,type, NODE_TYPE::ROOT,capacity);

	rootNode->capacity = capacity;
	rootNode->treeType = type;
	rootNode->aabb= aabb;

}

Tree::Tree(TREE_TYPE type, float3 minPoint, float3 maxPoint,int capacity) :type(type)
{
	rootNode = new TreeNode();

	rootNode->capacity = capacity;
	rootNode->treeType = type;
	rootNode->aabb.minPoint = minPoint;
	rootNode->aabb.maxPoint = maxPoint;
}

Tree::~Tree()
{}

void Tree::Draw()
{
	rootNode->Draw();
}

bool Tree::Insert(GameObject* gameObject)
{
	return rootNode->Insert(gameObject);
}

void Tree::Erase(GameObject* gameObject)
{
	rootNode->Erase(gameObject);
}

void Tree::Clear()
{
	rootNode->Clear();
}

void Tree::CollectChilldren(const AABB& aabb_, std::vector<const GameObject*>& vector)
{
	rootNode->CollectChilldren(aabb_,vector);
}

void Tree::CollectChilldren(const Frustum& frustum, std::vector<const GameObject*>& vector)
{
	rootNode->CollectChilldren(frustum, vector);
}

void Tree::CollectChilldren(const LineSegment& ray, std::map<float,const GameObject*>& container,bool nearest)
{
	rootNode->CollectChilldren(ray,container,nearest);
}

//~~~~~~~~NODE METHODS~~~~~~~~~~

TreeNode::TreeNode()
{
}

TreeNode::TreeNode(AABB aabb, TREE_TYPE type, NODE_TYPE ntype,int capacity): aabb(aabb),treeType(type),nodeType(ntype),capacity(capacity)
{
}

TreeNode::~TreeNode()
{}

void TreeNode::Split()
{
	
	switch (treeType)
	{
	case TREE_TYPE::QUADTREE:
		QuadSplit();
		break;
	case TREE_TYPE::OCTREE:
		OctSplit();
		break;
	}
}

void TreeNode::Draw()
{
	glLineWidth(7.0f);

	glBegin(GL_LINES);

	if (objects.empty())
		glColor3f(0, 0, 1); //Blue color
	else 
		glColor3f(1, 0, 0); //red color

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());
	
	glColor3f(1, 1, 1); //red color
	glEnd();

	if (nodeType!=NODE_TYPE::LEAF)
	{
		for (int i = 0; i < nodesAmount ; ++i)
			nodes[i].Draw();
	}
}

void TreeNode::QuadSplit()
{
	//Subdivide the AABB     x)
	AABB newAABBs[4];
	float3 maxPoint, minPoint;
/*
{maxx, maxy maxz)
	 ___________
	|	  |		|
	|_____|_____|
	|	  |		|
	|_____|_____|
				{minx, maxy minz)

*/
	//NORTH-WEST subnode
	maxPoint = {aabb.MaxX(), aabb.MaxY(), aabb.MaxZ() };
	minPoint = {(aabb.MaxX() + aabb.MinX())/2 , aabb.MinY(), (aabb.MaxZ()+aabb.MinZ())/2 };
	newAABBs[0].minPoint = minPoint;
	newAABBs[0].maxPoint = maxPoint;

	//NORTH-EAST subnode
	maxPoint = {( aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(), aabb.MaxZ() };
	minPoint = { aabb.MinX(), aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[1].minPoint = minPoint;
	newAABBs[1].maxPoint = maxPoint;

	//SOUTH-WEST subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MinY(), aabb.MinZ()};
	newAABBs[2].minPoint = minPoint;
	newAABBs[2].maxPoint = maxPoint;

	//SOUTH-EAST subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), aabb.MinY(), aabb.MinZ() };
	newAABBs[3].minPoint = minPoint;
	newAABBs[3].maxPoint= maxPoint;

	nodes = new TreeNode[4];

	this;

	for (int i = 0; i < 4; ++i) 
		nodes[i] = TreeNode(newAABBs[i], treeType, NODE_TYPE::LEAF,capacity);
	
	if (nodeType!=NODE_TYPE::ROOT)
		nodeType = NODE_TYPE::BRANCH;

	nodesAmount = 4;
}

void TreeNode::OctSplit()
{
	//Subdivide the AABB     x)
	AABB newAABBs[8];
	float3 maxPoint, minPoint;
	/*
	{maxx, maxy, maxz)
		 ___________
		|	  |		|
		|_____|_____|
		|	  |		|
		|_____|_____|
					{minx, maxy, minz)

	*/
	//NORTH-WEST-TOP subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(), aabb.MaxZ() };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2 , (aabb.MinY() + aabb.MaxY()) / 2, (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[0].minPoint = minPoint;
	newAABBs[0].maxPoint = maxPoint;

	//NORTH-EAST-TOP subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(), aabb.MaxZ() };
	minPoint = { aabb.MinX(), (aabb.MinY() + aabb.MaxY()) / 2, (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[1].minPoint = minPoint;
	newAABBs[1].maxPoint = maxPoint;

	//SOUTH-WEST-TOP subnode
	maxPoint = { aabb.MaxX(), aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, (aabb.MinY() + aabb.MaxY()) / 2, aabb.MinZ() };
	newAABBs[2].minPoint = minPoint;
	newAABBs[2].maxPoint = maxPoint;

	//SOUTH-EAST-TOP subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MaxY(),(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), (aabb.MinY() + aabb.MaxY()) / 2, aabb.MinZ() };
	newAABBs[3].minPoint = minPoint;
	newAABBs[3].maxPoint = maxPoint;

	//NORTH-WEST-BOTTOM subnode
	maxPoint = { aabb.MaxX(), (aabb.MinY() + aabb.MaxY()) / 2, aabb.MaxZ() };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2 ,aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[4].minPoint = minPoint;
	newAABBs[4].maxPoint = maxPoint;

	//NORTH-EAST-BOTTOM subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2,(aabb.MinY() + aabb.MaxY()) / 2, aabb.MaxZ() };
	minPoint = { aabb.MinX(),aabb.MinY(), (aabb.MaxZ() + aabb.MinZ()) / 2 };
	newAABBs[5].minPoint = minPoint;
	newAABBs[5].maxPoint = maxPoint;

	//SOUTH-WEST-BOTTOM subnode
	maxPoint = { aabb.MaxX(), (aabb.MinY() + aabb.MaxY()) / 2,(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { (aabb.MaxX() + aabb.MinX()) / 2, aabb.MinY(), aabb.MinZ() };
	newAABBs[6].minPoint = minPoint;
	newAABBs[6].maxPoint = maxPoint;

	//SOUTH-EAST-BOTTOM subnode
	maxPoint = { (aabb.MaxX() + aabb.MinX()) / 2,(aabb.MinY() + aabb.MaxY()) / 2,(aabb.MaxZ() + aabb.MinZ()) / 2 };
	minPoint = { aabb.MinX(), aabb.MinY(), aabb.MinZ() };
	newAABBs[7].minPoint = minPoint;
	newAABBs[7].maxPoint = maxPoint;

	nodes = new TreeNode[8];

	for (int i = 0; i < 8; ++i) 
		nodes[i] = TreeNode(newAABBs[i], treeType,NODE_TYPE::LEAF,capacity);

	if (nodeType != NODE_TYPE::ROOT)
		nodeType = NODE_TYPE::BRANCH;
	nodesAmount = 8;
	
}

bool TreeNode::Insert(GameObject* gameObject)
{
	if (!aabb.Intersects(gameObject->aabb))
		return false;

	int nodesContaining = 0;
	int container = 0;

	switch (nodeType)
	{
	case NODE_TYPE::ROOT:
		if (nodesAmount == 0) //Not dividied yet --> Behaves as a leaf node
		{
			objects.push_back(gameObject);

			if (objects.size() > capacity)
			{
				Split();

				std::vector<GameObject*> auxVector = objects;
				objects.clear();

				//If we split the node, we have to distribute the game objects contained
				for (int a = 0; a < auxVector.size(); ++a)
				{
					nodesContaining = 0;
					container = 0;
					for (int i = 0; i < nodesAmount; ++i)
					{
						if (nodes[i].aabb.Intersects(auxVector[a]->aabb))
						{
							nodesContaining++;
							container = i;
						}
						if (nodesContaining > 1)
							break;
					}

					//If the object only fits in one of the nodes, 
					//put it inside it, and split the node if needed
					if (nodesContaining == 1)
						nodes[container].Insert(auxVector[a]);

					else if (nodesContaining > 1)
						objects.push_back(auxVector[a]);
				}

			}
		}

		else				//If divided, behaves as a branch node
		{
			//See how many subnodes intersect with this gameobject
			for (int i = 0; i < nodesAmount; ++i)
			{
				if (nodes[i].aabb.Intersects(gameObject->aabb))
				{
					nodesContaining++;
					container = i;
				}
				if (nodesContaining > 1)
					break;
			}

			//If the object only fits in one of the nodes, 
			//put it inside it, and split the node if needed
			if (nodesContaining == 1)
				nodes[container].Insert(gameObject);

			else if (nodesContaining > 1)
				objects.push_back(gameObject);
		}

		break;
	case NODE_TYPE::BRANCH:

		//See how many subnodes intersect with this gameobject
		for (int i = 0; i < nodesAmount; ++i)
		{
			if (nodes[i].aabb.Intersects(gameObject->aabb))
			{
				nodesContaining++;
				container = i;
			}
			if (nodesContaining > 1)
				break;
		}

		//If the object only fits in one of the nodes, 
		//put it inside it, and split the node if needed
		if (nodesContaining == 1)
			nodes[container].Insert(gameObject);

		else if (nodesContaining > 1)
			objects.push_back(gameObject);

		break;

	case NODE_TYPE::LEAF:
		
		objects.push_back(gameObject);
		
		if (objects.size() > capacity)
		{
			Split();

			std::vector<GameObject*> auxVector = objects;
			objects.clear();

			//If we split the node, we have to distribute the game objects contained
			for (int a = 0; a < auxVector.size(); ++a)
			{
				nodesContaining = 0;
				container = 0;
				for (int i = 0; i < nodesAmount; ++i)
				{
					if (nodes[i].aabb.Intersects(auxVector[a]->aabb))
					{
						nodesContaining++;
						container = i;
					}
					if (nodesContaining > 1)
						break;
				}

				//If the object only fits in one of the nodes, 
				//put it inside it, and split the node if needed
				if (nodesContaining == 1)
					nodes[container].Insert(auxVector[a]);

				else if (nodesContaining > 1)
					objects.push_back(auxVector[a]);
			}
			
		}
		

		break;
	}
	return true;

}

void TreeNode::CollectChilldren(const AABB& aabb_, std::vector<const GameObject*>& vector)
{

	if (!aabb.Intersects(aabb_))
		return;

	for (int i = 0; i < objects.size(); ++i)
		vector.push_back(objects[i]);

	for (int i = 0; i < nodesAmount; ++i)
		nodes[i].CollectChilldren(aabb_,vector);

}

void TreeNode::CollectChilldren(const Frustum& frustum, std::vector<const GameObject*>& vector)
{

	if (!Intersect(frustum,aabb))
		return;

	for (int i = 0; i < objects.size(); ++i)
		vector.push_back(objects[i]);

	for (int i = 0; i < nodesAmount; ++i)
		nodes[i].CollectChilldren(frustum,vector);
	

}

void TreeNode::CollectChilldren(const LineSegment& ray, std::map<float, const GameObject*>& container, float nearest)
{
	if (ray.Intersects(aabb)){
		for (int i = 0; i < objects.size(); ++i){
			float nearHit, farHit;
			if (objects[i]->aabb.Intersects(ray, nearHit,farHit) ) {
				if (nearest)
					container[nearHit] = objects[i];
				else
					container[farHit] = objects[i];
			}
		}
		
		for (int i = 0; i < nodesAmount; ++i)
			nodes[i].CollectChilldren(ray, container, nearest);

	}

}

void TreeNode::Clear()
{
	for (int i = 0; i < nodesAmount; ++i)
	{
		nodes[i].Clear();
	}
	
	if (nodes != nullptr)
		delete[] nodes;

	objects.clear();
}

void TreeNode::ClearSubNodes()
{
	for (int i = 0; i < nodesAmount; ++i)
		nodes[i].Clear();

	if (nodes != NULL)
		delete[] nodes;

	nodesAmount = 0;
	if (nodeType != NODE_TYPE::ROOT)
		nodeType = NODE_TYPE::LEAF;
}

void TreeNode::Erase(GameObject* gameObject)
{
	for (int i = 0; i < objects.size(); ++i)
	{
		if (objects[i] == gameObject) {
			objects.erase(objects.begin()+i);
			return;
		}
	}

	if (nodesAmount > 0)
	{
		for (int i = 0; i < nodesAmount; ++i)
		{
			nodes[i].Erase(gameObject);
		}

		//In case the subnodes are the leaves, and become empty
		//after erasing the object, clear them
		if (nodes[0].nodeType == NODE_TYPE::LEAF)
		{
			bool nodesEmpty=true;
			for (int i = 0; i < nodesAmount; ++i)
			{
				if (!nodes[i].objects.empty()) {
					nodesEmpty = false;
					break;
				}
			}
			if (nodesEmpty)
				ClearSubNodes();
		}
	}
}

