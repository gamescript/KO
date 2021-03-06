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

#ifndef FLOATINGEYE_H
#define FLOATINGEYE_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "npc.h"

using namespace Urho3D;

class FloatingEye : public NPC
{
    URHO3D_OBJECT(FloatingEye, NPC);
public:
    static void RegisterObject(Context *context);
    FloatingEye(Context *context);
    virtual void OnNodeSet(Node *node);

    virtual void Update(float timeStep);
protected:
    Node* modelNode_;
    StaticModel* ballModel_;
    StaticModel* corneaModel_;
private:
    Vector3 smoothTargetPosition_;

};

#endif // FLOATINGEYE_H
