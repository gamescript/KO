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

#include <Urho3D/Input/Input.h>

#include "mastercontrol.h"
#include "dungeon.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class InputMaster : public Object
{
    OBJECT(InputMaster);
public:
    InputMaster(Context* context, MasterControl* masterControl);
    WeakPtr<Node> firstHit_;

    void DeselectAll();
private:
    MasterControl* masterControl_;

    Input* input_;
    void HandleMouseDown(StringHash eventType, VariantMap &eventData);
    void HandleKeyDown(StringHash eventType, VariantMap &eventData);
    void HandleMouseUp(StringHash eventType, VariantMap &eventData);

    Vector<SharedPtr<Dungeon> > selectedDungeons_;
    void SetSelection(SharedPtr<Dungeon> platform);
};
