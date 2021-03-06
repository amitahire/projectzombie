#pragma once
#include <list>
#include <vector>
#include <OgreFrameListener.h>
#include <Ogre.h>
#include <OIS/OIS.h>
/**
* \file Please deprecate this ASAP.
*
*
*/

namespace ZGame
{
  
  //class OgreConsole: public Ogre::Singleton<OgreConsole>, Ogre::FrameListener, Ogre::LogListener
  class OgreConsole: public Ogre::FrameListener, Ogre::LogListener
  {

  public:
    OgreConsole();
    ~OgreConsole();

    void   init(Ogre::Root *root);
    void   shutdown();

    void   setVisible(bool visible);
    bool   isVisible(){return visible;}

    void   print(const Ogre::String &text);

    virtual bool frameStarted(const Ogre::FrameEvent &evt);
    virtual bool frameEnded(const Ogre::FrameEvent &evt);

    void onKeyPressed(const OIS::KeyEvent &arg);

    void addCommand(const Ogre::String &command, void (*)(const Ogre::StringVector&));
    void removeCommand(const Ogre::String &command);

    //log
    void messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName ) {print(logName+": "+message);}
  private:
    bool            visible;
    bool            initialized;
    Ogre::Root         *root;
    Ogre::SceneManager   *scene;
    Ogre::Rectangle2D   *rect;
    Ogre::SceneNode      *node;
    Ogre::OverlayElement *textbox;
    Ogre::Overlay      *overlay;

    float            height;
    bool            update_overlay;
    size_t               start_line;
    std::list<Ogre::String>      lines;
    Ogre::String            prompt;
    std::map<Ogre::String,void (*)(const Ogre::StringVector&)>  commands;
  };
}