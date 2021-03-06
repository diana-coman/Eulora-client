/*
 * pawsstoragewindow.cpp -  Stefano Angeleri <weltall2@gmail.com>
 *
 * Copyright (C) 2010 Atomic Blue (info@planeshift.it, http://www.atomicblue.org)
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation (version 2 of the License)
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#ifndef PAWS_STORAGE_WINDOW
#define PAWS_STORAGE_WINDOW

#include <csutil/csstring.h>

#include "net/cmdbase.h"
#include "paws/pawswidget.h"
#include "paws/pawslistbox.h"
#include "paws/pawsnumberpromptwindow.h"

class pawsListBox;

/** The window that deals with interacting with a NPC storage.
 */
class pawsStorageWindow : public pawsWidget, public psClientNetSubscriber,public iOnNumberEnteredAction
{
public:
    pawsStorageWindow();
    virtual ~pawsStorageWindow();
    bool PostSetup();
    
    void HandleMessage( MsgEntry* me );
    void OnListAction( pawsListBox* listbox, int status );    

    bool OnButtonPressed( int mouseButton, int keyModifier, pawsWidget* widget );
    void Close();

    void DoTrade(int count,const char* itemName,const char* itemID);

    void OnNumberEntered(const char *name,int param,int value);

protected:
    // Handle messages from the server.
    void HandleStorage( const char* data );
    void HandleCategories( const char* data );
    void HandleItems( const char* data );
    void HandleMoney( const char* data );
    void UpdateMoney( const char* moneyName, const char* imageName, int value );

    void SetTradeMode( bool buy );

    /** Handles the command of trading the item by interacting with the player. 
     *  Checks if we buy a single or multiple item and asks in case of uncertainity the
     *  amount of items to buy
     *  @param all indicates that all the item in the current selected entry will be bought/sold
     *  @param single indicates only one item in the current selected entry will be bought/sold, even
     *                if more are available.
     */

    void TradeSelectedItem( bool all = false, bool single = false );
    
    csString GetColumnText(pawsListBoxRow* row, int colNum);
    void SetColumnText(pawsListBoxRow* row, int colNum, const csString & text);


    int merchantID;
    int tradeCommand;
    int selectedItem;   /// index of item, selected in the itemsBox

    pawsListBox* categoryBox;
    pawsListBox* itemsBox;
    pawsTextBox* coppers;
    
    csString currentItem,currentID;
}; 


//--------------------------------------------------------------------------
CREATE_PAWS_FACTORY( pawsStorageWindow );


#endif
