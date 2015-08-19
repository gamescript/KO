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

#pragma once

#include <Urho3D/Urho3D.h>

#include "deco.h"
#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Frop : public Deco
{
    OBJECT(Frop);
public:
    Frop(Context *context, MasterControl* masterControl, Vector3 position = Vector3::ZERO);
private:
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    StaticModel* fropModel_;
    Vector3 scale_;

    float growthStart_;

    float age_ = 0.0;
};
