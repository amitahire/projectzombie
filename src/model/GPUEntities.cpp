/*
 * GPUEntities.cpp
 *
 *  Created on: Sep 18, 2008
 *      Author: bey0nd
 */

#include "EngineView.h"
#include "GPUEntities.h"

using namespace ZGame;
using namespace Ogre;
GPUEntities::GPUEntities(Ogre::TexturePtr entsData, Ogre::TexturePtr imposterTex) : _gpuEntsData(entsData),_imposterTex(imposterTex)
{

}

GPUEntities::~GPUEntities()
{
  TextureManager* texMgr = TextureManager::getSingletonPtr();
  texMgr->remove(_gpuEntsData->getName());
  texMgr->remove(_imposterTex->getName());
}

void GPUEntities::setEntsData(const Ogre::TexturePtr &texptr)
{
  _gpuEntsData = texptr;
}

void GPUEntities::setImposterTex(const Ogre::TexturePtr &texptr)
{
  _imposterTex = texptr;
}

