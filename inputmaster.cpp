/* KO
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/IO/FileSystem.h>

#include "inputmaster.h"
#include "dungeon.h"
#include "kocam.h"

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context)
{
    masterControl_ = masterControl;
    input_ = GetSubsystem<Input>();
    //Subscribe mouse down event
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(InputMaster, HandleMouseDown));
    //Subscribe key down event.
    SubscribeToEvent(E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
}

void InputMaster::HandleMouseDown(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonDown;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT){
        //See through cursor
        /*int first = 0;
        if (masterControl_->world.cursor.hitResults[first].node_->GetNameHash() == N_CURSOR) first = 1;
        firstHit_ = SharedPtr<Node>(masterControl_->world.cursor.hitResults[first].node_);
        //Void interaction (create new platform)
        if (firstHit_->GetNameHash() == N_VOID){
            Vector3 cursorPos = masterControl_->world.cursor.sceneCursor->GetPosition();
            new Dungeon(context_, Vector3(round(cursorPos.x_), 0.0f, round(cursorPos.z_)), masterControl_);
        }
        //Dungeon selection
        else if (firstHit_->GetNameHash() == N_TILEPART)
        {
            //Select single platform
            if (!(input_->GetKeyDown(KEY_LSHIFT)||input_->GetKeyDown(KEY_RSHIFT)))
            {
                SharedPtr<Dungeon> platform = masterControl_->platformMap_[firstHit_->GetParent()->GetParent()->GetID()];
                SetSelection(platform);
            }
            //Add platform to selection when either of the shift keys is held down
            else
            {
                SharedPtr<Dungeon> platform = masterControl_->platformMap_[firstHit_->GetParent()->GetParent()->GetID()];
                platform->SetSelected(!platform->IsSelected());
                selectedDungeons_ += platform;
            }
        }*/

    }
    else if (button == MOUSEB_RIGHT){
        //Dungeon move command for each selected platform
    }
}

void InputMaster::SetSelection(SharedPtr<Dungeon> platform)
{
    DeselectAll();
    selectedDungeons_ += platform;
    platform->Select();
}

void InputMaster::HandleMouseUp(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonUp;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT) {}//Deselect when mouse did not move during click on N_VOID
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();

    //Exit when ESC is pressed
    if (key == KEY_ESC) DeselectAll();//masterControl_->Exit();

    //Take screenshot
    else if (key == KEY_9)
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    }
}

void InputMaster::DeselectAll()
{
    for (int i = 0; i < selectedDungeons_.Length(); i++)
    {
        selectedDungeons_[i]->Deselect();
    }
    selectedDungeons_.Clear();
}
