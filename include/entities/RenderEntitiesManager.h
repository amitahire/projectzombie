#pragma once
/**
Permission is hereby granted by Fdastero LLC, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/
/**
*author: beyzend 
*email: llwijk@gmail.com
**/
#include "ZPrerequisites.h"

#include "entities/EntitiesDefs.h"
#include "delegates/EntityDelegates.h"

using ZGame::Entities::ZENTITY_VEC;
namespace ZGame
{

    namespace Entities
    {
        /**
        *This class is a Manager for managing render entities components. 
        */
        class RenderEntitiesManager
        {
        public:
            RenderEntitiesManager();
            virtual 
                ~RenderEntitiesManager();
            RenderEntitiesManager * 
                getManager()
            {
                return this;
            }
            bool
                onInit(ZInitPacket *packet);
            void
                resetRenderEntities();
            RenderEntityComp*
                createComponent(const ZEntityResource* res);
        protected:

        private:
            void _removeChildObjects(Ogre::SceneNode* node);

        private:
            Ogre::SceneManager* _scnMgr;
            Ogre::SceneNode* _instancesRoot;

            typedef Ogre::vector<RenderEntityComp*>::type RdrEntsComps;
            RdrEntsComps _renderComps;

        };
    }
}
