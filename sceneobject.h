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

#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

using namespace Urho3D;

class SceneObject : public LogicComponent
{
    URHO3D_OBJECT(SceneObject, LogicComponent);
public:
    SceneObject(Context *context);
    virtual void OnNodeSet(Node *node);

    void Set(Vector3 position);
    Vector3 GetPosition() const {return node_->GetWorldPosition();}
protected:
    void Disable();
    const float variator_;
};

#endif // SCENEOBJECT_H
