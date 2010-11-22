#pragma warning( disable : 4503)
#pragma once

#include <list>
#include <map>
#include <Ogre.h>
#include <MaterialDensityPair.h>

#include <SurfaceMesh.h>
#include <Volume.h>
#include "world/VolumeMapView.h" //We use it directly here instead of relying on the Observer pattern to update views.
#include <OgreGrid2DPageStrategy.h>
#include <OgrePagedWorldSection.h>
#include <OgrePagedWorld.h>
#include <OgrePageManager.h>
#include "world/PerlinNoiseMapGen.h"
#include "world/TestMapGenerator.h"
/*
 * VolumeMap.h
 *
 *
 *
 *  Created on: Sep 21, 2010
 *      Author: beyzend
 */

namespace ZGame
{
  namespace World
  {
      class PhysicsManager;
    /**
     * This class defines a representation or a volume map. A volume map is a container for managing a collection of Volume Pages. Volume Pages are
     * defined as a mapping to Ogre's Paging System's PageIDs to this Volume Map's Volume Pages, which contains PolyVox Volume data. In correspondance with
     * Ogre's paging system, we implement load and unloading of pages. We currently keep a free list of Volume Pages that we use to get a free Volume Page. After
     * a free page is gotten, we then insert it into map structure. This is to facilitate searching for this page based on PageID when Ogre's Paging System
     * asks us to unload a page. That is, we remove this page from the Map and add it to the free list, to be reused. This current system is a first
     * implementation (we did it because it seemed easier.)
     *
     * We directly map our Volume Page to PolyVox's Volume. This may be inefficient (because our pages are small, and we need many pages to represent a
     * large level.) An alternative is to push the paging system to PolyVox volume where we allocate a bigger sized Volume and directly manipulate the volume's
     * data to provide paging. But this means we have to implement our own paging strategy (in Ogre this is called PagingStrategy) in order to figure out
     * when to load/unload PolyVox volumes (of a certain size). So for now this is what we do for we hook direclty into Ogre's paging system. In the future
     * we can optimize.
     */
    class VolumeMap : public Ogre::WorkQueue::RequestHandler, public Ogre::WorkQueue::ResponseHandler
    {
    public:
      VolumeMap(size_t numOfPages = (50*50), bool ASYNC = false);
      virtual
      ~VolumeMap();
      void
      load(PhysicsManager* phyMgr);
      void
      setOrigin(const Ogre::Vector3& origin)
      {
        _origin = origin;
      }

      const Ogre::Vector3&
      getOrigin() const
      {
        return _origin;
      }
      const Ogre::uint32
      getRegionSideLength()
      {
        return _regionSideLen;
      }

      int
      getRegionsHalfWidth()
      {
        return _regionsWidth / 2;
      }

      void
      loadPage(Ogre::PageID pageID);

      void
      unloadPage(Ogre::PageID pageID);

      //Ogre WorkQueue related overrides
      bool
      canHandleRequest(const Ogre::WorkQueue::Request* req, const Ogre::WorkQueue* srcQ);
      Ogre::WorkQueue::Response*
      handleRequest(const Ogre::WorkQueue::Request* req, const Ogre::WorkQueue* srcQ);
      bool
      canHandleResponse(const Ogre::WorkQueue::Response* res, const Ogre::WorkQueue* srcQ);
      void
      handleResponse(const Ogre::WorkQueue::Response* res, const Ogre::WorkQueue* srcQ);

      static const Ogre::uint16 WORKQUEUE_LOAD_REQUEST;

    protected:

      Ogre::uint16 _workQueueChannel;

      /**
       * This class defines a volume page. It has a correspondance to Ogre Paging's Paging system on PageID (uint32).
       */
      class VolumePage
      {
      public:

          VolumePage(size_t pageSize, size_t pageHeight, MapGenerator* generator) :
            data(pageSize, pageHeight, pageSize, 4), _allocated(false), _empty(false),
                gen(generator)
          {
              data.setBorderValue(0);
          }

        VolumePage(size_t pageSize, size_t pageHeight) :
          data(pageSize, pageHeight, pageSize, 4), _allocated(false), _empty(false),
              //gen(new PerlinNoiseMapGen())
              gen(new TestMapGenerator())
        {
            
        }
        ~VolumePage()
        {
            delete gen;
        }
        bool
        isEmpty()
        {
          return _empty;
        }
        void
        setEmpty(bool tf)
        {
          _empty = tf;
        }
        bool
        isAllocated()
        {
          return _allocated;
        }
        void
        setAllocated()
        {
          _allocated = true;
        }
        Ogre::PageID id;
        PolyVox::UInt8Volume data;
        VolumeMapView mapView;
        //PerlinNoiseMapGen gen;
        MapGenerator* gen;
        private:
        bool _allocated;
        bool _empty;
      };

      struct LoadRequest
      {
        VolumePage* page;
        VolumeMap* origin;
        PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* surface;
        friend std::ostream&
        operator<<(std::ostream& o, const LoadRequest& r)
        {
          return o;
        }
      };

    private:

      void
      _addToList(VolumePage* page);
      VolumePage*
      _getFree();
      void
      _initLists();
      void
      _freeAll();
      Ogre::uint32
      _packIndex(long x, long y);
      void
      _unpackIndex(Ogre::PageID pageID, long *x, long *y);
   

    private:
      //PolyVox::Volume<PolyVox::MaterialDensityPair44> _data;
      VolumeMapView _view;
      Ogre::uint32 _regionSideLen;
      Ogre::uint32 _regionsWidth;
      Ogre::uint32 _regionsHeight;
      Ogre::uint32 _regionsDepth;
      Ogre::uint32 _volWidthInRegions;
      Ogre::uint32 _volHeightInRegions;
      Ogre::uint32 _volDepthInRegions;
      Ogre::Vector3 _origin;
      bool _FORCE_SYNC;
      //typedef std::list<VolumePage*>::type FreeList;
      //typedef Ogre::list<VolumePage*>::type FreeList;
      //typedef std::list<VolumePage*> FreeList;
      typedef std::deque<VolumePage*> FreeList;
      //std::list<VolumePage*> _freeList;
      FreeList _freeList;
      //std::map<Ogre::PageID, VolumePage*> _pagesMap;
      //typedef Ogre::map<Ogre::PageID, VolumePage*>::type PagesMap;
      typedef std::map<Ogre::PageID, VolumePage*> PagesMap;
      PagesMap _pagesMap;
      size_t _numOfPages;
      PhysicsManager* _phyMgr;

    };
  }
}
