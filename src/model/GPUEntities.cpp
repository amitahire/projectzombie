/*
 * GPUEntities.cpp
 *
 *  Created on: Sep 18, 2008
 *      Author: bey0nd
 */

#include "EngineView.h"
#include "GPUEntities.h"
#include "GPUEntsGenProps.h"

using namespace ZGame;
using namespace Ogre;
GPUEntities::GPUEntities(const string entsName, const string entsData,
    const string dirData, const string imposterTex,
    auto_ptr<GPUEntsGenProps> props) :
      _dirData(dirData),_gpuEntsData(entsData), _imposterTex(imposterTex), _entsName(entsName),
      _props(props)
{
}

GPUEntities::~GPUEntities()
{
  TextureManager* texMgr = TextureManager::getSingletonPtr();
  texMgr->remove(_gpuEntsData.c_str());
  texMgr->remove(_imposterTex.c_str());
}

void
GPUEntities::setEntsData(const string texName)
{
  _gpuEntsData = texName;
}

void
GPUEntities::setImposterTex(const string texName)
{
  _imposterTex = texName;
}

const string
GPUEntities::getName()
{
  return _entsName;
}
