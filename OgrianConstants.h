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
OgrianConstants.h
Original Author: Mike Prosser
Additional Authors: 

Description: This holds all the global constants

/*------------------------------------*/

#ifndef __OgrianConstants_H__
#define __OgrianConstants_H__

using namespace Ogre;

namespace Ogrian
{

const Real CAMERA_MOVE_SPEED = 15;
const Real CAMERA_FOV = 80;
const Real CAMERA_NEAR_CLIP = 1;
const Real CAMERA_HEIGHT = 3;
const Real CAMERA_GRAV = 3;
const Real CAMERA_FALL_MAX = 10;

const Real WIZARD_SCALE = 3;

const Real THING_CULL_DIST = 300;
const Real FOG_DENSITY = 0.008;

const Real SKYPLANE_DISTANCE = 75;
const Real SKYPLANE_BOW = .75;

const Real FOLIAGE_NUM = 500;
const Real FOLIAGE_SCALE = 10;
const Real FOLIAGE_SCALE_VAR = 3;
const int FOLIAGE_LINE_MIN = 5;
const int FOLIAGE_LINE_MAX = 50;

const Real HEIGTHMAP_SLOPE_DIFF = 2;
const Real HEIGTHMAP_MIN_HEIGHT = 3;
const int HEIGHTMAP_SMOOTHING = 4;

const Real MANA_DRIFT_SPEED = 1;
const Real FLOAT_GRAV = .3;
const Real FLOAT_FALL_MAX = 2;

const int PHYSICS_GRID_SIZE = 100;

const int MUSIC_VOLUME = 64;
const int THING_EARSHOT = 300;

const Real KEY_DELAY = .4;

const int PORT = 6006;

const int MAX_PLAYERS = 8;
const int STRING_MAX_LENGTH = 24;

const unsigned long THING_UPDATE_PERIOD = 100;
const unsigned long THING_STOP_DELAY = 300;

const Real FIREBALL_FALL_RATE = 3;
const Real FIREBALL_SPEED = 40;
const Real FIREBALL_SCALE = 2;
const unsigned long FIREBALL_ROT_PERIOD = 200;

}
#endif
