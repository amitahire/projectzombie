#ifndef _ZGAME_CONTROLLER_H
#define _ZGAME_CONTROLLER_H

#include "Command.h"

namespace ZGame
{
  class Controller
  {
  public:
    ~Controller(){}
    virtual int execute(ZGame::Command) = 0; //pure virtual
  protected:
    Controller(){}
  };
}

#endif