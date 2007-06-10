//this file is used to bypass files that prevent compling due to out of date dependancies.
//Ogre::Vector3 defined in <Ogre.h>

#include <Ogre.h>
#include <iostream>

using namespace std;
using namespace Ogre;

namespace Ogrian
{

#define CONR(X) 0
#define Except(X,Y,Z) cout << X << " : " << Y << " : " << Z << endl; 

class CameraThing {
	public:
	Vector3 getDirection();
	Vector3 getVelocity();
	Vector3 getPosition();
};


class Renderer : public Singleton< Renderer > {
	public:
	static Renderer& getSingleton(void);
	CameraThing *getCameraThing();
	private:
	Renderer();
};

}
