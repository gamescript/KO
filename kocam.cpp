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

#include "player.h"
#include "kocam.h"

using namespace LucKey;

KOCam::KOCam(Context *context, MasterControl *masterControl):
    Object(context),
    masterControl_{masterControl},
    smoothTargetPosition_{Vector3::ZERO},
    smoothTargetVelocity_{Vector3::ZERO},
    yaw_{0.0f},
    pitch_{60.0f},
    velocity_{Vector3::ZERO},
    maxVelocity_{23.0f},
    acceleration_{7.0f},
    rotationSpeed_{0.0f},
    maxRotationSpeed_{512.0f},
    angularAcceleration_{128.0f},
    velocityMultiplier_{1.0f}
{
    float viewRange{128.0f};

    //Create the camera. Limit far clip distance to match the fog
    rootNode_ = masterControl_->world.scene->CreateChild("Camera");
    rootNode_->SetPosition(masterControl_->world.player_->GetPosition() + Vector3(0.23f, 8.8f, -4.2f));
    rootNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
    camera_ = rootNode_->CreateComponent<Camera>();
    camera_->SetFarClip(viewRange);
    camera_->SetNearClip(0.023f);

    Zone* zone = rootNode_->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(Vector3(-100.0f, -50.0f, -100.0f), Vector3(100.0f, 50.0f, 100.0f)));
    zone->SetFogColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
    zone->SetFogStart(10.0f);
    zone->SetFogEnd(viewRange-5.0f);

    SetupViewport();
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(KOCam, HandleUpdate));
}

void KOCam::SetupViewport()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();

    //Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, masterControl_->world.scene, camera_));
    viewport_ = viewport;

    //Add anti-asliasing and bloom
    effectRenderPath = viewport_->GetRenderPath()->Clone();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
    effectRenderPath->SetEnabled("FXAA3", true);
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->SetShaderParameter("BloomThreshold", 0.8f);
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.75f, 0.75f));
    effectRenderPath->SetEnabled("Bloom", true);

    viewport_->SetRenderPath(effectRenderPath);
    renderer->SetViewport(0, viewport);
}

Vector3 KOCam::GetWorldPosition() const
{
    return rootNode_->GetWorldPosition();
}

Quaternion KOCam::GetRotation() const
{
    return rootNode_->GetRotation();
}

void KOCam::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    assert(eventType);
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    Vector3 targetPosition = masterControl_->world.player_->GetPosition();
    Vector3 targetVelocity = masterControl_->world.player_->GetLinearVelocity();

    Input* input = GetSubsystem<Input>();

    //Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    Vector3 camForward = rootNode_->GetDirection();
    camForward = LucKey::Scale(camForward, Vector3::ONE - Vector3::UP).Normalized();

    Vector3 normalizedPlanarDirection = LucKey::Scale( rootNode_->GetDirection(), Vector3::ONE - Vector3::UP ).Normalized();
    if (input->GetKeyDown('T')) velocity_ +=  normalizedPlanarDirection * acceleration_ * timeStep;
    if (input->GetKeyDown('G')) velocity_ += -normalizedPlanarDirection * acceleration_ * timeStep;
    if (input->GetKeyDown('H')) rotationSpeed_ -= angularAcceleration_ * timeStep;
    if (input->GetKeyDown('F')) rotationSpeed_ += angularAcceleration_ * timeStep;
    if (input->GetKeyDown('Y')) velocity_ += Vector3::UP * acceleration_ * timeStep;
    if (input->GetKeyDown('R') && rootNode_->GetPosition().y_ > 1.5f) velocity_ += Vector3::DOWN * acceleration_ * timeStep;


    //Read joystick input
    JoystickState* joystickState = input->GetJoystickByIndex(0);
    if (joystickState){
        rotationSpeed_ += joystickState->GetAxisPosition(2) * timeStep * angularAcceleration_;
        velocity_ -= joystickState->GetAxisPosition(3) * camForward * acceleration_ * timeStep;
//        if (joystickState->GetButtonDown(JB_R2)) velocity_ += Vector3::UP;
//        if (joystickState->GetButtonDown(JB_L2) && rootNode_->GetPosition().y_ > 1.0f) velocity_ += Vector3::DOWN;
    }

    velocity_ *= 0.95f;
    rotationSpeed_ *= 0.95f;


    if ( velocityMultiplier_ < 8.0f && (input->GetKeyDown(KEY_LSHIFT)||input->GetKeyDown(KEY_RSHIFT)) ){
        velocityMultiplier_ += 0.23f;
    } else velocityMultiplier_ = pow(velocityMultiplier_, 0.75f);
    rootNode_->Translate(velocity_ * velocityMultiplier_ * timeStep, TS_WORLD);

    //Rotate left and right
    Clamp(rotationSpeed_, -maxRotationSpeed_, maxRotationSpeed_);
    rootNode_->RotateAround(targetPosition, Quaternion(rotationSpeed_ * velocityMultiplier_ * timeStep, Vector3::UP), TS_WORLD);

    //Prevent camera from going too low
    float yPos{rootNode_->GetPosition().y_};
    if (yPos < 1.5f)
    {
        velocity_.y_ = velocity_.y_ < 0.0f ? 0.0f : velocity_.y_;
        rootNode_->SetPosition(Vector3(rootNode_->GetPosition().x_, (yPos-1.5f)*0.23f+1.5f, rootNode_->GetPosition().z_));
    } else if (yPos > 23.0f){
        velocity_.y_ = velocity_.y_ > 0.0f ? 0.0f : velocity_.y_;
        rootNode_->SetPosition(Vector3(rootNode_->GetPosition().x_, (yPos-23.0f)*0.23f+23.0f, rootNode_->GetPosition().z_));
    }

    smoothTargetPosition_ = 0.1f * (9.0f * smoothTargetPosition_ + targetPosition);
    smoothTargetVelocity_ = 0.1f * (9.0f * smoothTargetVelocity_ + targetVelocity);
    rootNode_->Translate(smoothTargetVelocity_ * timeStep, TS_WORLD);
    Quaternion camRot = rootNode_->GetWorldRotation();
    Quaternion aimRotation = camRot;
    aimRotation.FromLookRotation(smoothTargetPosition_ - rootNode_->GetWorldPosition());
    rootNode_->SetRotation(aimRotation);
}

void KOCam::Lock(const Node& target)
{

}
