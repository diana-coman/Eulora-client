/** psinventorycache.cpp
 *
 * Copyright (C) 2006 Atomic Blue (info@planeshift.it, http://www.atomicblue.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation (version 2 of the License)
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Client's inventory cache.
 */

#include <psconfig.h>
#include "paws/pawsmanager.h"
#include "net/messages.h"
#include "psinventorycache.h"
#include "globals.h"

int psInventoryCache::CachedItemDescription::CompareSlot(CachedItemDescription* const &first, CachedItemDescription* const &second)
{
    if (first->slot < second->slot) return -1;
    else if (second->slot < first->slot) return 1;
    else return 0;
}



psInventoryCache::psInventoryCache () : version(0)
{
    EmptyInventory();
}

psInventoryCache::~psInventoryCache ()
{
    csHash<CachedItemDescription*>::GlobalIterator loop = itemhash.GetIterator();

    while (loop.HasNext())
    {
        CachedItemDescription* next = loop.Next();
        delete next;
    }

    itemhash.Empty();
    itemBySlot.Empty();
}

bool psInventoryCache::GetInventory()
{
    // testing if need to request full inventory or
    // just to refresh local cache.
    if (cacheStatus == INVALID)
    {
        // full list request
        psGUIInventoryMessage request;
        request.SendMessage();
    }
    else
    {
        // updates request
        psGUIInventoryMessage request(psGUIInventoryMessage::UPDATE_REQUEST);
        request.SendMessage();
    }

    return true;
}

void psInventoryCache::EmptyInventory(void)
{
    csHash<CachedItemDescription*>::GlobalIterator loop = itemhash.GetIterator();

    while (loop.HasNext())
    {
        CachedItemDescription* next = loop.Next();
        delete next;
    }

    itemhash.Empty();
    itemBySlot.Empty();

    PawsManager::GetSingleton().Publish("sigClearInventorySlots");
}

bool psInventoryCache::EmptyInventoryItem(int slot, int /*container*/)
{
    CachedItemDescription* id = itemhash.Get(slot,NULL);
    itemBySlot.Delete(id);
    delete id;
    itemhash.DeleteAll(slot);

    return true;
}

bool psInventoryCache::SetInventoryItem(int slot,
                                        int containerID,
                                        csString name,
                                        csString meshName,
                                        csString materialName,
                                        float weight,
                                        float size,
                                        int stackCount,
                                        csString iconImage,
                                        int purifyStatus)
{
    CachedItemDescription* id = itemhash.Get(slot, NULL);
    if (!id)
    {
        id = new CachedItemDescription;
        id->slot = slot;
        itemhash.PutUnique(slot, id);
        itemBySlot.InsertSorted(id, CachedItemDescription::CompareSlot);
    }

    id->containerID = containerID;
    id->name = name;
    id->meshName = meshName;
    id->materialName = materialName;
    id->weight = weight;
    id->size = size;
    id->stackCount = stackCount;
    id->iconImage = iconImage;
    id->purifyStatus = purifyStatus;

    // Don't publish slot updates for items in containers.
    // That is handled by pawsContainerDescWindow.
    if (id->stackCount>0 && id->iconImage.Length() != 0 && slot < 100)
    {
        csString sigData, data;
        sigData.Format("invslot_%d", slot);

        data.Format( "%s %d %d %s %s %s", id->iconImage.GetData(),
                     id->stackCount,
                     id->purifyStatus,
                     id->meshName.GetData(),
                     id->materialName.GetData(),
                     id->name.GetData());

        PawsManager::GetSingleton().Publish(sigData, data);
    }

    return true;
}

psInventoryCache::CachedItemDescription* psInventoryCache::GetInventoryItem(int slot)
{
    return itemhash.Get(slot,NULL);
}

