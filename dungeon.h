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

#ifndef DUNGEON_H
#define DUNGEON_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Tile;
class WallCollider;

enum TileElement {TE_CENTER = 0, TE_NORTH, TE_EAST, TE_SOUTH, TE_WEST, TE_NORTHWEST, TE_NORTHEAST, TE_SOUTHEAST, TE_SOUTHWEST, TE_LENGTH};
enum CornerType {CT_NONE, CT_IN, CT_OUT, CT_TWEEN, CT_DOUBLE, CT_FILL};
enum TileType {TT_SPACE, TT_EMPTY, TT_ENGINE};

class Dungeon : public Object
{
    OBJECT(Dungeon);
    friend class InputMaster;
public:
    Dungeon(Context *context, Vector3 position, MasterControl* masterControl);

    MasterControl* masterControl_;
    Node* rootNode_;

    bool CheckEmpty(Vector3 coords, bool checkTiles) const { return CheckEmpty(IntVector2(round(coords.x_), round(coords.z_)), checkTiles); }
    bool CheckEmpty(IntVector2 coords, bool checkTiles) const;
    bool CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool tileMap) const;
    IntVector2 GetNeighbourCoords(IntVector2 coords, TileElement element) const;
    CornerType PickCornerType(IntVector2 tileCoords, TileElement element) const;
    TileType GetTileType(IntVector2 coords);
    TileType GetNeighbourType(IntVector2 coords, TileElement element);


    virtual void Start();
    virtual void Stop();
    void AddColliders();
    void FixFringe();
    void FixFringe(IntVector2 coords);

    void AddTile(IntVector2 newTileCoords);
private:
    RigidBody* rigidBody_;
    HashMap<IntVector2, SharedPtr<Tile> > tileMap_;
    HashMap<IntVector2, SharedPtr<WallCollider> > collisionMap_;
    HashMap<IntVector2, TileType> buildingMap_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
};

#endif // DUNGEON_H
