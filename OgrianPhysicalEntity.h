#ifndef __OgrianPhysicalEntity_H__
#define __OgrianPhysicalEntity_H__

#include <Ogre.h>

using namespace Ogre;

// The OgrianPhysicalEntity class is the root of all objects that participate in physics. 
class OgrianPhysicalEntity
{
public:
	Vector3 pos;
	Vector3 vel;

	OgrianPhysicalEntity()
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;
	}

	OgrianPhysicalEntity(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	virtual void move(long time)
	{
		time /= 1000;
		pos.x += vel.x * time;
		pos.y += vel.y * time;
		pos.z += vel.z * time;
	}

	// they are ordered by x location
	bool operator<(OgrianPhysicalEntity* other)
	{
		return (pos.x < other->pos.x);
	}

	
private:
};

#endif