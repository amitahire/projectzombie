#ifndef _NETWORK_CLIENT_STATE_H
#define _NETWORK_CLIENT_STATE_H
#include <Ogre.h>

#include "net/NetController.h"
#include "EventDelegates.h"
#include "LifeCycleDelegates.h"
#include "Controller.h"

namespace ZGame
{
    namespace Networking
    {
        /** \brief This class handles call back for receive construction calls.
        *
        *This class implements the ReceiveConstructionInterface (see RakNet) for call back handling of ReceiveConstruction calls.
        *
        */
        class ReplicaConstructor : public ReceiveConstructionInterface
        {
        public:
            /** \brief This is the call back method to do the actual construction. */
            ReplicaReturnResult ReceiveConstruction(RakNet::BitStream* inBitStream, RakNetTime timestamp, NetworkID networkID,
                NetworkIDObject *existingObject, SystemAddress senderId, ReplicaManager *caller);
        };

        /** \brief This class handles call back for send download complete calls.
        *
        *This class implements the SendDownloadCompleteInterface (see RakNet) for call back handling of SendDownloadComplete calls.
        */
        class ReplicaSender : public SendDownloadCompleteInterface
        {
            /** \brief This is the call back method to handle send download complete calls.*/
            ReplicaReturnResult SendDownloadComplete(RakNet::BitStream* outBitStream, RakNetTime currentTime, SystemAddress senderId,
                ReplicaManager* caller);
        };

        /** \brief This class handles call back for recieve download complete calls.
        *
        *This class implements the ReceiveDownloadCompleteInterface (see RakNet) for call back handling of ReceiveDownloadComplete calls
        */
        class ReplicaReceiver : public ReceiveDownloadCompleteInterface
        {
            /** \brief This is the call back method to handle recieve download complete calls.*/
            ReplicaReturnResult ReceiveDownloadComplete(RakNet::BitStream* inBitStream, SystemAddress senderId, ReplicaManager* caller);
        };
        /** \brief This is the controller class which handles networking on the client side.
        *
        *This class subclasses NetController and is the controller which handles aspects of networking on the client side.
        */
        class NetClientController : public NetController, public Controller
        {
        public:
            /** \brief Default constructor. */
            NetClientController();
            virtual ~NetClientController();

            /** \brief implemeting execute from Controller interface */
            virtual int 
                execute(ZGame::Command) {return 0;}

            /** \brief Method to call to connect. */
            bool
                connect();
            /** \brief Method to call to disconnect. */
            bool
                disconnect();
            /** \brief Method to call to shutdown. */
            void 
                shutdown();

            //LifeCycle functions
            virtual bool 
                onInit();
            virtual bool 
                onUpdate(const Ogre::FrameEvent &evt);
            virtual bool 
                onDestroy();

        protected:
            virtual ReceiveConstructionInterface*
                getConstructionCB();
            virtual SendDownloadCompleteInterface*
                getSetDownloadCompleteCB();
            virtual ReceiveDownloadCompleteInterface*
                getReceiveDownloadCompleteCB();


        private:
            bool _isConnected;
            SystemAddress _serverSysAddress;

            /** \brief Our replica constructor call back object. */
            ReplicaConstructor _replicaConstructor;
            /** \brief Our replica sender call back object. */
            ReplicaSender _sendDownloadComplete;
            /** \brief Our replica receiver call back object. */
            ReplicaReceiver _recieveDownloadComplete;

            bool
                initClient();

            void 
                handlePacket();

            //unsigned char getPacketIdentifier(Packet* p);
            void printPacketId(unsigned char id);
        };
    }
}
#endif