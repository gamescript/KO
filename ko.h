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

#ifndef KO_H
#define KO_H

#include <Urho3D/Urho3D.h>

#include "controllable.h"

//using namespace Urho3D;

enum KOActions{ RUN, HACK, BASH, KICK, CAST };

class KO : public Controllable
{
    URHO3D_OBJECT(KO, Controllable);
    friend class KOCam;
    friend class FloatingEye;
public:
    static void RegisterObject(Context *context);
    KO(Context* context);
    virtual void OnNodeSet(Node *node);

    float GetHealth(){ return health_; }
    void Hit(float damage, int ownerID);
    void AddScore(int points);
    Vector3 GetLinearVelocity() {return rigidBody_->GetLinearVelocity();}

    void SetPosition(const Vector3& pos) { node_->SetPosition(pos); }

    void EquipRightHand();
    void EquipLeftHand();
protected:
    void HandleAction(int actionId);
private:
    float health_;
    float initialHealth_;
    int firstHitBy_;
    int lastHitBy_;
    int score_;

    StaticModel* leftHand_;
    StaticModel* rightHand_;

    SharedPtr<Sound> sample_;
    Vector<SharedPtr<SoundSource> > sampleSources_;

    void Update(float timeStep);
    void PlaySample(Sound *sample);
    void Hack();
};

#endif // KO_H
