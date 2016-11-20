/* KO
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
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

#include "player.h"

#include "mastercontrol.h"
#include "inputmaster.h"

Player::Player(int playerId, Context* context): Object(context),
    playerId_{playerId},
    autoPilot_{playerId_ == 2 && !GetSubsystem<Input>()->GetJoystickByIndex(playerId_-1)},
//    autoPilot_{false},
//    autoPilot_{true},
    alive_{true},
    score_{0},
    flightScore_{0},
    multiplier_{1}
{
//    Node* guiNode{ MC->scene_->CreateChild("GUI3D") };
//    gui3d_ = guiNode->CreateComponent<GUI3D>();
//    gui3d_->Initialize(playerId_);
}

void Player::Die()
{
    alive_ = false;
}
void Player::Respawn()
{
    ResetScore();
    multiplier_ = 1;
    alive_ = true;
}

void Player::SetScore(int points)
{
    score_ = points;
//    gui3d_->SetScore(score_);

}
void Player::ResetScore()
{
    SetScore(0);
}

void Player::EnterLobby()
{

//    for (Pilot* pilot : MC->GetComponentsRecursive<Pilot>()) {
//        if (playerId_ == pilot->GetPlayerId()){
//            GetSubsystem<InputMaster>()->SetPlayerControl(playerId_, pilot);
//            if (!alive_){
//                pilot->Revive();
//            } else pilot->EnterLobbyFromShip();
//        }
//    }

//    gui3d_->EnterLobby();
}
void Player::EnterPlay()
{
//    gui3d_->EnterPlay();
}

void Player::AddScore(int points)
{
    if (!alive_) return;

//    points *= static_cast<int>(pow(2.0, static_cast<double>(multiplier_-1)));
//    SetScore(GetScore()+points);
//    //Check for multiplier increase
//    for (int i{0}; i < 10; ++i){
//        unsigned tenPow{static_cast<unsigned>(pow(10, i))};
//        if (flightScore_ < tenPow && (flightScore_ + points) > tenPow){
//            ++multiplier_;
//            GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->
//                    PlaySample(MC->GetSample("MultiX"), 0.42f);
//            MC->arena_->FlashX(playerId_);
//            break;
//        }
//    }
//    flightScore_ += points;
//    toCount_ += points;
}

Vector3 Player::GetPosition()
{
    return GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_)->GetPosition();
}

Controllable* Player::GetControllable()
{
    Controllable* controllable{ GetSubsystem<InputMaster>()->GetControllableByPlayer(playerId_) };
    if (controllable)

        return controllable;

    else return nullptr;
}
