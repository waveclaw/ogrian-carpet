// a list of global constants

#ifndef __OgrianConstants_H__
#define __OgrianConstants_H__

using namespace Ogre;

namespace Ogrian
{

const float CAMERA_MOVE_SPEED = 5;
const float CAMERA_FOV = 80;
const float CAMERA_NEAR_CLIP = 1;
const float CAMERA_HEIGHT = 3;
const float FOG_DENSITY = 0.008;
const float SKYPLANE_DISTANCE = 75;
const float SKYPLANE_BOW = .75;
const float MAX_THING_RADIUS = 20;

const int FOLIAGE_LINE_MIN = 5;
const int FOLIAGE_LINE_MAX = 50;

const Real HEIGTHMAP_SLOPE_DIFF = 2;
const Real HEIGTHMAP_MIN_HEIGHT = 3;

const Real MANA_DRIFT_SPEED = 1;
const Real FLOAT_GRAV = .3;
const Real FLOAT_FALL_MAX = 2;

}
#endif
