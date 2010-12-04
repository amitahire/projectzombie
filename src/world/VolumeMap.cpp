#pragma warning(disable : 4503)
/*
* VolumeMap.cpp
*
*  Created on: Sep 21, 2010
*      Author: beyzend
*/
//#include <OgreMemoryAllocatorConfig.h>
#include <memory>
#include <limits>
#include <cmath>
#include <iostream>
#include <OgreException.h>
#include <CubicSurfaceExtractor.h>
using std::cout;
using std::endl;
#include "world/VolumeMap.h"
#include "PolyVoxImpl/Utility.h"
#include "world/PerlinNoiseMapGen.h"
#include "world/WorldDefs.h"
#include "world/ZCubicSurfaceExtractor.h"
#include "world/PhysicsManager.h"
#include "EngineView.h"

#include <OgreBulletCollisionsRay.h>


using ZGame::World::VolumeMap;
using PolyVox::ZCubicSurfaceExtractor;
using PolyVox::MaterialDensityPair44;
using PolyVox::Volume;
using PolyVox::Vector3DFloat;
using PolyVox::Vector3DUint16;
using PolyVox::Vector3DInt16;
using std::shared_ptr;
//using PolyVox::SurfaceExtractor;
//using PolyVox::CubicSurfaceExtractorWithNormals;
using PolyVox::CubicSurfaceExtractor;
using PolyVox::SurfaceMesh;
using namespace ZGame::World;
using namespace Ogre;



const Ogre::uint16 VolumeMap::WORKQUEUE_LOAD_REQUEST = 1;


VolumeMap::VolumeMap(size_t volSideLenInPages, bool FORCE_SYNC) :
_volSideLenInPages(volSideLenInPages), _volSizeInBlocks(volSideLenInPages*WORLD_BLOCK_WIDTH), _volHeight(WORLD_HEIGHT),
    _FORCE_SYNC(FORCE_SYNC)
{
    World::PerlinNoiseMapGen::initGradientPoints();
}

VolumeMap::~VolumeMap()
{
    _freeAll();
}

void
    VolumeMap::_freeAll()
{
    //using Ogre::map;
    //using Ogre::list;
    //Iterate through pagesMap and free that.

    //map<Ogre::PageID, VolumePage*>::iterator iter = _pagesMap.begin();
    PagesMap::iterator iter = _pagesMap.begin();
    for (iter = _pagesMap.begin(); iter != _pagesMap.end(); ++iter)
    {
        OGRE_DELETE_T(iter->second, VolumePage, Ogre::MEMCATEGORY_GENERAL);
    }
    _pagesMap.clear();
   
}

bool
    VolumeMap::canHandleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
{
    LoadRequest lreq = any_cast<LoadRequest> (req->getData());
    if (lreq.origin != this)
        return false;
    else
        return RequestHandler::canHandleRequest(req, srcQ);
}
/**
* This method will create a region in local Volume space by transform the volume page id, and Ogre page id.
**/
Ogre::Vector2 
    VolumeMap::_transformToVolumeLocal(Ogre::Vector2 volumeOrigin, Ogre::Vector2 local,
    size_t volSideLenInBlocks)
{
    size_t halfVolSide = volSideLenInBlocks / 2;
    Ogre::Vector2 coords = local;
    Ogre::Vector2 volumeXForm = volumeOrigin * volSideLenInBlocks;
    Ogre::Vector2 localXForm(halfVolSide, halfVolSide);
    coords += (volumeXForm + localXForm);
    return coords;
}

WorkQueue::Response*
    VolumeMap::handleRequest(const WorkQueue::Request* req, const WorkQueue* srcQ)
{
    LoadRequest lreq = any_cast<LoadRequest> (req->getData());
    VolumePage* page = lreq.page;
    WorkQueue::Response* response = 0;
    long volx, voly; long localx, localy;
    _unpackIndex(page->id, &volx, &voly);
    _unpackIndex(lreq.ogreId, &localx, &localy);
    //transform ids
    Ogre::Vector2 volumeOrigin(volx, voly);
    Ogre::Vector2 pageCoords(static_cast<Ogre::Real>(localx) * WORLD_BLOCK_WIDTH, 
        static_cast<Ogre::Real>(localy) * WORLD_BLOCK_WIDTH);
    Ogre::Vector2 localOrigin = _transformToVolumeLocal(volumeOrigin, pageCoords, _volSizeInBlocks);
    Ogre::Vector2 upperCorner = localOrigin + Ogre::Vector2(WORLD_BLOCK_WIDTH, WORLD_BLOCK_WIDTH);
    PolyVox::Region pageRegion(PolyVox::Vector3DInt16(localOrigin.x, 0, localOrigin.y),
        PolyVox::Vector3DInt16(upperCorner.x, WORLD_HEIGHT, upperCorner.y));
    World::TestMapGenerator gen;
    gen.generate(&page->data, pageRegion, localy, -localy);
 
    ZCubicSurfaceExtractor<uint8_t> surfExtractor(&page->data, pageRegion, lreq.surface);
    surfExtractor.execute();
    response = new WorkQueue::Response(req, true, Any());
    return response;
}

bool
    VolumeMap::canHandleResponse(const WorkQueue::Response* res, const WorkQueue* srcQ)
{
    LoadRequest lreq = any_cast<LoadRequest> (res->getRequest()->getData());
    if (lreq.origin != this)
        return false;
    else
        return true;
}
/**
** \note Please review this code to see if they are better ways for error handling (see the allocation code).  
*/
void
    VolumeMap::handleResponse(const WorkQueue::Response* res, const Ogre::WorkQueue* srcQ)
{
    using Ogre::WorkQueue;
    using PolyVox::SurfaceMesh;
    LoadRequest lreq = any_cast<LoadRequest> (res->getRequest()->getData());
    if (res->succeeded())
    {
        //cout << "Response success!" << endl;
        VolumePage* page = lreq.page;
        long x, z;
        _unpackIndex(lreq.ogreId, &x, &z);
        
        PageRegion* region = page->getRegion(lreq.ogreId);
        region->mapView.createRegion(page->worldOrigin, lreq.surface);
        region->mapView.finalizeRegion();
        region->mapView.createPhysicsRegion(_phyMgr);
        OGRE_DELETE_T(lreq.surface, SurfaceMesh, Ogre::MEMCATEGORY_GENERAL);
        _pagesMap[page->id] = page;
    }
    else
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "FAILED TO HANDLE RESPONSE in VOLUME_MAP");
    }

    //OGRE_DELETE_T(res, Response, Ogre::MEMCATEGORY_GENERAL);

}

void
    VolumeMap::_addToList(VolumePage* page)
{
    _freeList.push_back(page);
}

VolumeMap::VolumePage*
    VolumeMap::_getFree()
{
    //cout << "_getFree: _freeList size: " << _freeList.size() << endl;
    if(_freeList.empty())
        OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "No more free volume map pages.", "VolumeMap::_getFree()");
    //return OGRE_NEW_T(VolumePage, Ogre::MEMCATEGORY_GENERAL)(_regionSideLen,
    //_regionsHeight, new PerlinNoiseMapGen());
    VolumeMap::VolumePage* ret = _freeList.front();

    _freeList.pop_front();
    return ret;
    //cout << "_getFree: _freeList size: " << _freeList.size() << endl;
    //return ret;
}
void
    VolumeMap::_initLists()
{

    assert(_volSizeInBlocks >= WORLD_BLOCK_WIDTH && "Invalid num of pages.");
    assert((_volSizeInBlocks % WORLD_BLOCK_WDITH == 0) && "numOfPages per axis MUST be a multiple of WORLD_BLOCK_SIZE");
    size_t powerOfTwoSize = WORLD_BLOCK_WIDTH; //It should be no less than this.
    while(_volSizeInBlocks != powerOfTwoSize && _volSizeInBlocks % powerOfTwoSize != _volSizeInBlocks)
    {
        powerOfTwoSize *= 2;
    }
    _volSideLenInPages = powerOfTwoSize / WORLD_BLOCK_WIDTH;
    Ogre::PageID pageId = _packIndex(0, 0);
    _allocateVolume(pageId, powerOfTwoSize, _volHeight);
    _volSizeInBlocks = powerOfTwoSize;
}

VolumeMap::VolumePage*
    VolumeMap::_allocateVolume(Ogre::PageID pageId, size_t size, size_t height)
{
    PagesMap::iterator findMe = _pagesMap.find(pageId);
    if(findMe == _pagesMap.end())
    {
        VolumePage* page = OGRE_NEW_T(VolumePage, Ogre::MEMCATEGORY_GENERAL)(size, height);
        page->id = pageId;
        long x,y;
        _unpackIndex(pageId, &x, &y);
        Ogre::Vector3 volWorldOrigin(static_cast<Ogre::Real>(x) * size, 0.0f, static_cast<Ogre::Real>(-y) * size);
        _pagesMap[pageId] = page;
        return page;
    }
    else
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to allocate an existing Volume.", "VolumeMap::_allocateVolume");
    return 0;
}

void
    VolumeMap::load(PhysicsManager* phyMgr)
{
    _phyMgr = phyMgr;
    WorkQueue* wq = Root::getSingleton().getWorkQueue();
    _workQueueChannel = wq->getChannel("PROJECT_ZOMBIE/VolumeMap");
    wq->addRequestHandler(_workQueueChannel, this);
    wq->addResponseHandler(_workQueueChannel, this);
    _initLists();

}

/**
* This method will convert Ogre's paging system's ID into Volume ID, which is used to reference Volumes for this VolumeMap.
**/
Ogre::PageID
    VolumeMap::_pageIdToVolumeId(Ogre::PageID pageId, size_t volSideLen)
{
    long x, z;
    _unpackIndex(pageId, &x, &z);
    size_t halfVolSideLen = volSideLen / 2;
    if(x < 0)
        x = Ogre::Math::Floor(static_cast<Ogre::Real>(x) / halfVolSideLen + 0.5f);
    else
        x = x / halfVolSideLen;
    if(z < 0)
        z = Ogre::Math::Floor(static_cast<Ogre::Real>(z) / halfVolSideLen + 0.5f);
    else
        z = z / halfVolSideLen;
    return _packIndex(x, z);
}

void
    VolumeMap::loadPage(Ogre::PageID pageID)
{
    //translate Ogre paging system's page id into page id used in volume.
    Ogre::PageID volumeId = _pageIdToVolumeId(pageID, _volSideLenInPages);
    PagesMap::iterator findMe = _pagesMap.find(volumeId);
    VolumePage* page;
    if(findMe == _pagesMap.end())
    {
        page = _allocateVolume(volumeId, _volSizeInBlocks, _volHeight);
    }
    else
        page = findMe->second;
    LoadRequest req;
    req.origin = this;
    req.page = page;
    req.page->createRegion(pageID);
    req.ogreId = pageID;
    req.surface = OGRE_NEW_T (PolyVox::SurfaceMesh<PolyVox::PositionMaterial>, Ogre::MEMCATEGORY_GENERAL);
    Root::getSingleton().getWorkQueue()->addRequest(_workQueueChannel, WORKQUEUE_LOAD_REQUEST, Any(req), 0, _FORCE_SYNC);

}

void
    VolumeMap::unloadPage(Ogre::PageID pageID)
{
    cout << "Unloading PageID: " << pageID << endl;
    long x, y;
    _unpackIndex(pageID, &x, &y);
    cout << "pageID: " << x << " " << -y << endl;
    Ogre::PageID volumeId = _pageIdToVolumeId(pageID, _volSideLenInPages);

    PagesMap::iterator findMe = _pagesMap.find(volumeId);
    if(findMe == _pagesMap.end())
    {
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Trying to unload an volume page from a non-existing Volume", "VolumeMap::unloadPage");
    }
    VolumePage* page = findMe->second;
    page->getRegion(pageID)->mapView.unloadRegion(_phyMgr);
    page->removeRegion();
    if(page->isEmpty())
    {
        OGRE_DELETE_T(page, VolumePage, Ogre::MEMCATEGORY_GENERAL);
        _pagesMap.erase(volumeId);
    }
}

inline Ogre::uint32
    VolumeMap::_packIndex(long x, long y)
{
    using namespace Ogre;
    using Ogre::uint32;
    using Ogre::int16;
    using Ogre::uint16;
    // Convert to signed 16-bit so sign bit is in bit 15
    int16 xs16 = static_cast<int16> (x);
    int16 ys16 = static_cast<int16> (y);

    // convert to unsigned because we do not want to propagate sign bit to 32-bits
    uint16 x16 = static_cast<uint16> (xs16);
    uint16 y16 = static_cast<uint16> (ys16);

    uint32 key = 0;
    key = (x16 << 16) | y16;

    return key;
}

inline void
    VolumeMap::_unpackIndex(Ogre::PageID pageID, long *x, long *y)
{
    using Ogre::uint16;
    using Ogre::int16;
    uint16 y16 = static_cast<uint16> (pageID & 0xFFFFF);
    uint16 x16 = static_cast<uint16> ((pageID >> 16) & 0xFFFF);

    *x = static_cast<int16> (x16);
    *y = static_cast<int16> (y16);
}

/**
*This method will add a block to the volume. This method assumes that the passed in point is a point on a face of a cube,
*and the ray direction corresponds to the picking direction. (Note: This still works for empty space because the point of
*intersection is still assumed to be on the face of a cube, albiet an empty one.
*
**/
inline void
    VolumeMap::_modifyVolume(Ogre::Vector3 point, size_t blockType, Ogre::Vector3 rayDir,
    VOLUME_MODIFY_MODE mode)
{
    Ogre::PageID pageID;
    //We need to figure out which face. We need to hash the coordinate into a cube first,
    //this will determine the center of a cube. We then iterate through the faces of this cube
    //and tests whether the intersected point is in the face (point in plane). 
    Ogre::Vector3 cubeCenter(Ogre::Math::Floor(point.x + 0.5f),
        Ogre::Math::Floor(point.y + 0.5f),
        Ogre::Math::Floor(point.z + 0.5f));
    Vector3 faces[6];
    faces[0] = Ogre::Vector3(0.5f, 0.0f, 0.0f);
    faces[1] = Ogre::Vector3(-0.5f, 0.0f, 0.0f);
    faces[2] = Ogre::Vector3(0.0f, 0.5f, 0.0f);
    faces[3] = Ogre::Vector3(0.0f, -0.5f, 0.0f);
    faces[4] = Ogre::Vector3(0.0f, 0.0f, 0.5f);
    faces[5] = Ogre::Vector3(0.0f, 0.0f, -0.5f);
    cout << "-------------------" << endl;
    cout << "Cube center: " << cubeCenter << endl;
    cout << "point: " << point << endl;
    for(size_t i = 0; i < 6; ++i)
    {
        Ogre::Vector3 pointOnFace = cubeCenter + faces[i];
        cout << "pointOnFace: " << pointOnFace << endl;
        Ogre::Vector3 canadidateVecInPlane = point - pointOnFace; 
        cout << "canadidate vector is: " << canadidateVecInPlane << endl;
        //If canadidate and faceNormal is orthgonal then onTheFace is in the plane of the face.
        cout << "face normal: " << faces[i] << endl;
        Ogre::Real dotp = canadidateVecInPlane.dotProduct(faces[i]);
        cout << "dotp is " << dotp << endl;
        Ogre::Real epsilon = std::numeric_limits<Ogre::Real>::epsilon();
        if(dotp <= epsilon && dotp >= -epsilon)
        {
            cout << "canadidate vector is in plane" << endl;
            //Depending on the direction of ray we pick the cube next to it.
            if(faces[i].dotProduct(rayDir) < 0.0f)
            {
                if(mode == ADD_BLOCK)
                    cubeCenter += faces[i]*2.0f; //move away from the current cube in the direction of face normal.
                cout << "ray dir and normal not same direction. Cube center: " << cubeCenter << endl;
            }
            else
            {
                if(mode == REMOVE_BLOCK)
                    cubeCenter += faces[i]*2.0f;
                cout << "ray dir and normal not same direction. Cube center: " << cubeCenter << endl;
            }
            break;
        }    
    }
    //first hash point to page id. We are mapping natural numbers into into natural numbers grouped by WORLD_BLOCK_WIDTH
    //and negative of the natural number (excluding zero) to the negative of natural numbers grouped by WORLD_BLOCK_WDITH
    long x, z;
    if(cubeCenter.x < 0.0f)
        x = Ogre::Math::Floor(cubeCenter.x / WORLD_BLOCK_WIDTH);
    else
        x = cubeCenter.x / WORLD_BLOCK_WIDTH;
    if(cubeCenter.z < 0.0f)
        z = Ogre::Math::Floor(cubeCenter.z / WORLD_BLOCK_WIDTH);
    else
        z = cubeCenter.z / WORLD_BLOCK_WIDTH;

    z = -z; //negate z because Ogre paging system's coordinates are flipped.

    pageID = _packIndex(x, z);
    Ogre::PageID volumeId = _pageIdToVolumeId(pageID, _volSideLenInPages);
    long vx, vy;
    _unpackIndex(volumeId, &vx, &vy);
    //Find this page id.
    cout << "Page idx: " << x << " " << z << endl;
    cout << "Volume idx: " << vx << " " << vy << endl;
    cout << "Cube space world coordinate: " << cubeCenter << endl;

    PagesMap::iterator findMe = _pagesMap.find(volumeId);
    if(findMe != _pagesMap.end())
    {
        //Found this page let's add block to it.
        VolumePage* page = findMe->second;

        //Transform into cube space local coordinates.
        Ogre::Vector2 volumeOrigin(vx, vy);
        Ogre::Vector2 pageCoords(cubeCenter.x, -cubeCenter.z);
        Ogre::Vector2 temp = _transformToVolumeLocal(volumeOrigin, pageCoords, _volSizeInBlocks);
        cubeCenter.x = temp.x; cubeCenter.z = temp.y;
        PageRegion* region = page->getRegion(pageID);
        region->mapView.unloadRegion(_phyMgr);

        cout << "cube in local space: " << cubeCenter<< endl;;

        page->data.setVoxelAt(static_cast<uint16_t>(cubeCenter.x), static_cast<uint16_t>(cubeCenter.y), static_cast<uint16_t>(cubeCenter.z), 
            blockType);
        PolyVox::SurfaceMesh<PolyVox::PositionMaterial> surface;
        ZCubicSurfaceExtractor<uint8_t> surfExtractor(&page->data, page->data.getEnclosingRegion(), &surface);
        surfExtractor.execute();
        //regen the surface. Note we shouldn't need to reupdate the center of this page. The assumption here is that it should be set during creation.
        region->mapView.updateRegion(&surface);
        region->mapView.finalizeRegion();
        region->mapView.createPhysicsRegion(_phyMgr);

    }
    //else throw exeception here this should never happen.
    assert(findMe != _pagesMap.end() && "VolumeMap::_addBlockToVolume trying to add to non-existent Volume page.");
}

void
    VolumeMap::addBlock(Ogre::Ray &rayTo, Ogre::Real searchDistance)
{
    Ogre::Vector3 intersectPoint;
    if(_phyMgr->getCollisionPoint(intersectPoint, rayTo, searchDistance))
    {
        _modifyVolume(intersectPoint, 1, rayTo.getDirection(), ADD_BLOCK);
    }
    else //No intersection. Just make searchDistance to be 2 cube units in ray direction so you add a cube in front of you.
    {
        //2.0f may not correspond to 2 cubes..
        _modifyVolume(rayTo.getPoint(2.0f), 1, rayTo.getDirection(), ADD_BLOCK);
    }
}

void
    VolumeMap::removeBlock(Ogre::Ray &rayTo, Ogre::Real searchDistance)
{
    Ogre::Vector3 intersectPoint;
    cout << "VolumeMap::removeBlock" << endl;
    if(_phyMgr->getCollisionPoint(intersectPoint, rayTo, searchDistance))
    {
        _modifyVolume(intersectPoint, 0, rayTo.getDirection(), REMOVE_BLOCK);
    }
    else
    {
    }
}