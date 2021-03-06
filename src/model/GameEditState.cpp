/*
* GameEditState.cpp
*
*  Created on: Aug 28, 2008
*      Author: bey0nd
*/
#include <Ogre.h>
#include <SdkTrays.h>
#include "GameEditState.h"
#include "GameEditView.h"
#include "LifeCycleRegister.h"
#include "KeyEventRegister.h"
#include "MouseEventRegister.h"
#include "EventDelegates.h"
#include "DelegatesUtil.h" //This is for LifeCycle::bindLifeCycleObserver and etc...
#include "GraphicsController.h"
#include "gui/GuiController.h"
#include "world/WorldController.h"
#include "world/WorldConfig.h"
#include "world/CinematicController.h"
#include "gui/EditorScreen.h"
#include "gui/CineView.h"
#include "gui/ToolsetView.h"
#include "ZWorkspaceController.h"
#include "ZWorkspace.h"
#include "gui/GameView.h"

namespace ZGame
{
    GameEditState::GameEditState() :
GameState() , _controlMod(50.0f), _editorScreen(0)

{
    // TODO Auto-generated constructor stub
}

GameEditState::~GameEditState()
{
    // TODO Auto-generated destructor stub
}

void
    GameEditState::regLfcObsForInjection(LifeCycleRegister &lfcReg)
{
    GameState::regLfcObsForInjection(lfcReg);
    //This state
    LifeCycle::LifeCycleObserver lfcObs;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "In GameEditState regLfcObsForInjection");

    try
    {
        try
        {

            LifeCycle::bindAndRegisterLifeCycleObserver<GameEditState>(lfcReg, lfcObs, *this);
        }catch(Ogre::Exception e)
        {
            OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, e.getDescription() + " in GameEditState", "");
        }
    }catch(Ogre::Exception e)
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, e.getDescription(), "GameEditState::regLfcObsForInjection");
    }
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "Out GameEditState regLfcObsForInjection");
}

void
    GameEditState::regKeyObsForInjection(KeyEventRegister &keyReg)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "In GameEditState regKeyObsForInjection");

    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_TRIVIAL, "In GameEditState regKeyObsForInjection");
}

void
    GameEditState::regMouseObsForInjection(MouseEventRegister &mouseReg)
{
    GameState::regMouseObsForInjection(mouseReg);


}

bool
    GameEditState::onInit(ZGame::ZInitPacket *initPacket)
{
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "In GameEditState::onInit");

    //Here we will manually set the various views on editor screen. This is how we're doing this now, maybe in the future
    //we will have a more generalized way of adding views to screens.
    std::auto_ptr<Gui::CineView> cineView(new Gui::CineView(initPacket->workspaceCtrl->getZWorkspace()->getCinematicController()));
    std::auto_ptr<Gui::ToolsetView> toolsetView(new Gui::ToolsetView(initPacket->workspaceCtrl->getZWorkspace()->getToolsetController()));
    std::auto_ptr<Gui::GameView> gameView(new Gui::GameView(initPacket->workspaceCtrl->getZWorkspace()->getGameController()));
    _editorScreen = static_cast<Gui::EditorScreen*>(initPacket->guiCtrl->getScreen("EditorScreen"));
    _editorScreen->setCineView(cineView);
    _editorScreen->setToolsetView(toolsetView);
    _editorScreen->setGameView(gameView);
    _editorScreen->onLoad();
    _editorScreen->show();
    return true;
}

bool
    GameEditState::onUpdate(const Ogre::FrameEvent& evt)
{
    //Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL,"In GameEditState::onUpdate");
    return true;
}

bool
    GameEditState::onDestroy()
{
    using World::VolumeMapPaging;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "In GameEditState::onDestroy");
    return true;
}

bool
    GameEditState::onMouseDown(const OIS::MouseEvent &evt, const OIS::MouseButtonID id)
{
    return true;
}
bool
    GameEditState::onMouseUp(const OIS::MouseEvent &evt, const OIS::MouseButtonID id)
{
    return true;
}
bool
    GameEditState::onMouseMove(const OIS::MouseEvent &evt)
{
    return true;
}
bool
    GameEditState::onKeyDown(const OIS::KeyEvent &evt)
{
    return true;
}

bool
    GameEditState::onKeyUp(const OIS::KeyEvent &evt)
{
    return true;
}

void
    GameEditState::getGameStateBootstrapInfo(GameStateBootstrapInfo &info)
{
    info.initalCameraPos = Ogre::Vector3(32.0f, 250.0f, 32.0f);
    info.requireWorldController = true;
    info.requireControlModule = false;
    info.requireWorkspace = true;
}

void
    GameEditState::onGuiConfiguration(Gui::GuiController* guiCtrl)
{
}

void
    GameEditState::onWorldControllerConfiguration(World::WorldController* worldCtrl)
{
    std::auto_ptr<World::WorldConfig> worldConfig(new World::WorldConfig);
    worldConfig->load("world.cfg");
    worldCtrl->setWorldConfiguration(worldConfig);
}

void
    GameEditState::onCinematicControllerConfiguration(World::CinematicController* cineCtrl)
{
     using World::CinematicController;
    //Create the cameras;
    std::vector<CinematicController::CAM_PAIR> cams;
    cams.push_back(std::make_pair<Ogre::String, CinematicController::CAM_INIT_STATE>("PERSPECTIVE",
        std::make_pair<Ogre::Vector3, Ogre::Quaternion>( Ogre::Vector3(32.0f, 128.0f, 32.0f), Ogre::Quaternion() ) ) );
    //cams.push_back(std::make_pair<Ogre::String, CinematicController::CAM_INIT_STATE>("ORTHOGRAPHIC",
        //std::make_pair<Ogre::Vector3, Ogre::Quaternion>( Ogre::Vector3(32.0f, 250.0f, 32.0f), Ogre::Quaternion() ) ) );

    cineCtrl->loadCameras(cams, 0);
}

}

