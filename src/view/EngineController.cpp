/*
 * EngineController.cpp
 *
 *  Created on: Aug 24, 2008
 *      Author: bey0nd
 */

#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;


#include "EngineController.h"
#include "EngineView.h"
#include "GameStateFactory.h"
#include "StatesLoader.h"

namespace ZGame
{

  EngineController::EngineController() : _root(0),_scnMgr(0),_stillRunning(true),_engineView(0),_curStateInfo(0),_curGameState(0)
  {
    // TODO Auto-generated constructor stub
    _listenerID = "EngineControllerListenerID";
  }

  EngineController::~EngineController()
  {
    // TODO Auto-generated destructor stub
    _gameSInfoMap.clear();
    //delete _root;
  }

  void EngineController::transitionState(const string key)
  {
    try
    {
    loadCurrentState(key);
    realizeCurrentState();
    }catch(std::invalid_argument e)
    {
      std::ostringstream sstream;
      sstream << "Exception: " << e.what() << endl;
      sstream << "Transition state does not exist: "<< key << endl;
      sstream << "Transitioning from state: " << _curStateInfo->key << endl;
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,sstream.str());
    }
  }

  Ogre::Camera* EngineController::createDefaultCamera()
  {
    using namespace Ogre;
    //Camera* cam =_scnMgr->createCamera(_window->getName());
    Camera* cam = _scnMgr->createCamera("ENGINE_VIEW_CAMERA");
    cam->setPosition(0,10,500);
    cam->lookAt(0,0,-300);
    return cam;
  }

  bool EngineController::onInit()
  {
    _root = new Ogre::Root("plugins.cfg");
    if(_root->showConfigDialog())
      {
        _window = _root->initialise(true);
      }
    else
      return false;
    _scnMgr = _root->createSceneManager(Ogre::ST_GENERIC,"ExampleSMInstance");

    loadAssets();
    Ogre::Camera* cam = createDefaultCamera();
    Ogre::Viewport* vp = _window->addViewport(cam);
    vp->setBackgroundColour(Ogre::ColourValue::Green);
    cam->setAspectRatio((Ogre::Real)(vp->getActualWidth())/(Ogre::Real)(vp->getActualHeight()));

    _engineView = new ZGame::EngineView(_window,cam,_scnMgr);

    //load states
    loadStates();

    return true;
  }





  void EngineController::addMouseObserver(ZGame::EVENT::MouseEvtObserver obs)
  {
    _onMouseUpObs.push_back(obs.mue);
    _onMouseDownObs.push_back(obs.mde);
    _onMouseMoveObs.push_back(obs.mme);
  }


  /*
  void EngineController::updateMouseDownObs()
  {
    for(MouseDownObsItr it=_onMouseDownObs.begin();it!=_onMouseDownObs.end();++it)
      {
        (*it)();
      }
  }*/






  void EngineController::injectInputSubject(ZGame::InputController* inControl)
  {
    _inController = inControl;
    ZGame::EVENT::KeyboardEvtObserver keyObs;
    keyObs.kde.bind(&ZGame::EngineController::onKeyDown,this);
    keyObs.kue.bind(&ZGame::EngineController::onKeyUp,this);
    inControl->addKeyListeners(_listenerID,keyObs);
  }


  void EngineController::loadAssets()
  {
    Ogre::ConfigFile cf;
    string resourcePath;
    resourcePath = "";
    cf.load(resourcePath + "resources.cfg");
    //go thourhg all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while(seci.hasMoreElements())
      {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for(i=settings->begin();i!=settings->end();++i)
          {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                resourcePath + archName, typeName, secName);
          }
      }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  }

  void EngineController::run()
  {
    realizeCurrentState();
    while(_stillRunning)
      {
        _root->renderOneFrame();

        _lfcPump.updateOnUpdateObs();
        //updateOnUpdateObs(); //update lifecyle update observers
        //boost::thread::sleep(0);
      }
  }

  void EngineController::onDestroy()
  {
    delete _window;
    //_root->shutdown();
    if(_root)
      delete _root;
  }

  void EngineController::loadStates()
  {
    StatesLoader stLoader;
    GameStateInfo startState;
    stLoader.loadStates(_gameSInfoMap,startState);
    loadStartStateToCurrentState(startState.key);

  }

  bool EngineController::onKeyUp(const OIS::KeyEvent &event)
  {
    cout << "In EngineController::onKeyUp" << endl;
    if(event.key == OIS::KC_ESCAPE)
      _stillRunning = false;
    _keyPump.updateKeyUpObs(event);
    return true;
  }

  bool EngineController::onKeyDown(const OIS::KeyEvent &event)
  {
    cout << "In EngineController::onKeyDown" << endl;
    if(event.key == OIS::KC_LEFT)
      {
        transitionState("GameMainMenuStateKey");
      }
    else if(event.key == OIS::KC_RIGHT)
      {
        transitionState("GameEditStateKey");
      }
    else if(event.key == OIS::KC_DOWN)
      {
        transitionState("ErrorState");
      }
    _keyPump.updateKeyDownObs(event);
    return true;
  }



  void EngineController::loadStartStateToCurrentState(const string curKey)
  {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,
        "In loadStartStateToCurrentstate");
    ZGame::GameStateInfoMapItr it = _gameSInfoMap.find(curKey);
    if (it != _gameSInfoMap.end())
      {
        _curStateInfo = &it->second;
        if (_curStateInfo->stateType == ZGame::GameStateInfo::STATELESS)
          {
            _lfcPump.removeAllObs(); //make sure we clear all LFC observers.
            _keyPump.removeAllObs();
            //removeAllLifeCycleObs(); //make sure we clear any observers.
            _curGameState = 0;
          }
        else
          {
            //do stateful crap here.
          }
      }
    else
      throw(std::invalid_argument("Current State does not exist!"));

  }

  void EngineController::loadCurrentState(const string curKey)
  {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,"In load current state");
    ZGame::GameStateInfoMapItr it = _gameSInfoMap.find(curKey);
    if(it != _gameSInfoMap.end())
      {
          if (_curStateInfo->stateType == ZGame::GameStateInfo::STATELESS)
          {
            if (_curGameState == 0)
              throw(invalid_argument(
                  "Current game state is null when trying to load a new STATELESS current state"));
            _lfcPump.updateOnDestroyObs();
            //updateOnDestroyObs();
            delete _curGameState;
            _lfcPump.removeAllObs(); //remove all LFC observers
            _keyPump.removeAllObs();
            //removeAllLifeCycleObs(); //make sure we clear any observers.
            _curGameState = 0;
            _curStateInfo = &it->second;
          }
        else
          {
            //do stateful crap here.
          }
      }
    else
      throw(std::invalid_argument("Current State does not exist!"));

  }
  void EngineController::unloadCurrentState()
  {

  }
  /**
   * This class realizes the current state. What it does is load the data pointed to by current state meta data.
   */
  void EngineController::realizeCurrentState()
  {
    using namespace ZGame;
    //attach the observers
    Ogre::LogManager* logM = Ogre::LogManager::getSingletonPtr();
    logM->logMessage(Ogre::LML_NORMAL,"In realizeCurrentState");
    logM->logMessage(Ogre::LML_NORMAL,"StateInfo: ");
    logM->logMessage(Ogre::LML_NORMAL,"Key: "+_curStateInfo->key);
    logM->logMessage(Ogre::LML_NORMAL,"Class: "+_curStateInfo->gameStateClass);
    if(_curStateInfo->stateType == GameStateInfo::STATEFUL)
      {
        //add to stateful
      }
    else
      {
        ZGame::EVENT::KeyboardEvtObserver keyObs;
        if(_curGameState != 0)
          throw(invalid_argument("Invalid current game state when realizing new state. Current game state is not null!"));
        _curGameState = ZGame::GameStateFactory::createGameState(_curStateInfo->gameStateClass);
        _curGameState->initialize();

        //Inject LifeCycleSubject
        LifeCycle::LifeCycleSubject lcs; //life cycle subject
        lcs.bind(&LifeCyclePump::addLifeCycleObserver,&_lfcPump);
        //lcs.bind(&EngineController::addLifeCycleObserver,this);
        _curGameState->injectLifeCycleSubject(lcs);

        //Inject Keyboard subject
        EVENT::KeyEvtSubject ks; //keyboard subject
        ks.bind(&KeyboardPump::addKeyboardObserver,&_keyPump);
        _curGameState->injectKeyEvtSubject(ks);

        _lfcPump.updateOnItObs();
      }

  }






}
