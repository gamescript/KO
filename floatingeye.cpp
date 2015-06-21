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

#include <Urho3D/Core/CoreEvents.h>

#include "floatingeye.h"
#include "player.h"

FloatingEye::FloatingEye(Context *context, MasterControl *masterControl, Vector3 pos):
    NPC(context, masterControl, pos),
    smoothTargetPosition_{Vector3::ZERO}
{
    rootNode_->SetName("FloatingEye");

    rigidBody_->SetMass(1.0f);

    modelNode_ = rootNode_->CreateChild("ModelNode");

    ballModel_ = modelNode_->CreateComponent<StaticModel>();
    ballModel_->SetModel(masterControl_->resources.models.enemies.floatingEye);
    ballModel_->SetMaterial(masterControl_->resources.materials.floatingEye);
    ballModel_->SetCastShadows(true);

    corneaModel_ = modelNode_->CreateComponent<StaticModel>();
    corneaModel_->SetModel(masterControl_->resources.models.enemies.cornea);
    corneaModel_->SetMaterial(masterControl_->resources.materials.cornea);
    corneaModel_->SetCastShadows(false);

    SubscribeToEvent(E_UPDATE, HANDLER(FloatingEye, HandleUpdate));
}

void FloatingEye::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();
    modelNode_->SetPosition(Vector3(masterControl_->Sine(0.9f, -0.023f, 0.023f, randomizer_*M_TAU),
                                    masterControl_->Sine(1.0f, -0.05f, 0.075f, -randomizer_*M_TAU),
                                    masterControl_->Sine(0.91f, -0.023f, 0.023f, randomizer_*M_TAU)));

    Vector3 targetPosition = masterControl_->world.player_->rootNode_->GetWorldPosition();

    Quaternion rotation = rootNode_->GetWorldRotation();
    Quaternion aimRotation = rotation;
    aimRotation.FromLookRotation(targetPosition - rootNode_->GetWorldPosition());
    rootNode_->SetRotation(rotation.Slerp(aimRotation, 1.5f*timeStep));
}
