/*
* LifeCycleDelegatesUtil.h
*
*  Created on: Sep 28, 2008
*      Author: bey0nd
*/

#ifndef LIFECYCLEDELEGATESUTIL_H_
#define LIFECYCLEDELEGATESUTIL_H_
#include "LifeCycleDelegates.h"
#include "EventDelegates.h"
#include "delegates/EntityDelegates.h"
namespace ZGame
{
    namespace LifeCycle
    {
        static const unsigned int LFC_ON_DESTROY = 0x01;
        static const unsigned int LFC_ON_INIT = 0x02;
        static const unsigned int LFC_ON_UPDATE = 0x04;
        
        static void clearLfcObs(LifeCycleObserver& lfcObs)
        {
            lfcObs.onDestroy.clear();
            lfcObs.onInit.clear();
            lfcObs.onUpdate.clear();
        }
        
        template<typename T>
        static void bindLifeCycleObserver(LifeCycleObserver &lfcObs,
            T& binder, unsigned eventMask = LFC_ON_DESTROY | LFC_ON_INIT | LFC_ON_UPDATE)
        {
            eventMask & LifeCycle::LFC_ON_INIT ? lfcObs.onInit.bind(&binder,&T::onInit) : lfcObs.onInit.clear(); 
            eventMask & LFC_ON_UPDATE ? lfcObs.onUpdate.bind(&binder,&T::onUpdate) : lfcObs.onUpdate.clear();
            eventMask & LFC_ON_DESTROY ? lfcObs.onDestroy.bind(&binder,&T::onDestroy) : lfcObs.onDestroy.clear();
        }
    }
    namespace Entities
    {
      /**
        template<typename T>
        static void bindEntityLifeCycleObserver(EntityLifeCycleObserver &lfcObs,
            T& binder)
        {
            lfcObs.onInit.bind(&binder,&T::onInit);
            lfcObs.onUpdate.bind(&binder,&T::onUpdate);
            lfcObs.onDestroy.bind(&binder,&T::onDestroy)
        }
      **/
        template<typename T>
        static void bindEntityAspects(EntityAspects &aspects, T& binder, bool isServer)
        {
            if(isServer)
                aspects.onDestroy.bind(&binder,&T::onDestroyServer);
            else
                aspects.onDestroy.bind(&binder,&T::onDestroyClient);
            aspects.onSendConstruction.bind(&binder,&T::onSendConstruction);
            aspects.onRead.bind(&binder,&T::onRead);
            aspects.onWrite.bind(&binder,&T::onWrite);
        }
    }
    namespace EVENT
    {
        template<typename T>
        static void bindKeyObserver(KeyboardEvtObserver &keyObs,
            T& binder)
        {
            keyObs.kde.bind(&binder,&T::onKeyDown);
            keyObs.kue.bind(&binder,&T::onKeyUp);
        }
        static void clearKeyObs(EVENT::KeyboardEvtObserver &keyObs)
        {
            keyObs.kde.clear();
            keyObs.kue.clear();
        }
        template<typename T>
        static void bindMouseObserver(MouseEvtObserver &mouseObs,
            T& binder)
        {
            mouseObs.mde.bind(&binder,&T::onMouseDown);
            mouseObs.mme.bind(&binder,&T::onMouseMove);
            mouseObs.mue.bind(&binder,&T::onMouseUp);
        }
        static void clearMouseObs(EVENT::MouseEvtObserver &mouseObs)
        {
            mouseObs.mde.clear();
            mouseObs.mme.clear();
            mouseObs.mue.clear();
        }


    }
}

#endif /* LIFECYCLEDELEGATESUTIL_H_ */
