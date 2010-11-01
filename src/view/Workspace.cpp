/*
 * Workspace.cpp
 *
 *  Created on: Aug 23, 2010
 *      Author: beyzend
 */
#include "ZWorkspace.h"
#include "EngineView.h"
#include "entities/EntitiesManager.h"
#include "entities/RenderEntitiesManager.h"
#include "ZCL/ZCLController.h"
#include "world/WorldController.h"
#include <sstream>

using namespace ZGame;
using ZGame::ZWorkspace;

size_t ZWorkspace::_ID = 0;

ZWorkspace::ZWorkspace(Entities::EntitiesManager* entMgr, Entities::RenderEntitiesManager* rdrEntMgr, OgreBites::SdkTrayManager* sdkTray, ZCL::ZCLController* zclCtrl,
    World::WorldController* worldCtrl) :
    _entMgr(entMgr), _rdrEntMgr(rdrEntMgr), _tray(sdkTray),
    _zclCtrl(zclCtrl), _worldCtrl(worldCtrl)
{
  _scnMgr = EngineView::getSingleton().getSceneManager();
  _workspaceRoot = _scnMgr->getRootSceneNode()->createChildSceneNode("WorkspaceNode");
}

ZWorkspace::~ZWorkspace()
{
}

void
ZWorkspace::_getKey(Ogre::String &key)
{
  std::ostringstream oss;
  oss << "WRK_ICON" << _ID++;
  key = oss.str();
}

void
ZWorkspace::_removeIcons()
{
  for(size_t i = 0; i < _icons.size(); ++i)
    {
      Ogre::SceneNode* node = _icons[i];
      _scnMgr->destroySceneNode(node);
    }
  _icons.clear();
}

void
ZWorkspace::resetAll()
{
  getEntitiesManager()->resetEntities();
  getRenderEntitiesManager()->resetRenderEntities();
  //remove icons
  _removeIcons();
}



Ogre::SceneNode*
ZWorkspace::createIcon()
{
  using Ogre::String;
  using Ogre::SceneNode;
  //Create an ICON node.
  Ogre::String key;
  _getKey(key);
  Ogre::Entity* iconEnt = _scnMgr->createEntity("ninja.mesh");
  Ogre::SceneNode* node = _workspaceRoot->createChildSceneNode(key);
  node->attachObject(iconEnt);
  node->setScale(10.0f, 10.0f, 10.0f);
  _icons.push_back(node);
  return node;

}

void
ZWorkspace::buildGroups()
{
  _entMgr->buildGroups();
  _zclCtrl->prepare(_entMgr->getEntBuffers(), _worldCtrl->getWorldMap());
}

void
ZWorkspace::updateGroupGoals()
{
  _entMgr->updateGoalsBuffer();
}



