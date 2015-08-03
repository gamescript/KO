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

#include "wallcollider.h"

WallCollider::WallCollider(Context* context, Dungeon* dungeon, IntVector2 coords):
    Object(context)
{
    CollisionShape* collider = dungeon->rootNode_->CreateComponent<CollisionShape>();
    Vector3 position = Vector3(coords.x_, 0.0f, -coords.y_);
    collider->SetBox(Vector3(1.25f, 1.0f, 1.25f), position);
}
