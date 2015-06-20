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

#include "tile.h"
#include "deco.h"

Deco::Deco(Context* context, MasterControl *masterControl, Tile* tile, String name):
    SceneObject(context, masterControl)
{
    rootNode_->SetParent(tile->rootNode_);
    rootNode_->SetPosition(Vector3(
                               Random(-0.25f, 0.25f),
                               0.0f,
                               Random(-0.25f, 0.25f)));
}
