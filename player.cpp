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

Player::Player(Context *context, MasterControl *masterControl):
    SceneObject(context, masterControl)
{
    rootNode_->SetName("KO");

    model_ = rootNode_->CreateComponent<AnimatedModel>();
    model_->SetModel(masterControl_->resources.models.ko);
    model_->SetMaterial(1, masterControl_->resources.materials.cloth);
    model_->SetMaterial(2, masterControl_->resources.materials.skin);
    model_->SetMaterial(4, masterControl_->resources.materials.hair);
    model_->SetMaterial(0, masterControl_->resources.materials.pants);
    model_->SetMaterial(3, masterControl_->resources.materials.metal);

    model_->SetCastShadows(true);

    animCtrl_ = rootNode_->CreateComponent<AnimationController>();
    animCtrl_->PlayExclusive("Resources/Models/Walk.ani", 0, true);

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();
    rigidBody_->SetFriction(0.0f);
    rigidBody_->SetRestitution(0.0f);
    rigidBody_->SetMass(1.0f);
    rigidBody_->SetLinearFactor(Vector3::ONE - Vector3::UP);
    rigidBody_->SetLinearDamping(0.99f);
    rigidBody_->SetAngularFactor(Vector3::UP);
    rigidBody_->SetAngularDamping(1.0f);
    rigidBody_->SetLinearRestThreshold(0.01f);
    rigidBody_->SetAngularRestThreshold(0.1f);

    StaticModel* rightHand = rootNode_->GetChild("Hand.R",true)->CreateComponent<StaticModel>();
    rightHand->SetModel(masterControl_->resources.models.items.sword);
    rightHand->SetMaterial(masterControl_->resources.materials.metal);
    rightHand->SetCastShadows(true);
    StaticModel* leftHand = rootNode_->GetChild("Hand.L",true)->CreateComponent<StaticModel>();
    leftHand->SetModel(masterControl_->resources.models.items.shield);
    leftHand->SetMaterial(1, masterControl_->resources.materials.leather);
    leftHand->SetMaterial(0, masterControl_->resources.materials.metal);
    leftHand->SetCastShadows(true);

    CollisionShape* collisionShape = rootNode_->CreateComponent<CollisionShape>();
    collisionShape->SetCylinder(0.3f, 0.5f);

    SubscribeToEvent(E_UPDATE, HANDLER(Player, HandleUpdate));
}

void Player::AddScore(int points)
{
    score_ += points;
}

void Player::PlaySample(Sound* sample)
{
    for (int i = 0; i < sampleSources_.Size(); i++){
        if (!sampleSources_[i]->IsPlaying()){
            sampleSources_[i]->Play(sample_);
            break;
        }
    }
}

void Player::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;

    //Take the frame time step, which is stored as a double
    double timeStep = eventData[P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();

    //Orientation vectors
    Vector3 camRight = masterControl_->world.camera->rootNode_->GetRight();
    Vector3 camForward = masterControl_->world.camera->rootNode_->GetDirection();
    camRight = KO::Scale(camRight, Vector3::ONE - Vector3::UP).Normalized();
    camForward = KO::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();
    //Movement values
    Vector3 move = Vector3::ZERO;
    Vector3 moveJoy = Vector3::ZERO;
    Vector3 moveKey = Vector3::ZERO;
    float thrust = 300.0f;
    float maxSpeed = 18.0f;

    //Read input
    JoystickState* joystickState = input->GetJoystickByIndex(0);
    if (joystickState){
    moveJoy = camRight * joystickState->GetAxisPosition(0) +
            -camForward * joystickState->GetAxisPosition(1);
    }
    moveKey = -camRight * input->GetKeyDown(KEY_A) +
               camRight * input->GetKeyDown(KEY_D) +
            camForward * input->GetKeyDown(KEY_W) +
            -camForward * input->GetKeyDown(KEY_S);

    //Pick most significant input
    moveJoy.Length() > moveKey.Length() ? move = moveJoy : move = moveKey;

    //Restrict move vector length
    if (move.Length() > 1.0f) move.Normalize();
    //Deadzone
    else if (move.Length() < 0.01f) move *= 0.0f;

    animCtrl_->SetSpeed("Resources/Models/Walk.ani", rigidBody_->GetLinearVelocity().Length());

    //Apply movement
    Vector3 force = move * thrust * timeStep;
    rigidBody_->ApplyForce(force);

    //Update rotation according to direction of the player's movement.
    if (rigidBody_->GetLinearVelocity().Length() > 0.01f){
        Vector3 velocity = rigidBody_->GetLinearVelocity();
        Quaternion rotation = rootNode_->GetWorldRotation();
        Quaternion aimRotation = rotation;
        aimRotation.FromLookRotation(velocity);
        rootNode_->SetRotation(rotation.Slerp(aimRotation, 7.0f * timeStep * velocity.Length()));
    }
}

void Player::Hack()
{
    
}
