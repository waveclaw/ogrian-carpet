/*****************************************************************************
	Copyright 2004 Mike Prosser

    This file is part of Ogrian Carpet.

    Ogrian Carpet is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Ogrian Carpet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ogrian Carpet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*****************************************************************************/

/*------------------------------------*
OgrianThing.h
Original Author: Mike Prosser
Additional Authors: 

Description: the Thing class is the basic moveable object. 
It interacts with the renderer and the physics engine. 
It is rendered as a Billboard.

/*------------------------------------*/

#ifndef __OgrianThing_H__
#define __OgrianThing_H__

#include <Ogre.h>
#include "OgrianSprite.h"
#include "OgrianAudio.h"

using namespace Ogre;

namespace Ogrian
{

enum ThingType
{
	THING,
	ROLLINGTHING,
	FLOATINGTHING,
	MANATHING,
	FOLIAGETHING,
	CAMERATHING,
};

// this is used for collision detection
enum ThingShape
{
	SPHERE,
	CYLINDER,
	CUBE,
};

class Thing
{
public:
	Thing(String material, String prefix="Thing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE);
	virtual ~Thing();

	virtual void setVelocity(Vector3 vel);

	// set the position
	virtual void setPosition(Vector3 pos);

	virtual void setScale(Real scale);
	virtual void setWidth(Real width);
	virtual void setHeight(Real height);

	virtual void setMaterial(String material);

	virtual inline void setShape(ThingShape);
	virtual inline ThingShape getShape();

	// applies the velocity to the position
	virtual void move(Real time);

	// calculate the x/z distance between two Things
	virtual Real cylinderDistance(Thing* e);

	// calculate the real distance between two Things
	virtual Real sphereDistance(Thing* e);

	// calculate the rectangular distance between two things using only the x and z axis
	// this is like using a square instead of a circle for distance
	virtual Real axisDistance(Thing* e);

	// get the position
	virtual Vector3 getPosition();

	// get the velocity
	virtual Vector3 getVelocity();
	virtual Real getWidth();
	virtual Real getHeight();

	// each thing has a type so things can tell what they've collided with
	virtual ThingType getType(); 

	// override this for interesting behaviors
	virtual void collided(Thing* e);

	// get rid of this thing - sets alive to false and will cause this thing to be deleted
	virtual void destroy();

	// returns fale if this thing has been destroy()ed and is awaiting deletion.
	virtual bool isAlive();

	// the physics engine calls this when this thing is added to the physics engine
	virtual void placedInPhysics();

	// conveniance methods for accessing the y axis
	virtual void setPosY(Real y);
	virtual void setVelY(Real y);
	virtual Real getPosY();
	virtual Real getVelY();

	virtual Real getGroundY();
	virtual Real getGroundY(Vector3 pos);

	// play a sound
	virtual void playSound(String filename);

	
private:
	Vector3 mPos;
	Vector3 mVel;

	// graphical rendering stuff
	Sprite* mSprite;
	String mName;

	Real mWidth;
	Real mHeight; 

	ThingShape mShape;

	bool mAlive;

	bool mInPhysics; // wether or not its in the physics engine yet

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	virtual void _addToRenderer();
	virtual void _removeFromRenderer();
	virtual void _updateVisibility(); // based on distance from camera
};

}
#endif