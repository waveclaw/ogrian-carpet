// a list of global constants

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

const Real CAMERA_THING_CULL_DIST = 200;
const Real FOG_DENSITY = 0.008;

const Real SKYPLANE_DISTANCE = 75;
const Real SKYPLANE_BOW = .75;
const Real MAX_THING_RADIUS = 20;

const int FOLIAGE_LINE_MIN = 5;
const int FOLIAGE_LINE_MAX = 50;

const Real HEIGTHMAP_SLOPE_DIFF = 2;
const Real HEIGTHMAP_MIN_HEIGHT = 3;

const Real MANA_DRIFT_SPEED = 1;
const Real FLOAT_GRAV = .3;
const Real FLOAT_FALL_MAX = 2;

}
#endif
