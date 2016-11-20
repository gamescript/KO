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

//#include "mastercontrol.h"
#include "kocam.h"

#include "ko.h"

void KO::RegisterObject(Context *context)
{
    context->RegisterFactory<KO>();
}

KO::KO(Context *context):
    Controllable(context)
{
}

void KO::OnNodeSet(Node *node)
{ (void)node;

    Controllable::OnNodeSet(node_);

    node_->SetName("KO");
    node_->SetRotation(Quaternion(160.0f, Vector3::UP));

    model_->SetModel(MC->GetModel("KO"));
    model_->SetMaterial(0, MC->GetMaterial("KO"));

    rightHand_ = node_->GetChild("Sword",true)->CreateComponent<StaticModel>();
    rightHand_->SetCastShadows(true);

    leftHand_ = node_->GetChild("Shield",true)->CreateComponent<StaticModel>();
    leftHand_->SetCastShadows(true);

    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.99f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    collisionShape_->SetCylinder(0.3f, 0.5f);

    EquipLeftHand();
    EquipRightHand();
}

void KO::AddScore(int points)
{
    score_ += points;
}

void KO::PlaySample(Sound* sample)
{
    for (unsigned i{0}; i < sampleSources_.Size(); ++i){
        if (!sampleSources_[i]->IsPlaying()) {

            sampleSources_[i]->Play(sample);
            break;
        }
    }
}

void KO::Update(float timeStep)
{
    //Orientation vectors
    Vector3 camForward{ MC->world.camera->GetNode()->GetDirection() };
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
    //Movement values
    float thrust{ 300.0f };
    float maxSpeed{ 18.0f };

    //Apply movement
    Vector3 force{ Quaternion(camForward.Angle(Vector3::FORWARD), Vector3::DOWN)
                * move_ * thrust * timeStep * (1 + 0.42f * actions_[RUN]) };
    rigidBody_->ApplyForce(force);

    //Update animation
    if (rigidBody_->GetLinearVelocity().Length() > 0.05f) {

        animCtrl_->SetStartBone("Models/Walk.ani", "RootBone");
        animCtrl_->Play("Models/Walk.ani", 0, true, 0.23f);
        animCtrl_->SetSpeed("Models/Walk.ani", rigidBody_->GetLinearVelocity().Length() * 2.63f);

        animCtrl_->SetStartBone("Models/Swing1.ani", "LowerBack");
    }
    else {

        animCtrl_->PlayExclusive("Models/Idle.ani", 0, true, 0.42f);
        animCtrl_->SetStartBone("Models/Swing1.ani", "RootBone");
    }

    AlignWithVelocity(timeStep);

    //Breathe
    float mouthClosed{ MC->Sine(0.1f, 0.0f, 0.9f, MC->Sine(0.23f, -1.0f, 1.0f)) };
    model_->SetMorphWeight(0, mouthClosed);

}

void KO::EquipRightHand()
{
    rightHand_->SetModel(MC->GetModel("Sword"));
    rightHand_->SetMaterial(MC->GetMaterial("Metal"));
}

void KO::EquipLeftHand()
{
    leftHand_->SetModel(MC->GetModel("Shield"));
    leftHand_->SetMaterial(1, MC->GetMaterial("Leather"));
    leftHand_->SetMaterial(0, MC->GetMaterial("Metal"));
}

void KO::HandleAction(int actionId)
{
    switch(actionId) {
    case HACK: Hack(); break;
    default: break;
    }
}

void KO::Hack()
{
    if (animCtrl_->GetTime("Models/Swing1.ani") > 1.0f
     || !animCtrl_->IsPlaying("Models/Swing1.ani"))
    {
        animCtrl_->Play("Models/Swing1.ani", 1, false, 0.23f);
        animCtrl_->SetSpeed("Models/Swing1.ani", 1.42f);
        animCtrl_->SetTime("Models/Swing1.ani", 0.0f);
        animCtrl_->SetAutoFade("Models/Swing1.ani", 0.23f);
    }
}
