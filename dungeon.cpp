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

#include "dungeon.h"

#include "floatingeye.h"
#include "firepit.h"
#include "frop.h"
#include "tile.h"
#include "wallcollider.h"
#include "kocam.h"
#include "player.h"

namespace Urho3D {
template <> unsigned MakeHash(const IntVector2& value)
  {
    return LucKey::IntVector2ToHash(value);
  }
}

Dungeon::Dungeon(Context *context, MasterControl* masterControl):
    Object(context)
{
    masterControl_ = masterControl;
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Dungeon, HandleUpdate));
    rootNode_ = MC->world.scene->CreateChild("Dungeon");

    rigidBody_ = rootNode_->CreateComponent<RigidBody>();

    //InitializeRandom();

    TmxFile2D* tmxFile = MC->cache_->GetResource<TmxFile2D>("Maps/test.tmx");
    if (tmxFile)
        InitializeFromMap(*tmxFile);
    else
        InitializeRandom();
}


void Dungeon::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

void Dungeon::AddTile(IntVector2 newTileCoords)
{
    tileMap_[newTileCoords] = SharedPtr<Tile>(new Tile(context_, newTileCoords, this));
}

void Dungeon::InitializeRandom()
{
    // Add base tile
    IntVector2 firstCoordPair = IntVector2(0,0);
    tileMap_[firstCoordPair] = new Tile(context_, firstCoordPair, this);
    // Add random tiles
    int addedTiles = 1;
    int dungeonSize = 32;

    while (addedTiles < dungeonSize){
        //Pick a random existing tile from a list.
        Vector<IntVector2> coordsVector = tileMap_.Keys();
        IntVector2 randomTileCoords = coordsVector[Random((int)coordsVector.Size())];

        //Check neighbours in random order
        char startDir = Random(1,4);
        for (int direction = startDir; direction < startDir+4; direction++){
            int clampedDir = LucKey::Cycle(direction, 1, 4);
            if (CheckEmptyNeighbour(randomTileCoords, (TileElement)clampedDir, true))
            {
                IntVector2 newTileCoords = GetNeighbourCoords(randomTileCoords, (TileElement)clampedDir);
                AddTile(newTileCoords);
                addedTiles++;
                if (newTileCoords.x_ != 0) {
                    IntVector2 mirrorTileCoords = LucKey::Scale(newTileCoords, IntVector2(-1,1));
                    AddTile(mirrorTileCoords);
                    addedTiles++;
                }
                if (newTileCoords.y_ != 0) {
                    IntVector2 mirrorTileCoords = LucKey::Scale(newTileCoords, IntVector2(1,-1));
                    AddTile(mirrorTileCoords);
                    addedTiles++;
                }
                if (newTileCoords.x_ != 0 && newTileCoords.y_ != 0) {
                    IntVector2 mirrorTileCoords = LucKey::Scale(newTileCoords, IntVector2(-1,-1));
                    AddTile(mirrorTileCoords);
                    addedTiles++;
                }
            }
        }
    }

    //Add Colliders
    AddColliders();
    FixFringe();

    for (int i{0}; i < 23; i++){

        FloatingEye* fe{ MC->world.scene->CreateChild("FloatingEye")->CreateComponent<FloatingEye>() };
        fe->Set(Vector3(Random(-5.0f, 5.0f), 0.0f, Random(-5.0f, 5.0f)));
    }
}

void Dungeon::InitializeFromMap(const TmxFile2D& tmxFile)
{
    const TmxLayer2D* layer0 = tmxFile.GetLayer(0);
    if (!layer0)
        return;
    if (layer0->GetType() != LT_TILE_LAYER)
        return;

    const TmxTileLayer2D& tileLayer = *static_cast<const TmxTileLayer2D*>(layer0);

    for (int y = 0; y < tileLayer.GetHeight(); ++y) {
        for (int x = 0; x < tileLayer.GetWidth(); ++x) {
            if (Tile2D* tile = tileLayer.GetTile(x, y)) {
                if (tile->HasProperty("floor"))
                    AddTile(IntVector2(x, y));
            }
        }
    }

    const TmxLayer2D* layer1 = tmxFile.GetLayer(1);
    if (!layer1)
        return;
    if (layer1->GetType() != LT_OBJECT_GROUP)
        return;

    const TmxObjectGroup2D& objectGroup = *static_cast<const TmxObjectGroup2D*>(layer1);

    for (unsigned i = 0; i < objectGroup.GetNumObjects(); ++i) {
        TileMapObject2D* object = objectGroup.GetObject(i);
        int gid = object->GetTileGid();
        PropertySet2D* properties = tmxFile.GetTilePropertySet(gid);
        if (!properties)
            continue;

        Vector3 pos(object->GetPosition().x_ * 3.12f, 0.0f, 3.12f * object->GetPosition().y_ - tileLayer.GetHeight() + 1.0f);
        if (properties->HasProperty("FloatingEye")) {

            FloatingEye* fe{ MC->world.scene->CreateChild()->CreateComponent<FloatingEye>() };
            fe->Set(pos);
        } else if (properties->HasProperty("Player")) {
            MC->world.player_->Set(pos);
        } else if (properties->HasProperty("FirePit")) {
            FirePit* fp{ MC->world.scene->CreateChild()->CreateComponent<FirePit>() };
            fp->Set(pos);
        } else if (properties->HasProperty("Plant")) {
            Frop* f{ MC->world.scene->CreateChild()->CreateComponent<Frop>() };
            f->Set(pos);
        }
    }

    AddColliders();
    FixFringe();
}

void Dungeon::AddColliders()
{
    for (auto i = tileMap_.Begin(); i != tileMap_.End(); ++i) {
        const IntVector2& coords = i->first_;
        for (int element = 0; element <= 4; element++){
            IntVector2 checkCoords = GetNeighbourCoords(coords, (TileElement)element);
            if (CheckEmpty(checkCoords, false) && CheckEmpty(checkCoords, true))
                collisionMap_[checkCoords] = new WallCollider(context_, this, checkCoords);
        }
    }
}

void Dungeon::FixFringe()
{
    Vector<SharedPtr<Tile> > tiles = tileMap_.Values();
    for (unsigned tile = 0; tile < tiles.Size(); tile++)
    {
        tiles[tile]->FixFringe();
    }
}

void Dungeon::FixFringe(IntVector2 coords)
{
    for (int coordsOffset = 0; coordsOffset < TE_LENGTH; coordsOffset++)
    {
        IntVector2 neighbourCoords = GetNeighbourCoords(coords, (TileElement)coordsOffset);
        if (!CheckEmpty(neighbourCoords, true)) tileMap_[neighbourCoords]->FixFringe();
    }
}

bool Dungeon::CheckEmpty(IntVector2 coords, bool checkTiles) const
{
    if (checkTiles)
        return (!tileMap_.Contains(coords));
    else
        return (!collisionMap_.Contains(coords));
}


bool Dungeon::CheckEmptyNeighbour(IntVector2 coords, TileElement element, bool checkTiles) const
{
    return CheckEmpty(GetNeighbourCoords(coords, element), checkTiles);
}



IntVector2 Dungeon::GetNeighbourCoords(IntVector2 coords, TileElement element) const
{
    IntVector2 shift = IntVector2::ZERO;
    switch (element){
    case TE_NORTH: shift.y_ =  1; break;
    case TE_EAST:  shift.x_ =  1; break;
    case TE_SOUTH: shift.y_ = -1; break;
    case TE_WEST:  shift.x_ = -1; break;
    case TE_NORTHEAST: shift.x_ =  1; shift.y_ =  1; break;
    case TE_SOUTHEAST: shift.x_ =  1; shift.y_ = -1; break;
    case TE_SOUTHWEST: shift.x_ = -1; shift.y_ = -1; break;
    case TE_NORTHWEST: shift.x_ = -1; shift.y_ =  1; break;
    default: case TE_CENTER: break;
    }
    return coords + shift;
}

TileType Dungeon::GetTileType(IntVector2 coords)
{
    if (!CheckEmpty(coords))
    {
        return tileMap_[coords]->buildingType_;
    }
    else return TT_SPACE;
}

TileType Dungeon::GetNeighbourType(IntVector2 coords, TileElement element)
{
    return GetTileType(GetNeighbourCoords(coords, element));
}

CornerType Dungeon::PickCornerType(IntVector2 tileCoords, TileElement element) const
{
    bool emptyCheck[3] = {false, false, false};
    switch (element){
    case TE_NORTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_EAST);
    }
    break;
    case TE_SOUTHEAST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_EAST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHEAST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
    }break;
    case TE_SOUTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_SOUTH);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_SOUTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_WEST);
    }break;
    case TE_NORTHWEST: {
        emptyCheck[0] = CheckEmptyNeighbour(tileCoords, TE_WEST);
        emptyCheck[1] = CheckEmptyNeighbour(tileCoords, TE_NORTHWEST);
        emptyCheck[2] = CheckEmptyNeighbour(tileCoords, TE_NORTH);
    }break;
    default: break;
    }

    int neighbourMask = 0;
    for (int i = 2; i >= 0; i--){
        neighbourMask += !emptyCheck[i] << i;
    }
    switch (neighbourMask){
    case 0: return CT_IN; break;
    case 1: return CT_TWEEN; break;
    case 2: return CT_DOUBLE; break;
    case 3: return CT_NONE; break;
    case 4: return CT_NONE; break;
    case 5: return CT_OUT; break;
    case 6: return CT_NONE; break;
    case 7: return (element == TE_SOUTHEAST) ? CT_FILL : CT_NONE; break;
    default: return CT_NONE; break;
    }
}
