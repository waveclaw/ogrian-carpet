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
#include "OgrianPacketEnum.h"

#include "BitStream.h"

using namespace Ogre;

namespace Ogrian
{

enum ThingType
{
	THING,
	EFFECT,
	ROLLINGTHING,
	FLOATINGTHING,
	MANATHING,
	FIREBALLTHING,
	FOLIAGETHING,
	CAMERATHING,
	WIZARDTHING,
	CASTLETOWER,
	CASTLEWALL,
};

enum ThingVisRep
{
	SPRITE,
	ORIENTEDSPRITE,
	MODEL,
};

enum ThingUpdateType
{
	PERIODIC,
	CONTINUOUS,
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
	Thing(String material, ThingVisRep visrep=SPRITE, String prefix="Thing", bool fixed_y=false, 
		Real scale=1, Vector3 pos=Vector3(0,0,0), ThingShape shape=SPHERE);
	virtual ~Thing();

	// set the velocity
	virtual void setVelocity(Vector3 vel);

	// set the position
	virtual void setPosition(Vector3 pos);

	// set the orientation
	virtual void setOrientation(Real direction);

	// get the orientation
	virtual Real getOrientation();

	// set the scale (width and height)
	virtual void setScale(Real scale);

	// set the width
	virtual void setWidth(Real width);

	// set the height
	virtual void setHeight(Real height);

	// set the material
	virtual void setMaterial(String material);

	// set the colour
	virtual void setColour(ColourValue& colour);
	
	// get the colour
	virtual ColourValue getColour();

	// set the period for flickering (rotating 180 and back)
	virtual void setFlickerPeriod(Real time);

	// get the visual representation (sprite, orientedsprite, or mesh)
	virtual VisRep* getVisRep();

	// set the shape used for collision detection
	virtual inline void setShape(ThingShape);

	// get the shape used for collision detection
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

	// override this for interesting behaviors (not called on clients)
	virtual void collided(Thing* e) { }

	// override this for interesting behaviors (not called on clients)
	virtual void collidedGround() {}

	// returns true if this thing can be damaged
	virtual bool isDamageable() { return false; }

	// damage this thing
	virtual void damage(int amount, int sourceTeamNum) { } 

	// called when this thing has taken too much damage, override this for interesting behaviors
	virtual void die() { } 

	// get rid of this thing - sets alive to false and will cause this thing to be deleted
	virtual void destroy();

	// returns fale if this thing has been destroy()ed and is awaiting deletion.
	virtual bool isAlive();

	// the physics engine calls this when this thing is added to the physics engine
	// and provides a unique identifier
	virtual void placedInPhysics(int uid);

	// conveniance methods for accessing the y axis
	virtual void setPosY(Real y);
	virtual void setVelY(Real y);
	virtual Real getPosY();
	virtual Real getVelY();

	virtual Real getGroundY();
	virtual Real getGroundY(Vector3 pos);

	// play a sound
	virtual void playSound(int id);
	virtual void stopSound();

	// generate a bitstream from this thing
	virtual void generateBitStream(BitStream& bitstream, int pid=ID_UPDATE_THING);

	// interpret a bitstream for this thing
	virtual void interpretBitStream(BitStream& bitstream);

	// get the unique identifier
	virtual int getUID();

	// get a String description
	virtual String getString();

	// set the team number
	virtual void setTeamNum(int teamNum);

	// get the team number
	virtual int getTeamNum();

	// set update type
	virtual void setUpdateType(ThingUpdateType type);

	// get update type
	virtual ThingUpdateType getUpdateType();

	// request an update
	virtual void requestUpdate();

	// returns true when an update has been requested
	virtual bool updateRequested();

	// get the last time a bitstream was generated for this thing
	virtual unsigned long lastUpdateTime();

	// BE VERY CAREFUL WITH THIS
	virtual void _setUID(int uid);

	
private:
	Vector3 mPos;
	Vector3 mVel;
	Real mOrientation;
	int mTeamNum;

	// graphical rendering stuff
	VisRep* mVisRep; // the visual representation
	String mName; // the name of the visual representation

	Real mWidth;
	Real mHeight; 

	// for flickering
	unsigned long mFlickerPeriod;
	unsigned long mLastRotTime;
	bool mLastRotDir;

	ThingShape mShape;

	bool mAlive;

	bool mInPhysics; // wether or not its in the physics engine yet

	bool mInEarshot; // if this thing can be heard by the camera
	bool mPlayingSound; // if this thing is playing a sound
	int mCurrentSound; // the index of the current sound
	int mSoundId; // the id of the sound

	// the update type
	ThingUpdateType mUpdateType;

	// set to true to request an update
	bool mUpdateRequested;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	// a unique identifier
	int mUID;

	unsigned long mLastUpdateTime; // the last time a bitstream was generated for this thing

	virtual void _updateVisibility(); // based on distance from camera

	virtual void _updateAudibility(); // based on distance from camera
};

}
#endif