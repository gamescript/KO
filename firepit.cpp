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

void FirePit::RegisterObject(Context *context)
{
    context->RegisterFactory<FirePit>();
}

FirePit::FirePit(Context* context):
    SceneObject(context)
{
}

void FirePit::OnNodeSet(Node *node)
{
    node_->SetRotation(Quaternion(0.0f, variator_ * 360.0f, 0.0f));

    StaticModel* model_ = node_->CreateComponent<StaticModel>();
    model_->SetModel(MC->cache_->GetResource<Model>("Models/FirePit.mdl"));
    model_->SetMaterial(0, MC->cache_->GetResource<Material>("Materials/Metal.xml"));
    model_->SetMaterial(1, MC->resources.materials.darkness);
    model_->SetMaterial(2, MC->cache_->GetResource<Material>("Materials/Amber.xml"));
    model_->SetCastShadows(true);

    node_->CreateComponent<RigidBody>();
    CollisionShape* collider = node_->CreateComponent<CollisionShape>();
    collider->SetCylinder(0.4f, 0.5f);

    Node* particleNode = node_->CreateChild("Fire");
    particleNode->SetPosition(Vector3::UP * 0.16f);
    ParticleEmitter* flameEmitter = particleNode->CreateComponent<ParticleEmitter>();
    flameEmitter->SetEffect(MC->cache_->GetResource<ParticleEffect>("Particles/fire1.xml"));
    ParticleEmitter* sparkEmitter = particleNode->CreateComponent<ParticleEmitter>();
    sparkEmitter->SetEffect(MC->cache_->GetResource<ParticleEffect>("Particles/fire_sparks.xml"));

    lightNode_ = node_->CreateChild("LightNode");
    light_ = lightNode_->CreateComponent<Light>();
    light_->SetColor(Color(1.0f, 0.6f, 0.4f));
    light_->SetRange(5.0f);
    light_->SetCastShadows(true);
    light_->SetShadowBias(BiasParameters(0.00005f, 0.5f));
    light_->SetShadowCascade(CascadeParameters(1.0f, 2.0f, 3.0f, 5.0f, 0.5f));
    light_->SetShadowResolution(0.25f);
}

void FirePit::Update(float timeStep)
{
    UpdateLightPosition();
    UpdateBrightness();
}

void FirePit::UpdateLightPosition()
{
    float range = 0.001f;
    float x = 0.0f;
    for (int i = 1; i < 9; i++)
        x += MC->Sine(4.0f + i, -range, range, i+(i*variator_ * 1.0f*M_PI))/(i*0.666f);
    float y = 0.5f;
    for (int i = 1; i < 9; i++)
        y += MC->Sine(5.0f + i, -range, range, i+(i*variator_ * 1.5f*M_PI))/(i*0.666f);
    float z = 0.0f;
    for (int i = 1; i < 9; i++)
        z += MC->Sine(6.0f + i, -range, range, i+(i*variator_ * 2.0f*M_PI))/(i*0.666f);
    lightNode_->SetPosition(Vector3(x, y, z));
}

void FirePit::UpdateBrightness()
{
    float brightness = 1.23f;
    for (int i = 1; i < 5; i++)
    {
        brightness += MC->Sine(variator_ + 7.123f + i, -0.001f, 0.023f, (variator_ * 2.0f*M_PI) + i*(0.2f+variator_));
    }
    light_->SetBrightness(brightness);
}
