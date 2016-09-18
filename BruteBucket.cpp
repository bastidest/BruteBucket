#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;


class Bucket
{
private:
public:
	int maxVol;
	int curVol;
	string name;
	Bucket(int maximumVolume, int currentVolume, string bucketName);
	void fillBucket();
	void emptyBucket();
	void decantBucket(Bucket & destination);
	void print();
};

Bucket::Bucket(int maximumVolume, int currentVolume, string bucketName)
{
	maxVol = maximumVolume;
	curVol = currentVolume;
	name = bucketName;
}

void Bucket::emptyBucket()
{
	curVol = 0;
}

void Bucket::fillBucket()
{
	curVol = maxVol;
}

void Bucket::decantBucket(Bucket& destination)
{
	int freeDestinationSpace = destination.maxVol - destination.curVol;

	// if there is space left in destination bucket
	if (freeDestinationSpace > 0)
	{
		// if whole source bucket can be decanted into destination bucket without a overflow
		if (freeDestinationSpace >= curVol)
		{
			destination.curVol += curVol;
			curVol = 0;
		}
		// when the source bucket can only partially be decanted into the destination bucket
		else
		{
			curVol -= freeDestinationSpace;
			destination.curVol += freeDestinationSpace;
		}
	}
}

void Bucket::print()
{
	cout << name << "{" << curVol << "/" << maxVol << "}" << endl;
}


class BucketNode
{
public:
	BucketNode *decant1, *decant2, *empty1, *empty2, *fill1, *fill2, *parent;
	Bucket *one, *two;
	int depth;
	string action;
	BucketNode(Bucket *one_, Bucket *two_, int depth_);
};

BucketNode::BucketNode(Bucket *one_, Bucket *two_, int depth_)
{
	one = one_;
	two = two_;
	depth = depth_;
}

void mainLoop(BucketNode CurrentBucketNode);

BucketNode* decantBucket1(BucketNode *Parent)
{
	Bucket one = *(*Parent).one;
	Bucket two = *(*Parent).two;
	one.decantBucket(two);
	BucketNode current(&one, &two, (*Parent).depth + 1);
	current.action = "Decant Bucket 1 into Bucket 2";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

BucketNode* decantBucket2(BucketNode *Parent)
{
	Bucket one = *(*Parent).one;
	Bucket two = *(*Parent).two;
	two.decantBucket(one);
	BucketNode current(&one, &two, (*Parent).depth + 1);
	current.action = "Decant Bucket 2 into Bucket 1";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

BucketNode* emptyBucket1(BucketNode *Parent)
{
	Bucket one = *(*Parent).one;
	one.emptyBucket();
	BucketNode current(&one, (*Parent).two, (*Parent).depth + 1);
	current.action = "Empty Bucket 1";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

BucketNode* emptyBucket2(BucketNode *Parent)
{
	Bucket two = *(*Parent).two;
	two.emptyBucket();
	BucketNode current((*Parent).one, &two, (*Parent).depth + 1);
	current.action = "Empty Bucket 2";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

BucketNode* fillBucket1(BucketNode *Parent)
{
	Bucket one = *(*Parent).one;
	one.fillBucket();
	BucketNode current(&one, (*Parent).two, (*Parent).depth + 1);
	current.action = "Fill up Bucket 1";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

BucketNode* fillBucket2(BucketNode *Parent)
{
	Bucket two = *(*Parent).two;
	two.fillBucket();
	BucketNode current((*Parent).one, &two, (*Parent).depth + 1);
	current.action = "Fill up Bucket 2";
	current.parent = Parent;
	mainLoop(current);
	return &current;
}

int maxDepth, goal;
bool finished = false;

bool hasFinished(Bucket one, Bucket two)
{
	return finished || one.curVol == goal || two.curVol == goal;
}

void mainLoop(BucketNode CurrentBucketNode)
{
	if (CurrentBucketNode.depth > maxDepth || finished)
		return;

	Bucket one = *(CurrentBucketNode.one);
	Bucket two = *(CurrentBucketNode.two);

	if (one.curVol == goal || two.curVol == goal)
	{
		cout << "Found solution! depth=" << CurrentBucketNode.depth << endl;
		one.print();
		two.print();
		BucketNode *CurrentParent = CurrentBucketNode.parent;
		// while node has parent print out last action to solution
		while (CurrentParent != NULL)
		{
			cout << "--" << (*CurrentParent).action << endl;
			CurrentParent = (*CurrentParent).parent;
		}
		finished = true;
		return;
	}

	if (!hasFinished(one, two) && one.curVol > 0 && two.curVol != two.maxVol)
	{
		CurrentBucketNode.decant1 = decantBucket1(&CurrentBucketNode);
	}
	if (!hasFinished(one, two) && two.curVol > 0 && one.curVol != two.maxVol)
	{
		CurrentBucketNode.decant2 = decantBucket2(&CurrentBucketNode);
	}
	if (!hasFinished(one, two) && one.curVol > 0)
	{
		CurrentBucketNode.empty1 = emptyBucket1(&CurrentBucketNode);
	}
	if (!hasFinished(one, two) && two.curVol > 0)
	{
		CurrentBucketNode.empty2 = emptyBucket2(&CurrentBucketNode);
	}
	if (!hasFinished(one, two) && one.curVol < one.maxVol)
	{
		CurrentBucketNode.fill1 = fillBucket1(&CurrentBucketNode);
	}
	if (!hasFinished(one, two) && two.curVol < two.maxVol)
	{
		CurrentBucketNode.fill2 = fillBucket2(&CurrentBucketNode);
	}
}


int main()
{
	int maxVol1, maxVol2;
	cout << "Specify the maximum Volume of the first Bucket: ";
	cin >> maxVol1;
	cout << "Specify the maximum Volume of the second Bucket: ";
	cin >> maxVol2;
	cout << "Specify the Goal: ";
	cin >> goal;
	cout << "Specify the maximum search depth: ";
	cin >> maxDepth;

	if (goal > maxVol1 && goal > maxVol2)
	{
		cout << "Goal cannot be bigger than the largest Bucket!";
		return -1;
	}	
    
	Bucket Bucket1(maxVol1, 0, "Bucket1");
	Bucket Bucket2(maxVol2, 0, "Bucket2");

	BucketNode Root(&Bucket1, &Bucket2, 0);

	mainLoop(Root);

	system("PAUSE");
}

