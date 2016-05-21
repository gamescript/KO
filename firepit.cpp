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

#include "firepit.h"

FirePit::FirePit(Context* context, MasterControl* masterControl):
    SceneObject(context, masterControl)
{
    rootNode_->SetRotation(Quaternion(0.0f, variator_ * 360.0f, 0.0f));

    StaticModel* model_ = rootNode_->CreateComponent<StaticModel>();
    model_->SetModel(masterControl_->cache_->GetResource<Model>("Models/FirePit.mdl"));
    model_->SetMaterial(0, masterControl_->cache_->GetResource<Material>("Materials/Metal.xml"));
    model_->SetMaterial(1, masterControl_->resources.materials.darkness);
    model_->SetMaterial(2, masterControl_->cache_->GetResource<Material>("Materials/Amber.xml"));
    model_->SetCastShadows(true);

    rootNode_->CreateComponent<RigidBody>();
    CollisionShape* collider = rootNode_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.4f, 0.5f);

    Node* particleNode = rootNode_->CreateChild("Fire");
    particleNode->SetPosition(Vector3::UP * 0.16f);
    ParticleEmitter* flameEmitter = particleNode->CreateComponent<ParticleEmitter>();
    flameEmitter->SetEffect(masterControl_->cache_->GetResource<ParticleEffect>("Particles/fire1.xml"));
    ParticleEmitter* sparkEmitter = particleNode->CreateComponent<ParticleEmitter>();
    sparkEmitter->SetEffect(masterControl_->cache_->GetResource<ParticleEffect>("Particles/fire_sparks.xml"));

    lightNode_ = rootNode_->CreateChild("LightNode");
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetColor(Color(1.0f, 0.6f, 0.4f));
    light_->SetRange(5.0f);
    light_->SetCastShadows(true);
    light_->SetShadowBias(BiasParameters(0.00005f, 0.5f));    
    light_->SetShadowCascade(CascadeParameters(1.0f, 2.0f, 3.0f, 5.0f, 0.5f));
    light_->SetShadowResolution(0.25f);

    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(FirePit, HandleSceneUpdate));
}

void FirePit::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    UpdateLightPosition();
    UpdateBrightness();
}

void FirePit::UpdateLightPosition()
{
    float range = 0.001f;
    float x = 0.0f;
    for (int i = 1; i < 9; i++)
        x += masterControl_->Sine(4.0f + i, -range, range, i+(i*variator_ * 1.0f*M_PI))/(i*0.666f);
    float y = 0.5f;
    for (int i = 1; i < 9; i++)
        y += masterControl_->Sine(5.0f + i, -range, range, i+(i*variator_ * 1.5f*M_PI))/(i*0.666f);
    float z = 0.0f;
    for (int i = 1; i < 9; i++)
        z += masterControl_->Sine(6.0f + i, -range, range, i+(i*variator_ * 2.0f*M_PI))/(i*0.666f);
    lightNode_->SetPosition(Vector3(x, y, z));
}

void FirePit::UpdateBrightness()
{
    float brightness = 1.23f;
    for (int i = 1; i < 5; i++)
    {
        brightness += masterControl_->Sine(variator_ + 7.123f + i, -0.001f, 0.023f, (variator_ * 2.0f*M_PI) + i*(0.2f+variator_));
    }
    light_->SetBrightness(brightness);
}
