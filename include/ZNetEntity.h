/**
* ZNetEntity.h
*
* This class defines a Networked enabled entity. 
* Subclass Replica and implements all virtual interfaces, in order to implment networking specific elements.
**/
#ifndef _ZGAME_NETENTITY_H
#define _ZGAME_NETENTITY_H

#include <Ogre.h>

#include "RakNetTypes.h"
#include "PacketPriority.h"
#include "ReplicaEnums.h"
#include "Replica.h"
#include "ZEntity.h"

using namespace RakNet;
namespace ZGame
{
    class ZNetEntity : public ZEntity, public Replica
    {
    public:
        /** \brief Constructor for this class which takes in entity and mesh names.
        *
        * The constructor maps directly the entity name and mesh name into Ogre entity and mesh.
        */
        ZNetEntity(const Ogre::String &entName,const Ogre::String &meshName);
        ~ZNetEntity(){}

        /** \brief called to send construction */
        virtual ReplicaReturnResult
            SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags,
            RakNet::BitStream* outBitStream, bool*includeTimestamp);
        /** \brief called to SendConstruction. */
        virtual ReplicaReturnResult
            SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool* includeTimestamp);

        /** \brief called to recieve destruction.*/
        virtual ReplicaReturnResult
            ReceiveDestruction(RakNet::BitStream* inBitStream, SystemAddress systemAddress, RakNetTime timestamp);

        /** \brief called to send scope change. */
        virtual ReplicaReturnResult
            SendScopeChange(bool inScope, RakNet::BitStream* outBitStream, RakNetTime currentTime, SystemAddress sytemAddress,
            bool* includeTimestamp);

        /** \brief called to recieve scope change. */
        virtual ReplicaReturnResult
            ReceiveScopeChange(RakNet::BitStream* inBitStream, SystemAddress sytemAddress, RakNetTime timestamp);

        /** \brief called to serialize this entity. */
        virtual ReplicaReturnResult
            Serialize(bool* sendTimestamp, RakNet::BitStream* outBitStream, RakNetTime lastSendTime,
            PacketPriority* priority, PacketReliability* reliability, RakNetTime currentTIme, SystemAddress systemAddress, unsigned int &flags);

        /** \brief called to deserialize this entity. */
        virtual ReplicaReturnResult
            Deserialize(RakNet::BitStream* inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress);

        /** \brief called to get the sort priority for this entity during serialization. */  
        int
            GetSortPriority(void) const {return 0;}


    protected:
    private:
    };
}

#endif
