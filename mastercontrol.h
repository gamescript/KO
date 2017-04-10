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

#ifndef MASTERCONTROL_H
#define MASTERCONTROL_H

#include <Urho3D/Urho3D.h>

#include "luckey.h"

using namespace Urho3D;

class KOCam;
class InputMaster;
class Dungeon;
class KO;
class Player;

typedef struct GameWorld
{
    SharedPtr<KOCam> camera;
    SharedPtr<Scene> scene;
    SharedPtr<Node> voidNode;
    SharedPtr<KO> ko;
    struct {
        SharedPtr<Node> sceneCursor;
        SharedPtr<Cursor> uiCursor;
        PODVector<RayQueryResult> hitResults;
    } cursor;
} GameWorld;

typedef struct HitInfo
{
    Vector3 position_;
    Vector3 hitNormal_;
    Node* hitNode_;
    Drawable* drawable_;
} HitInfo;

namespace {
StringHash const N_VOID = StringHash("Void");
StringHash const N_CURSOR = StringHash("Cursor");
StringHash const N_TILEPART = StringHash("TilePart");
StringHash const N_SLOT = StringHash("Slot");
StringHash const N_FLOATINGEYE = StringHash("FloatingEye");
}

#define MC MasterControl::GetInstance()

class MasterControl : public Application
{
    URHO3D_OBJECT(MasterControl, Application);
    friend class InputMaster;
    friend class KOCam;
public:
    MasterControl(Context* context);
    static MasterControl* GetInstance();
    String GetResourceFolder() const { return resourceFolder_; }

    GameWorld world;
    SharedPtr<Graphics> graphics_;

    Vector< SharedPtr<Player> > players_;
    Vector< SharedPtr<Player> > GetPlayers() const;
    Player* GetPlayer(int playerID) const;

    Material* GetMaterial(String name) const;
    Model* GetModel(String name) const;
    Texture* GetTexture(String name) const;
    Sound* GetMusic(String name) const;
    Sound* GetSample(String name) const;

    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    void Exit();

    void CreateSineLookupTable();
    float Sine(float x);
    float Sine(float freq, float min, float max, float shift = 0.0f);
private:
    static MasterControl* instance_;
    String resourceFolder_;

    SharedPtr<UI> ui_;
    SharedPtr<Renderer> renderer_;
    SharedPtr<XMLFile> defaultStyle_;

    void CreateConsoleAndDebugHud();

    void CreateScene();
    void CreateUI();
    void SubscribeToEvents();

    void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void CreateDungeon(const Vector3 pos);
    void UpdateCursor(double timeStep);
    bool CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults);

    bool paused_;

    Vector<double> sine_;
    void LoadResources();

};

#endif // MASTERCONTROL_H
