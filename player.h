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

#ifndef PLAYER_H
#define PLAYER_H

#include <Urho3D/Urho3D.h>

#include "sceneobject.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
}

using namespace Urho3D;

class Player : public SceneObject
{
    OBJECT(Player);
    friend class KOCam;
    friend class FloatingEye;
public:
    Player(Context* context, MasterControl* masterControl);
    //SharedPtr<Text> scoreText_;

    double GetHealth(){return health_;}
    void Hit(float damage, int ownerID);
    void AddScore(int points);
    Vector3 GetLinearVelocity() {return rigidBody_->GetLinearVelocity();}
private:
    float health_ = 1.0f;
    float initialHealth_ = 1.0f;
    int firstHitBy_ = 0;
    int lastHitBy_ = 0;
    int score_ = 0;

    const float shotInterval_ = 0.23f;
    float sinceLastShot_ = 0.0f;

    AnimatedModel* model_;
    AnimationController* animCtrl_;

    RigidBody* rigidBody_;

    SharedPtr<Sound> sample_;
    Vector<SharedPtr<SoundSource> > sampleSources_;

    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    void PlaySample(Sound *sample);
    void Hack();
};

#endif // PLAYER_H
