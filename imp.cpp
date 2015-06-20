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
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>

#include "imp.h"

Imp::Imp(Context* context, MasterControl* masterControl, Node* parent, Vector3 pos):
Object(context)
{
    masterControl_ = masterControl;
    randomizer_ = Random(0.5f,0.75f);

    rootNode_ = parent->CreateChild("Imp");
    //rootNode_->SetPosition(Vector3(Random(-10.0f,10.0f), 0.5f, Random(-10.0f,10.0f)));
    rootNode_->Rotate(Quaternion(0.0f,Random(360.0f),0.0f));
    rootNode_->SetScale(Random(0.01f,0.015f));
    spinNode_= rootNode_->CreateChild("ImpModelNode");
    impModel_ = spinNode_->CreateComponent<AnimatedModel>();
    impModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/imp.mdl"));
    impModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/impclothed_lowpol.xml"));
    impModel_->SetCastShadows(true);
    impModel_->SetAnimationEnabled(true);



    smokeAnim_ = masterControl_->cache_->GetResource<Animation>("Resources/Animations/Smoke.ani");
    AnimationState* smokeState = impModel_->AddAnimationState(smokeAnim_);

    if (smokeState)
    {
        smokeState->SetWeight(1.0f);
        smokeState->SetLooped(true);
        smokeState->SetLayer(0);
    }

    SubscribeToEvent(E_UPDATE, HANDLER(Imp, HandleUpdate));
}

void Imp::Start()
{
}
void Imp::Stop()
{
}

void Imp::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    timeStep *= 5;
    rootNode_->Rotate(Quaternion(0.0f,timeStep*randomizer_*10.0f,0.0f));
    rootNode_->Translate(0.0f,0.0f,-randomizer_*timeStep*0.01f);

    //spinNode_->Rotate(Quaternion(0.0f,5.0f*timeStep*randomizer_,0.0f));
    if (impModel_->GetNumAnimationStates())
    {
            impModel_->GetAnimationStates()[0]->AddTime(10.0f*timeStep);
    }
}
