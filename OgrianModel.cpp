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
OgrianModel.cpp
Original Author: Mike Prosser
Additional Authors: 

Description: This is a 3D mesh VisRep

/*------------------------------------*/

#include "OgrianModel.h"
#include "OgrianRenderer.h"

using namespace Ogre;

namespace Ogrian
{

unsigned long Model::msNextGeneratedNameExt = 1;
Mesh* Model::msMeshTest = 0;

//----------------------------------------------------------------------------

Model::Model()
{
	mName = String("OgrianModel") << msNextGeneratedNameExt++;

	mWidth = 1;
	mHeight = 1;

	mInRenderer = false;

	generateMeshes();

	mMeshName = "cube.mesh";

}

//----------------------------------------------------------------------------

Model::~Model()
{

	removeFromRenderer();
}

//----------------------------------------------------------------------------

void Model::setMesh(String mesh)
{
	mMeshName = mesh;
	
	if (mInRenderer)
	{
		removeFromRenderer();
		addToRenderer();
	}
}

//----------------------------------------------------------------------------

void Model::generateMeshes()
{
	if (msMeshTest != 0) return;

	String name = "testMesh";
	Plane plane;
	plane.normal = Vector3(0,0,1);
	Real width = 1;
	Real height = 1; 
	int xsegments = 1; 
	int ysegments = 1;
    bool normals = false;
	int numTexCoordSets = 1;
	Real xTile = 1;
	Real yTile = 1; 
	Vector3 upVector(1,1,1);

	HardwareBuffer::Usage vertexBufferUsage = HardwareBuffer::HBU_STATIC;
	HardwareBuffer::Usage indexBufferUsage = HardwareBuffer::HBU_STATIC;
	bool vertexShadowBuffer = false;
	bool indexShadowBuffer = false;

	int i;
	Mesh* pMesh = MeshManager::getSingleton().createManual(name);
	SubMesh *pSub = pMesh->createSubMesh();

	// Set up vertex data
	// Use a single shared buffer
	pMesh->sharedVertexData = new VertexData();
	VertexData* vertexData = pMesh->sharedVertexData;
	// Set up Vertex Declaration
	VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
	size_t currOffset = 0;
	// We always need positions
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// Optional normals
	if(normals)
	{
		vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
		currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	}

    for (i = 0; i < numTexCoordSets; ++i)
    {
		// Assumes 2D texture coords
        vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, i);
		currOffset += VertexElement::getTypeSize(VET_FLOAT2);
    }

	vertexData->vertexCount = (xsegments + 1) * (ysegments + 1);

    // Allocate vertex buffer
	HardwareVertexBufferSharedPtr vbuf = 
		HardwareBufferManager::getSingleton().
		createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount,
		vertexBufferUsage, vertexShadowBuffer);

	// Set up the binding (one source only)
	VertexBufferBinding* binding = vertexData->vertexBufferBinding;
	binding->setBinding(0, vbuf);

	// Work out the transform required
    // Default orientation of plane is normal along +z, distance 0
    Matrix4 xlate, xform, rot;
    Matrix3 rot3;
    xlate = rot = Matrix4::IDENTITY;
    // Determine axes
    Vector3 zAxis, yAxis, xAxis;
    zAxis = plane.normal;
    zAxis.normalise();
    yAxis = upVector;
    yAxis.normalise();
    xAxis = yAxis.crossProduct(zAxis);
    if (xAxis.length() == 0)
    {
        //upVector must be wrong
        Except(Exception::ERR_INVALIDPARAMS, "The upVector you supplied is parallel to the plane normal, so is not valid.",
            "MeshManager::createPlane");
    }

    rot3.FromAxes(xAxis, yAxis, zAxis);
    rot = rot3;

    // Set up standard xform from origin
    xlate.setTrans(plane.normal * -plane.d);

    // concatenate
    xform = xlate * rot;

    // Generate vertex data
	// Lock the whole buffer
	Real* pReal = static_cast<Real*>(
		vbuf->lock(HardwareBuffer::HBL_DISCARD) );
    Real xSpace = width / xsegments;
    Real ySpace = height / ysegments;
    Real halfWidth = width / 2;
    Real halfHeight = height / 2;
    Real xTex = (1.0f * xTile) / xsegments;
    Real yTex = (1.0f * yTile) / ysegments;
    Vector3 vec;
    Vector3 min, max;
    Real maxSquaredLength;
    bool firstTime = true;

    for (int y = 0; y < ysegments + 1; ++y)
    {
        for (int x = 0; x < xsegments + 1; ++x)
        {
            // Work out centered on origin
            vec.x = (x * xSpace) - halfWidth;
            vec.y = (y * ySpace) - halfHeight;
            vec.z = 0.0f;
            // Transform by orientation and distance
            vec = xform * vec;
            // Assign to geometry
            *pReal++ = vec.x;
            *pReal++ = vec.y;
            *pReal++ = vec.z;

            // Build bounds as we go
            if (firstTime)
            {
                min = vec;
                max = vec;
                maxSquaredLength = vec.squaredLength();
                firstTime = false;
            }
            else
            {
                min.makeFloor(vec);
                max.makeCeil(vec);
                maxSquaredLength = max(maxSquaredLength, vec.squaredLength());
            }

            if (normals)
            {
                // Default normal is along unit Z
                vec = Vector3::UNIT_Z;
                // Rotate
                vec = rot * vec;

                *pReal++ = vec.x;
                *pReal++ = vec.y;
                *pReal++ = vec.z;
            }

            for (i = 0; i < numTexCoordSets; ++i)
            {
                *pReal++ = x * xTex;
                *pReal++ = 1 - (y * yTex);
            }


        } // x
    } // y

	// Unlock
	vbuf->unlock();
    // Generate face list
    pSub->useSharedVertices = true;
	//MeshManager::getSingleton().tesselate2DMesh(pSub, xsegments + 1, ysegments + 1, false, indexBufferUsage, indexShadowBuffer);

    //pMesh->_updateBounds();
    pMesh->_setBounds(AxisAlignedBox(min, max));
    pMesh->_setBoundingSphereRadius(Math::Sqrt(maxSquaredLength));

    // load
    pMesh->load();
    pMesh->touch();

}

//----------------------------------------------------------------------------

ColourValue Model::getColour()
{
	return ColourValue::White;
}

//----------------------------------------------------------------------------

// returns true if it's in the renderer
bool Model::inRenderer()
{
	return mInRenderer;
}

//----------------------------------------------------------------------------

// set the position
void Model::setPosition(Vector3 pos)
{
	mPos = pos;

	if (mInRenderer)
		mNode->setPosition(pos);
}

//----------------------------------------------------------------------------

void Model::setWidth(Real width)
{
	mWidth = width;

	if (mInRenderer)
	{
		Real b = mEntity->getMesh()->getBoundingSphereRadius();
		mNode->setScale(mWidth/b,mHeight/b,mWidth/b);
	}
}

//----------------------------------------------------------------------------

void Model::setHeight(Real height)
{
	mHeight = height;

	if (mInRenderer)
	{
		Real b = mEntity->getMesh()->getBoundingSphereRadius();
		mNode->setScale(mWidth/b,mHeight/b,mWidth/b);
	}
}

//----------------------------------------------------------------------------

void Model::setMaterial(String material)
{
	mMaterial = material;

	if (mInRenderer)
		mEntity->setMaterialName(mMaterial);
}

//----------------------------------------------------------------------------

String Model::getMaterial()
{
	return mMaterial;
}

//----------------------------------------------------------------------------

void Model::addToRenderer()
{
	if (mInRenderer) return;
	mInRenderer = true;
		
	// create the entity
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	mEntity = sceneMgr->createEntity(mName,mMeshName);

	// attach the set
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->attachObject(mEntity);

	// apply its properteis
	setMaterial(mMaterial);
	setPosition(mPos);
	setWidth(mWidth);
}

//----------------------------------------------------------------------------

void Model::removeFromRenderer()
{
	if (!mInRenderer) return;

	mNode->detachObject(mName);
	static_cast<SceneNode*>( mNode -> getParent() )->removeAndDestroyChild( mNode->getName() ); 

	// remove it from the scene
	SceneManager* sceneMgr = Renderer::getSingleton().getSceneManager();
	sceneMgr->removeEntity(mName);

	// null the mvars
	mEntity = 0;
	mNode = 0;

	mInRenderer = false;
}

//----------------------------------------------------------------------------

}