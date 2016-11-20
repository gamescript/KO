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
#include "frop.h"
#include "firepit.h"

Tile::Tile(Context *context, const IntVector2 coords, Dungeon *platform):
    Object(context)
{
    masterControl_ = platform->masterControl_;
    dungeon_ = platform;
    coords_ = coords;

    rootNode_ = dungeon_->rootNode_->CreateChild("Tile");
    rootNode_->SetPosition(Vector3((float)coords_.x_, 0.0f, -(float)coords_.y_));

    //Set up center and edge nodes.
    for (int i = 0; i != TE_LENGTH; i++){
        elements_[i] = rootNode_->CreateChild("TilePart");
        int nthOfType = ((i-1)%4);
        if (i > 0) elements_[i]->Rotate(Quaternion(0.0f, 90.0f-nthOfType*90.0f, 0.0f));
        //Add the right model to the node
        StaticModel* model = elements_[i]->CreateComponent<StaticModel>();
        switch (i){
        case TE_CENTER:    model->SetModel(MC->GetModel("Block_center"));
            model->SetMaterial(MC->GetMaterial("Floor"));
            break;
        case TE_NORTH:case TE_EAST:case TE_SOUTH:case TE_WEST:
            model->SetModel(MC->GetModel("Block_tween"));
            model->SetMaterial(0, MC->GetMaterial("Floor"));
            break;
        case TE_NORTHEAST:case TE_SOUTHEAST:case TE_SOUTHWEST:case TE_NORTHWEST:
            model->SetModel(MC->GetModel("Block_outcorner"));
            model->SetMaterial(0, MC->GetMaterial("Wall"));
            if (model->GetNumGeometries() > 2)
            model->SetMaterial(1, MC->GetMaterial("Darkness"));
            break;
        default:break;
        }
        model->SetCastShadows(true);
    }
}

//Fix this tile's element models and materials according to
void Tile::FixFringe()
{
    for (int element = 1; element != TE_LENGTH; element++)
    {
        StaticModel* model = elements_[element]->GetComponent<StaticModel>();
        //Fix sides
        if (element <= 4){
            //If corresponding neighbour is empty
            if (dungeon_->CheckEmptyNeighbour(coords_, (TileElement)element, true))
            {
                model->SetModel(MC->GetModel("Block_side"));
                model->SetMaterial(0, MC->GetMaterial("Wall"));
                model->SetMaterial(1, MC->GetMaterial("Darkness"));
            }
            //If neighbour is not empty
            else {
                if (element == 1 || element == 4) {
                    model->SetModel(MC->GetModel("Block_tween"));
                    model->SetMaterial(0, MC->GetMaterial("Floor"));
                }
                else model->SetModel(SharedPtr<Model>());
            }
        }
        //Fix corners
        else {
            switch (dungeon_->PickCornerType(coords_, (TileElement)element)){
            case CT_NONE:   model->SetModel(SharedPtr<Model>()); break;
            case CT_IN:
                model->SetModel(MC->GetModel("Block_incorner"));
                model->SetMaterial(0, MC->GetMaterial("Wall"));
                model->SetMaterial(1, MC->GetMaterial("Darkness"));
                break;
            case CT_OUT:
                model->SetModel(MC->GetModel("Block_outcorner"));
                model->SetMaterial(0, MC->GetMaterial("Wall"));
                break;
            case CT_TWEEN:
                model->SetModel(MC->GetModel("Block_tweencorner"));
                model->SetMaterial(0, MC->GetMaterial("Wall"));
                model->SetMaterial(1, MC->GetMaterial("Darkness"));
                break;
            case CT_DOUBLE:
                model->SetModel(MC->GetModel("Block_doublecorner"));
                model->SetMaterial(0, MC->GetMaterial("Wall"));
                break;
            case CT_FILL:
                model->SetModel(MC->GetModel("Block_fillcorner"));
                model->SetMaterial(0, MC->GetMaterial("Floor"));
                break;
            default: break;
            }
        }
    }
}

