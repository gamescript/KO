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

#include "mastercontrol.h"
#include "player.h"
#include "kocam.h"


void Player::RegisterObject(Context *context)
{
    context->RegisterFactory<Player>();
}

Player::Player(Context *context):
    SceneObject(context)
{
}

void Player::OnNodeSet(Node *node)
{
    node_->SetName("KO");
    node_->SetRotation(Quaternion(160.0f, Vector3::UP));

    model_ = node_->CreateComponent<AnimatedModel>();
    model_->SetModel(MC->resources.models.ko);
    model_->SetMaterial(0, MC->resources.materials.ko);
    model_->SetCastShadows(true);

    rightHand_ = node_->GetChild("Sword",true)->CreateComponent<StaticModel>();
    rightHand_->SetCastShadows(true);

    leftHand_ = node_->GetChild("Shield",true)->CreateComponent<StaticModel>();
    leftHand_->SetCastShadows(true);

    animCtrl_ = node_->CreateComponent<AnimationController>();

    rigidBody_ = node_->CreateComponent<RigidBody>();
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.99f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    CollisionShape* collisionShape{ node_->CreateComponent<CollisionShape>() };
    collisionShape->SetCylinder(0.3f, 0.5f);

    EquipLeftHand();
    EquipRightHand();
}

void Player::AddScore(int points)
{
    score_ += points;
}

void Player::PlaySample(Sound* sample)
{
    for (unsigned i{0}; i < sampleSources_.Size(); ++i){
        if (!sampleSources_[i]->IsPlaying()) {

            sampleSources_[i]->Play(sample_);
            break;
        }
    }
}

void Player::Update(float timeStep)
{
    Input* input{ GetSubsystem<Input>() };

    //Orientation vectors
    Vector3 camRight{ MC->world.camera->rootNode_->GetRight() };
    Vector3 camForward{ MC->world.camera->rootNode_->GetDirection() };
    camRight = LucKey::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
    //Movement values
    Vector3 move{ Vector3::ZERO };
    Vector3 moveJoy{ Vector3::ZERO };
    Vector3 moveKey{ Vector3::ZERO };
    float thrust{ 300.0f };
    float maxSpeed{ 18.0f };

    //Read input
    JoystickState* joystickState{ input->GetJoystickByIndex(0) };
    if (joystickState){
        moveJoy = camRight * joystickState->GetAxisPosition(0) +
               -camForward * joystickState->GetAxisPosition(1);
    }

    moveKey = -camRight * input->GetKeyDown(KEY_A) +
               camRight * input->GetKeyDown(KEY_D) +
               camForward * input->GetKeyDown(KEY_W) +
              -camForward * input->GetKeyDown(KEY_S);

    //Pick most significant input
    moveJoy.Length() > moveKey.Length() ? move = moveJoy
                                        : move = moveKey;

    //Restrict move vector length
    if (move.Length() > 1.0f) move.Normalize();
    //Deadzone
    else if (move.Length() < 0.01f) move *= 0.0f;

    //Update animation
    if (rigidBody_->GetLinearVelocity().Length() > 0.05f){
        animCtrl_->SetStartBone("Models/Swing1.ani", "UpperBack");
        animCtrl_->PlayExclusive("Models/Swing1.ani", 0, true, 0.23f);
        animCtrl_->SetSpeed("Models/Swing1.ani", rigidBody_->GetLinearVelocity().Length()*2.63f);
    }
    else {
        animCtrl_->PlayExclusive("Models/Idle.ani", 0, true, 0.23f);
    }

    //Apply movement
    Vector3 force{ move * thrust * timeStep * (1+0.42f*input->GetKeyDown(KEY_SHIFT)) };
    rigidBody_->ApplyForce(force);

    //Update rotation according to direction of the player's movement.
    if (rigidBody_->GetLinearVelocity().Length() > 0.01f){
        Vector3 velocity = rigidBody_->GetLinearVelocity();
        Quaternion rotation = node_->GetWorldRotation();
        Quaternion aimRotation = rotation;
        aimRotation.FromLookRotation(velocity);
        node_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));
    }

    //Breathe
    float mouthClosed{ MC->Sine(0.1f, 0.0f, 0.9f, MC->Sine(0.23f, -1.0f, 1.0f)) };
    model_->SetMorphWeight(0, mouthClosed);

}

void Player::EquipRightHand()
{
    rightHand_->SetModel(MC->resources.models.items.sword);
    rightHand_->SetMaterial(MC->resources.materials.metal);
}

void Player::EquipLeftHand()
{
    leftHand_->SetModel(MC->resources.models.items.shield);
    leftHand_->SetMaterial(1, MC->resources.materials.leather);
    leftHand_->SetMaterial(0, MC->resources.materials.metal);
}

void Player::Hack()
{
    
}
