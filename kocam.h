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

#ifndef KOCAM_H
#define KOCAM_H

#include <Urho3D/Urho3D.h>

#include "mastercontrol.h"
#include "dungeon.h"

namespace Urho3D {
class Drawable;
class Node;
class Scene;
class Sprite;
class Viewport;
class RenderPath;
class Camera;
}

using namespace Urho3D;

class KOCam : public Object
{
    OBJECT(KOCam);
    friend class MasterControl;
    friend class InputMaster;
    friend class Player;
public:
    KOCam(Context *context, MasterControl* masterControl);

    virtual void Start();
    virtual void Stop();

    SharedPtr<Camera> camera_;
    SharedPtr<Viewport> viewport_;
    SharedPtr<RenderPath> effectRenderPath;

    Vector3 GetWorldPosition();
    Quaternion GetRotation();
private:
    MasterControl* masterControl_;
    void HandleUpdate(StringHash eventType, VariantMap &eventData);
    SharedPtr<Node> rootNode_;
    Vector3 smoothTargetPosition_;
    Vector3 smoothTargetVelocity_;

    SharedPtr<RigidBody> rigidBody_;
    float yaw_;
    float pitch_;
    //double roll_ = 0.0;
    float yawDelta_ = 0.0;
    float pitchDelta_ = 0.0;
    float forceMultiplier = 1.0;
    void SetupViewport();
    void Lock(SharedPtr<Dungeon> platform);
};

#endif // KOCAM_H
