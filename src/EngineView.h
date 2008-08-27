/*
 * EngineView.h
 *
 *  Created on: Aug 26, 2008
 *      Author: bey0nd
 */

#ifndef ENGINEVIEW_H_
#define ENGINEVIEW_H_

#include <Ogre.h>
//#include "EngineController.h"

namespace ZGame
{
class EngineController;
class EngineView
{
public:
  Ogre::RenderWindow* renderWindow;
  Ogre::Camera* camera;

  EngineView(Ogre::RenderWindow *window,Ogre::Camera* cam);
  virtual
  ~EngineView();

  void setCurrentCamera(Ogre::Camera* cam);

protected:
  Ogre::Camera* _curCam;

  friend class ZGame::EngineController;
  //life-cycle methods
  void onInit();
  void onDestroy();
  void onUpdate();

};
}

#endif /* ENGINEVIEW_H_ */
