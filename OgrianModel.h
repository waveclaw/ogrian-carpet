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
OgrianModel.h
Original Author: Mike Prosser
Additional Authors: 

Description: This is a 3D mesh VisRep

/*------------------------------------*/

#ifndef __OgrianModel_H__
#define __OgrianModel_H__

#include <Ogre.h>
#include "Math.h"

#include "OgrianSprite.h"

using namespace Ogre;

namespace Ogrian
{

class Model : public VisRep
{
public:
	Model();
	virtual ~Model();

	// set the mesh 
	virtual void setMesh(String mesh);

	// set the position
	virtual void setPosition(Vector3 pos);

	// set the width
	virtual void setWidth(Real width);
	
	// set the height
	virtual void setHeight(Real height);

	// set the material used by this sprite
	virtual void setMaterial(String material);

	// get the colour
	virtual ColourValue getColour();

	// get the material used by this sprite
	virtual String getMaterial();
	
	// unused
	virtual int addPose(String basename) { return 0; }
	virtual void setPose(int index) {}
	virtual void frame() {}
	virtual void setOrientation(Real orientation) {}
	virtual void setRotation(Real rotation) {}
	virtual void setColour(ColourValue& colour) {}

	// add this model to the renderer
	virtual void addToRenderer();

	// remove this model from the renderer
	virtual void removeFromRenderer();

	// returns true if this model is in the renderer
	virtual inline bool inRenderer();

private:
	bool mInRenderer;
	Real mWidth;
	Real mHeight; 
	Vector3 mPos;
	String mMaterial;
	String mName;
	String mMeshName;

	SceneNode* mNode;
	Entity* mEntity;

	// Incremented count for next name extension
    static unsigned long msNextGeneratedNameExt;

	// the meshes
	static Mesh* msMeshTest;

	// generate the Mesh
	virtual void generateMeshes();
};
}
#endif