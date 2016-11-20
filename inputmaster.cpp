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

#include "kocam.h"
#include "player.h"

#include "inputmaster.h"

using namespace LucKey;

InputMaster::InputMaster(Context* context):
    Object(context)
{
    keyBindingsMaster_[KEY_UP]     = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_UP)]    = MasterInputAction::UP;
    keyBindingsMaster_[KEY_DOWN]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_DOWN)]  = MasterInputAction::DOWN;
    keyBindingsMaster_[KEY_LEFT]   = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_LEFT)]  = MasterInputAction::LEFT;
    keyBindingsMaster_[KEY_RIGHT]  = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_DPAD_RIGHT)] = MasterInputAction::RIGHT;
    keyBindingsMaster_[KEY_RETURN] = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CROSS)]      = MasterInputAction::CONFIRM;
    keyBindingsMaster_[KEY_ESCAPE] = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_CIRCLE)]     = MasterInputAction::CANCEL;
    keyBindingsMaster_[KEY_P]      = buttonBindingsMaster_[static_cast<int>(SixaxisButton::SB_START)]      = MasterInputAction::PAUSE;
    keyBindingsMaster_[KEY_ESCAPE] = MasterInputAction::MENU;

    keyBindingsPlayer_[1][KEY_W] = keyBindingsPlayer_[1][KEY_UP]     = PlayerInputAction::UP;
    keyBindingsPlayer_[1][KEY_S] = keyBindingsPlayer_[1][KEY_DOWN]   = PlayerInputAction::DOWN;
    keyBindingsPlayer_[1][KEY_A] = keyBindingsPlayer_[1][KEY_LEFT]   = PlayerInputAction::LEFT;
    keyBindingsPlayer_[1][KEY_D] = keyBindingsPlayer_[1][KEY_RIGHT]  = PlayerInputAction::RIGHT;
    keyBindingsPlayer_[1][KEY_C] = keyBindingsPlayer_[1][KEY_LSHIFT] = PlayerInputAction::RUN;
    keyBindingsPlayer_[1][KEY_V] = keyBindingsPlayer_[1][KEY_SPACE]  = PlayerInputAction::HACK;
    keyBindingsPlayer_[1][KEY_B] = keyBindingsPlayer_[1][KEY_ALT]    = PlayerInputAction::CAST;

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputMaster, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputMaster, HandleJoystickButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputMaster, HandleJoystickAxisMove));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(InputMaster, HandleUpdate));
}

void InputMaster::HandleUpdate(StringHash eventType, VariantMap &eventData)
{ (void)eventType; (void)eventData;

    InputActions activeActions{};
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        Vector<PlayerInputAction> emptyActions{};
        activeActions.player_[pId] = emptyActions;
    }

    //Convert key presses to actions
    for (int key : pressedKeys_){
        //Check for master key presses
        if (keyBindingsMaster_.Contains(key)){
            MasterInputAction action{keyBindingsMaster_[key]};
            if (!activeActions.master_.Contains(action))
                activeActions.master_.Push(action);
        }
        //Check for player key presses
        for (Player* p : MC->GetPlayers()){

            int pId{ p->GetPlayerId() };
            if (keyBindingsPlayer_[pId].Contains(key)){
                PlayerInputAction action{keyBindingsPlayer_[pId][key]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
        }
    }
    //Check for joystick button presses
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        for (int button : pressedJoystickButtons_[pId-1])
            if (buttonBindingsPlayer_[pId].Contains(button)){
                PlayerInputAction action{ buttonBindingsPlayer_[pId][button]};
                if (!activeActions.player_[pId].Contains(action))
                    activeActions.player_[pId].Push(action);
            }
    }

    //Handle the registered actions
    HandleActions(activeActions);
}

void InputMaster::HandleActions(const InputActions& actions)
{
    //Handle master actions
    for (MasterInputAction action : actions.master_){
        switch (action){
        case MasterInputAction::UP:                 break;
        case MasterInputAction::DOWN:               break;
        case MasterInputAction::LEFT:               break;
        case MasterInputAction::RIGHT:              break;
        case MasterInputAction::CONFIRM:            break;
        case MasterInputAction::CANCEL:             break;
        case MasterInputAction::PAUSE:              break;
        case MasterInputAction::MENU:               break;
        default: break;
        }
    }

    //Handle player actions
    for (Player* p : MC->GetPlayers()){

        int pId{ p->GetPlayerId() };
        auto playerInputActions = actions.player_[pId];

        Controllable* controlled{ controlledByPlayer_[pId] };
        if (controlled){

            Vector3 stickMove{ Vector3(axesPosition_[pId-1][0], 0.0f, axesPosition_[pId-1][1]) };
            Vector3 stickAim{  Vector3(axesPosition_[pId-1][2], 0.0f, axesPosition_[pId-1][3]) };

            controlled->SetMove(GetMoveFromActions(playerInputActions) + stickMove);
            controlled->SetAim(GetAimFromActions(playerInputActions) + stickAim);

            std::bitset<4>restActions{};
            restActions[0] = playerInputActions->Contains(PlayerInputAction::RUN);
            restActions[1] = playerInputActions->Contains(PlayerInputAction::HACK);

            controlled->SetActions(restActions);
        }
    }
}

void InputMaster::Screenshot()
{
    Image screenshot(context_);
    Graphics* graphics{ GetSubsystem<Graphics>() };
    graphics->TakeScreenShot(screenshot);
    //Here we save in the Data folder with date and time appended
    String fileName{ GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
            Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png" };
    //Log::Write(1, fileName);
    screenshot.SavePNG(fileName);
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
        Screenshot();
    } break;
    default: break;
    }
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int key{ eventData[KeyUp::P_KEY].GetInt() };
    if (pressedKeys_.Contains(key)) pressedKeys_.Remove(key);
}

void InputMaster::HandleJoystickButtonDown(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonDown::P_BUTTON].GetInt()) };
    if (!pressedJoystickButtons_[joystickId].Contains(button)) pressedJoystickButtons_[joystickId].Push(button);
}
void InputMaster::HandleJoystickButtonUp(StringHash eventType, VariantMap &eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickButtonDown::P_JOYSTICKID].GetInt() };
    SixaxisButton button{ static_cast<SixaxisButton>(eventData[JoystickButtonUp::P_BUTTON].GetInt()) };
    if (pressedJoystickButtons_[joystickId].Contains(button)) pressedJoystickButtons_[joystickId].Remove(button);
}

void InputMaster::HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{ (void)eventType;

    int joystickId{ eventData[JoystickAxisMove::P_JOYSTICKID].GetInt() };
    int axis{ eventData[JoystickAxisMove::P_AXIS].GetInt() };
    float position{ eventData[JoystickAxisMove::P_POSITION].GetFloat() };

    axesPosition_[joystickId][axis] = position;
}

Vector3 InputMaster::GetMoveFromActions(Vector<PlayerInputAction>* actions)
{
    return Vector3{Vector3::RIGHT *
                (actions->Contains(PlayerInputAction::RIGHT) -
                 actions->Contains(PlayerInputAction::LEFT))

                + Vector3::FORWARD *
                (actions->Contains(PlayerInputAction::UP) -
                 actions->Contains(PlayerInputAction::DOWN))};
}
Vector3 InputMaster::GetAimFromActions(Vector<PlayerInputAction>* actions)
{
//    return Vector3{ Vector3::RIGHT *
//                (actions->Contains(PlayerInputAction::FIRE_E) -
//                 actions->Contains(PlayerInputAction::FIRE_W))

//                +   Vector3::FORWARD *
//                (actions->Contains(PlayerInputAction::FIRE_N) -
//                 actions->Contains(PlayerInputAction::FIRE_S))
//                + Quaternion(45.0f, Vector3::UP) *
//                   (Vector3::RIGHT *
//                (actions->Contains(PlayerInputAction::FIRE_SE) -
//                 actions->Contains(PlayerInputAction::FIRE_NW))

//                +   Vector3::FORWARD *
//                (actions->Contains(PlayerInputAction::FIRE_NE) -
//                 actions->Contains(PlayerInputAction::FIRE_SW)))};
}

void InputMaster::SetPlayerControl(Player* player, Controllable* controllable)
{
    int playerId{ player->GetPlayerId() };

    if (controlledByPlayer_.Contains(playerId)){
        if (controlledByPlayer_[playerId] == controllable)
            return;
        controlledByPlayer_[playerId]->ClearControl();
    }
    controlledByPlayer_[playerId] = controllable;
}

Player* InputMaster::GetPlayerByControllable(Controllable* controllable)
{
    for (int k : controlledByPlayer_.Keys())
    {
        if (controlledByPlayer_[k] == controllable)
            return MC->GetPlayer(k);
    }
    return nullptr;
}
Controllable* InputMaster::GetControllableByPlayer(int playerId)
{
    return controlledByPlayer_[playerId];
}
