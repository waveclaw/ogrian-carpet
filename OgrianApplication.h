/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    OgrianApplication.h
Description: Main class for Ogrian Carpet
-----------------------------------------------------------------------------
*/

#ifndef __OgrianApplication_H__
#define __OgrianApplication_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgrianConstants.h"
#include "OgrianFrameListener.h"
#include "OgrianHeightMap.h"
#include "OgrianPhysics.h"
#include "OgrianRenderer.h"

using namespace Ogre;

/** Base class which manages the standard startup of an Ogre application.
    Designed to be subclassed for specific examples if required.
*/
class OgrianApplication
{
public:
    /// Start the example
    virtual void go(void)
    {
		OgrianRenderer::getSingleton().go();
    }

};

#endif
