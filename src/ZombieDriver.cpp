//#include <boost/thread.hpp>

#include <iostream>
#include <stdexcept>
using namespace std;
#include <boost/random.hpp>
#include <Ogre.h>
#include "InputController.h"
#include "EngineController.h"
#include "GPUEntsDistributor.h"

/*
 * ZombieDriver.cpp
 *
 *  Created on: Aug 20, 2008
 *      Author: bey0nd
 */
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT 
WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int
main(int argc, char** argv)
#endif
{
  using namespace ZGame;

  ZGame::EngineController engineControl;
  using namespace ZGame;
  try
    {
      if (!engineControl.onInit())
        return 1;
    }
  catch (Ogre::Exception e)
    {
      ostringstream oss;
      oss << "EngineControl onInit failed: " << endl;
      oss << e.what() << endl;
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, oss.str());
      engineControl.onDestroy();
      return 1;
    }
  catch (std::exception e)
    {
      ostringstream oss;
      oss << "EngineContro onInit failed: " << endl;
      oss << e.what() << endl;
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, oss.str());
      engineControl.onDestroy();
      return 1;
    }

  try
    {
      engineControl.run();
    }
  catch (Ogre::Exception e)
    {
      ostringstream oss;
      oss << "Something bad happened, when running the engine." << endl;
      oss << e.what() << endl;
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, oss.str());
      engineControl.onDestroy();
    }
  catch (std::exception e)
    {
      ostringstream oss;
      oss << "Something bad happened, when running the engine." << endl;
      oss << e.what() << endl;
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, oss.str());
      engineControl.onDestroy();
    }
  engineControl.onDestroy();
  cout << "returinging. " << endl;
  return 0;
}
#ifdef __cplusplus
}
#endif

//ZGame::InputController inControl;

/*
 struct MyInputThread
 {
 void operator()()
 {
 inControl.run();
 inControl.onDestroy();
 }
 }inputThread;
 */
