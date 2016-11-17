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

#include "floatingeye.h"
#include "player.h"

void FloatingEye::RegisterObject(Context *context)
{
    context->RegisterFactory<FloatingEye>();
}

FloatingEye::FloatingEye(Context *context):
    NPC(context),
    smoothTargetPosition_{Vector3::ZERO}
{
}

void FloatingEye::OnNodeSet(Node *node)
{
    NPC::OnNodeSet(node);

    node_->SetName("FloatingEye");

    rigidBody_->SetMass(1.0f);

    modelNode_ = node_->CreateChild("ModelNode");

    ballModel_ = modelNode_->CreateComponent<StaticModel>();
    ballModel_->SetModel(MC->resources.models.enemies.floatingEye);
    ballModel_->SetMaterial(MC->resources.materials.floatingEye);
    ballModel_->SetCastShadows(true);

    corneaModel_ = modelNode_->CreateComponent<StaticModel>();
    corneaModel_->SetModel(MC->resources.models.enemies.cornea);
    corneaModel_->SetMaterial(MC->resources.materials.cornea);
    corneaModel_->SetCastShadows(false);
}

void FloatingEye::Update(float timeStep)
{
    modelNode_->SetPosition(Vector3(MC->Sine(0.9f, -0.023f, 0.023f, variator_*M_PI*2.0f),
                                    MC->Sine(1.0f, -0.05f, 0.075f, -variator_*M_PI*2.0f),
                                    MC->Sine(0.91f, -0.023f, 0.023f, variator_*M_PI)));

    Vector3 targetPosition{ MC->world.player_->GetPosition() };

    Quaternion rotation{ node_->GetWorldRotation() };
    Quaternion aimRotation;
    if (aimRotation.FromLookRotation(targetPosition - node_->GetWorldPosition()))
        node_->SetRotation(rotation.Slerp(aimRotation, 1.5f*timeStep));
}
