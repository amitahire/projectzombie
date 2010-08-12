#include <iostream>
using std::cout;
using std::endl;
#include <boost/random.hpp>
#include <Ogre.h>
#include "entities/ZEntityBuilder.h"
#include "CommandController.h"
#include "command/CommandList.h"
#include "command/CreateEntCmd.h"
#include "entities/ZEntity.h"
#include "delegates/EntityDelegates.h"
#include "GPUEntsDistributor.h"
#include "world/WorldScale.h"

using namespace ZGame::Entities;

extern ZGame::World::WorldScale WSCALE;


/**
*This method will build specified number of entities. The user has the option to specify whether to use the command and control system to generate a system
*wide command which correspond to the operation of building entities (e.g. generate RenderEntities.)
*
*
*
*/
bool 
EntitiesBuilder::build(EntitiesManager* entMgr, int numOfEnts)
{
    using namespace Ogre;
    using ZGame::COMMAND::CreateEntCmd;
    using ZGame::Entities::ZENTITY_VEC;
    using ZGame::GPUEntsDistributor;
    //Loop through each entities and randomly assign coords.
    ZENTITY_VEC const* ents = entMgr->getEntities();
    ZENTITY_VEC::const_iterator iter;

    Vector3 min = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 max = Vector3(500.0f*WSCALE.unitsPerMeter, 0.0f, 25.0f*WSCALE.unitsPerMeter);
    boost::minstd_rand rng;
    boost::uniform_int<> xDist(static_cast<int>(min.x), static_cast<int>(max.x));
    boost::uniform_int<> zDist(static_cast<int>(min.z), static_cast<int>(max.z));
    GPUEntsDistributor<boost::minstd_rand, boost::uniform_int<> > dist(rng, xDist, zDist);
    Vector3 pos; 
    //Quaternion orient(Ogre::Radian(Ogre::Math::PI), Ogre::Vector3::UNIT_Y); //identity orientation.
    Quaternion orient;
    cout << "Creating " << numOfEnts << " number of entities." << endl;
    for(int i=0; i<numOfEnts; ++i)
    {
        //cout << "Create ent #: " << i << endl;
        ZEntity* ent = entMgr->createZEntity();
        dist.nextPosition(pos);
        //pos.y = Ogre::Math::RangeRandom(250.0f, 450.0f);
        ent->onRead(pos, orient);
    }
    
    CreateEntCmd createCmd;
    cout << "Calling executeCmd: " << endl;
    CommandController::getSingleton().executeCmd(createCmd);
    return true;
}

bool
EntitiesBuilder::unbuild(ZEntity* zEnt)
{
    
    assert(zEnt != 0 && "ZEntity is null!");
    cout << "In PlayerENtity::onDestroyClient()" << endl;
    COMMAND::StringCommand nodeRemove(CommandList::NODEREMOVE);
    return true;
}