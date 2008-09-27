/*
 * GameMainState.cpp
 *
 *  Created on: Sep 18, 2008
 *      Author: bey0nd
 */

#include <cmath>
using namespace std;
#include "GameMainState.h"
#include "GPUEntities.h"
#include "GPUEntsGenProps.h"
#include "GPUEntsView.h"
#include "GPUEntsGen.h"
#include "ZEntity.h"
#include "EngineView.h"

using namespace Ogre;
using namespace ZGame;

GameMainState::GameMainState() : GameState(), _gpuEntsView(0),_cam(0),_dz(1.0f),_forward(false),_backward(false),
_trans(false)
{

}

GameMainState::~GameMainState()
{

}

void GameMainState::initialize()
{
  Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL,"In GameMainState::initialize()");
  //add life cycle stuff here
  LifeCycle::LifeCycleSubjectInjector lfSubInjector;
  lfSubInjector.bind(&GameMainState::injectLifeCycleSubject,this);
  addLifeCycleSubjectInjector(lfSubInjector);
  EVENT::KeyEvtSubjectInjector keySubInjector;
  keySubInjector.bind(&GameMainState::injectKeyEvtSubject,this);
  addKeySubjectInjector(keySubInjector);
}

void GameMainState::injectLifeCycleSubject(ZGame::LifeCycle::LifeCycleSubject &subject)
{
  LifeCycle::LifeCycleObserver lfcObs;
  lfcObs.onInit.bind(&GameMainState::onInit,this);
  lfcObs.onUpdate.bind(&GameMainState::onUpdate,this);
  lfcObs.onDestroy.bind(&GameMainState::onDestroy,this);
  subject(lfcObs);
}

void GameMainState::injectKeyEvtSubject(ZGame::EVENT::KeyEvtSubject &subject)
{
  EVENT::KeyboardEvtObserver keyObs;
  keyObs.kde.bind(&GameMainState::onKeyDown,this);
  keyObs.kue.bind(&GameMainState::onKeyUp,this);
  subject(keyObs);
}

bool GameMainState::onInit()
{
  Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL,"In GameMainState onInit");
  createGPUEntities();
  _cam = EngineView::getSingleton().getCurrentCamera();
  Plane plane(Vector3::UNIT_Y,0);
  //ground testing plane
  MeshManager::getSingleton().createPlane("TempGroundPlane",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,plane,1650,1650,1,1,true,1,1,1,Vector3::UNIT_Z);
    SceneManager* scnMgr = EngineView::getSingleton().getSceneManager();
    string planeName = "TempGroundPlaneEntity";
    Entity* texEnt = scnMgr->createEntity(planeName,"TempGroundPlane");
    string name;
    name = "TempGroundPlaneNode";
    SceneNode* texNode = scnMgr->getRootSceneNode()->createChildSceneNode(name,Vector3(0.0f,-1.0f,0.0f));
    texNode->attachObject(texEnt);
    texNode->setVisible(true,true);
  return true;
}

void GameMainState::move()
{
  if(_forward)
    {
      _cam->moveRelative(Vector3(0.0f,0.0f,-_trans));
    }
  else if(_backward)
    {
      _cam->moveRelative(Vector3(0.0f,0.0f,_trans));
    }
}

bool GameMainState::onUpdate(const Ogre::FrameEvent &evt)
{
  move();
 return true;
}

bool GameMainState::onDestroy()
{
  Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,"In GameMainState::noDestroy()");
  if(_gpuEntsView)
    delete _gpuEntsView;
  return true;
}

bool GameMainState::onKeyDown(const OIS::KeyEvent &evt)
{


  //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL,"In GameMainState::onKeyDown");
  if(evt.key == OIS::KC_W)
      {
        _forward = true;
      }
    else if(evt.key == OIS::KC_S)
      {
        _backward = true;
      }
    else if(evt.key == OIS::KC_UP)
      {
        _cam->moveRelative(Vector3(0.0f,_trans,0.0f));
      }
    else if(evt.key == OIS::KC_DOWN)
      {
        _cam->moveRelative(Vector3(0.0f,-_trans,0.0f));
      }
    else if(evt.key == OIS::KC_A)
      {
        _dz += 0.1;
        _trans = Math::Exp(_dz);
      }
    else if(evt.key == OIS::KC_D)
      {
        _dz -= 0.1;
        _trans = Math::Exp(_dz);

      }
    else if(evt.key == OIS::KC_Q)
      {
        _gpuEntsView->alphaBlend();
      }
  return true;
}

bool GameMainState::onKeyUp(const OIS::KeyEvent &evt)
{
  //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL,"In GameMainState::onKeyUp");
  _forward = false;
  _backward = false;
  return true;
}

void GameMainState::createGPUEntities()
{
  Ogre::LogManager::getSingleton().logMessage(LML_NORMAL,"GameMainState::createGPUEntities");
  ZEntity zent("ZombieEntity","robot.mesh");
  int texW = 512;
  int texH = 512;
  Real minX,maxX,minZ,maxZ; //the space into which we want to distribute the GPU entities
  minX = -100.0f; maxX = 100.0f; minZ = -800.0; maxZ = 800.0f;
  Real entHeight = 2.0f; //1.6 meters
  auto_ptr<GPUEntsGenProps> props(new GPUEntsGenProps(texW,texH,entHeight));
  props->setExtents(minX,minZ,maxX,maxZ);
  GPUEntsGen entsGen(&zent,props);
  entsGen.build();
  _gpuEnts = entsGen.getOutput();
  _gpuEntsView = new GPUEntsView();
  _gpuEntsView->attachGPUEnts(_gpuEnts.get());
  Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL,"GameMainState::createGPUEntities done");
}

