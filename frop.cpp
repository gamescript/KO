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

void Frop::RegisterObject(Context *context)
{
    context->RegisterFactory<Frop>();
}

Frop::Frop(Context *context) :
    Deco(context)
{
}

void Frop::OnNodeSet(Node *node)
{
    Deco::OnNodeSet(node);

    growthStart_ = Random(0.0f, 5.0f);
    node_->Rotate(Quaternion(Random(-10.0f, 10.0f),
                             Random(360.0f),
                             Random(-10.0f, 10.0f)));
    node_->SetScale(0.0f);
    float randomWidth{ Random(0.5f, 2.0f) };
    scale_ = Vector3(randomWidth, Random(0.5f,1.0f+randomWidth), randomWidth);
    fropModel_ = node_->CreateComponent<StaticModel>();
    fropModel_->SetModel(MC->GetModel("Frop"));
    fropModel_->SetMaterial(MC->GetMaterial("Frop"));
    fropModel_->SetCastShadows(true);
}

void Frop::Update(float timeStep)
{
    age_ += timeStep;
    if (age_ > growthStart_ && node_->GetScale().Length() < scale_.Length()-0.01f)
        node_->SetScale(node_->GetScale()+(timeStep*(scale_ - node_->GetScale())));
}
