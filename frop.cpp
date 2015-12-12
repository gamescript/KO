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

#include "frop.h"

Frop::Frop(Context *context, MasterControl *masterControl) :
    Deco(context, masterControl)
{
    growthStart_ = Random(0.f, 5.f);
    rootNode_->Rotate(Quaternion(Random(-10.f, 10.f),Random(360.f),Random(-10.f, 10.f)));
    rootNode_->SetScale(0.f);
    float randomWidth = Random(0.5f,2.f);
    scale_ = Vector3(randomWidth, Random(0.5f,1.f+randomWidth), randomWidth);
    fropModel_ = rootNode_->CreateComponent<StaticModel>();
    fropModel_->SetModel(masterControl_->cache_->GetResource<Model>("Resources/Models/Frop.mdl"));
    fropModel_->SetMaterial(masterControl_->cache_->GetResource<Material>("Resources/Materials/Frop.xml"));
    fropModel_->SetCastShadows(true);

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Frop, HandleUpdate));
}

void Frop::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    double timeStep = eventData[P_TIMESTEP].GetFloat();
    age_ += timeStep;
    if (age_ > growthStart_ && rootNode_->GetScale().Length() < scale_.Length()-0.01f)
        rootNode_->SetScale(rootNode_->GetScale()+(timeStep*(scale_ - rootNode_->GetScale())));
}
