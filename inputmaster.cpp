/* KO
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
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

#include "inputmaster.h"
#include "kocam.h"
#include "player.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context),
    masterControl_{masterControl},
    input_{GetSubsystem<Input>()}
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_UP)]    = MasterInputAction::UP;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_RIGHT)] = MasterInputAction::RIGHT;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_DOWN)]  = MasterInputAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_LEFT)]  = MasterInputAction::LEFT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CROSS)]      = MasterInputAction::CONFIRM;
    keyBindingsMaster_[KEY_ESCAPE]    = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CIRCLE)]     = MasterInputAction::CANCEL;
    keyBindingsMaster_[KEY_PAUSE]  = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_START)]      = MasterInputAction::PAUSE;
    keyBindingsMaster_[KEY_ESCAPE]    = MasterInputAction::MENU;

    keyBindingsPlayer1_[KEY_W] = keyBindingsPlayer1_[KEY_UP]    = PlayerInputAction::UP;
    keyBindingsPlayer1_[KEY_D] = keyBindingsPlayer1_[KEY_RIGHT] = PlayerInputAction::RIGHT;
    keyBindingsPlayer1_[KEY_S] = keyBindingsPlayer1_[KEY_DOWN]  = PlayerInputAction::DOWN;
    keyBindingsPlayer1_[KEY_A] = keyBindingsPlayer1_[KEY_LEFT]  = PlayerInputAction::LEFT;
    keyBindingsPlayer1_[KEY_C] = keyBindingsPlayer1_[KEY_LSHIFT] = PlayerInputAction::RUN;
    keyBindingsPlayer1_[KEY_V] = keyBindingsPlayer1_[KEY_ALT]   = PlayerInputAction::HACK;
    keyBindingsPlayer1_[KEY_B] = keyBindingsPlayer1_[KEY_SPACE] = PlayerInputAction::CAST;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoyButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoyButtonUp));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    InputActions activeActions{};
    //Convert key presses to actions
    for (unsigned k{0}; k < static_cast<int>(pressedKeys_.Size()); k++){
        int key{ pressedKeys_[k] };
        if (keyBindingsMaster_.Contains(key)){
            MasterInputAction action{ keyBindingsMaster_[key] };
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        if (keyBindingsPlayer1_.Contains(key)){
            PlayerInputAction action{ keyBindingsPlayer1_[key] };
            if (!activeActions.player1_.Contains(action))
                activeActions.player1_.Push(action);
        }
    }
    HandleActions(activeActions);
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    if (actions.master_.Contains(MasterInputAction::MENU)) MC->Exit();

//    //Handle player actions
//    if (MC->PlayerIsAlive(BLIP) && MC->PlayerIsHuman(BLIP)){
//        Fish* player1 = MC->GetPlayer(BLIP);
//        Vector2 player1Movement
//                = (Vector2::RIGHT *
//                   (actions.player1_.Contains(PlayerInputAction::RIGHT) -
//                    actions.player1_.Contains(PlayerInputAction::LEFT)))
//                + (Vector2::UP *
//                   (actions.player1_.Contains(PlayerInputAction::UP) -
//                    actions.player1_.Contains(PlayerInputAction::DOWN)));
//        player1Movement = LucKey::Rotate(player1Movement, -MC->world_.camera->GetRotation().EulerAngles().y_);
//        player1->SetRunning(actions.player1_.Contains(PlayerInputAction::RUN));
//        player1->SetMovement(player1Movement);
//        if (actions.player1_.Contains(PlayerInputAction::HACK)) player1->Jump();
//        else player1->JumpRelease();
//    }
//    if (MC->PlayerIsAlive(BLUP) && MC->PlayerIsHuman(BLUP)){
//        Fish* player2 = MC->GetPlayer(BLUP);
//        Vector2 player2Movement
//                = (Vector2::RIGHT *
//                   (actions.player2_.Contains(PlayerInputAction::RIGHT) -
//                    actions.player2_.Contains(PlayerInputAction::LEFT)))
//                + (Vector2::UP *
//                   (actions.player2_.Contains(PlayerInputAction::UP) -
//                    actions.player2_.Contains(PlayerInputAction::DOWN)));
//        player2Movement = LucKey::Rotate(player2Movement, -MC->world_.camera->GetRotation().EulerAngles().y_);
//        player2->SetRunning(actions.player2_.Contains(PlayerInputAction::RUN));
//        player2->SetMovement(player2Movement);
//        if (actions.player2_.Contains(PlayerInputAction::HACK)) player2->Jump();
//        else player2->JumpRelease();
//    }
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    int key{ eventData[KeyDown::P_KEY].GetInt() };
    if (!pressedKeys_.Contains(key)) pressedKeys_.Push(key);

    switch (key){
    case KEY_ESCAPE:{
        MC->Exit();
    } break;
    case KEY_9:{
        Image screenshot(context_);
        Graphics* graphics{ GetSubsystem<Graphics>() };
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName{ GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png" };
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    } break;
    default: break;
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{
    int key{ eventData[KeyUp::P_KEY].GetInt() };
    if (pressedKeys_.Contains(key)) pressedKeys_.Remove(key);
}

void InputMaster::HandleJoyButtonDown(StringHash eventType, VariantMap &eventData)
{
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };
    if (!pressedJoystickButtons_.Contains(button)) pressedJoystickButtons_.Push(button);
}
void InputMaster::HandleJoyButtonUp(StringHash eventType, VariantMap &eventData)
{
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };
    if (pressedJoystickButtons_.Contains(button)) pressedJoystickButtons_.Remove(button);
}

