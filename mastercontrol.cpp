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

#include "inputmaster.h"
#include "kocam.h"
#include "dungeon.h"
#include "player.h"
#include "ko.h"
#include "floatingeye.h"
#include "firepit.h"
#include "frop.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;

MasterControl* MasterControl::GetInstance()
{
    return MasterControl::instance_;
}

MasterControl::MasterControl(Context *context):
    Application(context),
    paused_(false),
    world{}
{
    instance_ = this;
}

void MasterControl::Setup()
{
    engineParameters_["WindowTitle"] = "KO: The Curse of Greyface";
    engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")+"KO.log";
    engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
    engineParameters_["WindowIcon"] = "icon.png";
//    engineParameters_["FullScreen"] = false;
//    engineParameters_["Headless"] = true;
//    engineParameters_["WindowWidth"] = 960;
//    engineParameters_["WindowHeight"] = 540;
}
void MasterControl::Start()
{
    FloatingEye::RegisterObject(context_);
    FirePit::RegisterObject(context_);
    Frop::RegisterObject(context_);
    KOCam::RegisterObject(context_);
    KO::RegisterObject(context_);

    context_->RegisterSubsystem(new InputMaster(context_));
//    cache_ = GetSubsystem<ResourceCache>();
    graphics_ = GetSubsystem<Graphics>();
    renderer_ = GetSubsystem<Renderer>();

    LoadResources();

    CreateSineLookupTable();

    CreateConsoleAndDebugHud();
    CreateScene();
    CreateUI();
    SubscribeToEvents();

    Sound* music{ CACHE->GetResource<Sound>("Music/Pantera_Negra_-_Sumerian_Speech.ogg") };
    music->SetLooped(true);
    Node* musicNode{ world.scene->CreateChild("Music") };
    SoundSource* musicSource{ musicNode->CreateComponent<SoundSource>() };
    musicSource->SetSoundType(SOUND_MUSIC);
    musicSource->Play(music);
}
void MasterControl::Stop()
{
    engine_->DumpResources(true);
}

void MasterControl::SubscribeToEvents()
{
    SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(MasterControl, HandleSceneUpdate));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
}

void MasterControl::CreateConsoleAndDebugHud()
{
    // Create console
    Console* console{ engine_->CreateConsole() };
    console->SetDefaultStyle(defaultStyle_);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud{ engine_->CreateDebugHud() };
    debugHud->SetDefaultStyle(defaultStyle_);
}

void MasterControl::CreateUI()
{
    ResourceCache* cache{ GetSubsystem<ResourceCache>() };
    UI* ui{ GetSubsystem<UI>() };

    //Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will control the camera
    world.cursor.uiCursor = new Cursor(context_);
    world.cursor.uiCursor->SetVisible(false);
    ui->SetCursor(world.cursor.uiCursor);

    //Set starting position of the cursor at the rendering window center
    world.cursor.uiCursor->SetPosition(graphics_->GetWidth()/2, graphics_->GetHeight()/2);

    //Construct new Text object, set string to display and font to use
    Text* instructionText{ ui->GetRoot()->CreateChild<Text>() };
    instructionText->SetText("KO");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/ChaosTimes_lig.ttf"), 32);
    //The text has multiple rows. Center them in relation to each other
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight()/2.1);
    instructionText->SetColor(Color(1.0f, 0.023f, 0.0f, 0.5f));
}

void MasterControl::LoadResources()
{
    // Get default style
    defaultStyle_ = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");

}

void MasterControl::CreateScene()
{
    world.scene = new Scene(context_);

    //Create octree, use default volume (-1000, -1000, -1000) to (1000,1000,1000)
    /*Octree* octree = */world.scene->CreateComponent<Octree>();
    //octree->SetSize(BoundingBox(Vector3(-10000, -100, -10000), Vector3(10000, 1000, 10000)), 1024);
    /*PhysicsWorld* physicsWorld = */world.scene->CreateComponent<PhysicsWorld>();
    world.scene->CreateComponent<DebugRenderer>();

    //Create cursor
    world.cursor.sceneCursor = world.scene->CreateChild("Cursor");
    world.cursor.sceneCursor->SetPosition(Vector3(0.0f,0.0f,0.0f));
    /*StaticModel* cursorObject = */world.cursor.sceneCursor->CreateComponent<StaticModel>();
    //cursorObject->SetModel(cache_->GetResource<Model>("Models/Cursor.mdl"));
    //cursorObject->SetMaterial(cache_->GetResource<Material>("Materials/glow.xml"));

    //Create an invisible plane for mouse raycasting
    world.voidNode = world.scene->CreateChild("Void");
    //Location is set in update since the plane moves with the camera.
    world.voidNode->SetScale(Vector3(1000.0f, 1.0f, 1000.0f));
    StaticModel* planeObject{ world.voidNode->CreateComponent<StaticModel>() };
    planeObject->SetModel(GetModel("Plane"));
    planeObject->SetMaterial(GetMaterial("Invisible"));

    //Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode{ world.scene->CreateChild("DirectionalLight") };
    lightNode->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
    Light* light{ lightNode->CreateComponent<Light>() };
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetBrightness(0.23f);
    light->SetColor(Color(1.0f, 0.8f, 0.7f));
    light->SetCastShadows(true);
    light->SetShadowIntensity(0.5f);
    light->SetSpecularIntensity(0.0f);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowResolution(0.23f);

    //Set cascade splits at 10, 50, 200 world unitys, fade shadows at 80% of maximum shadow distance
    light->SetShadowCascade(CascadeParameters(7.0f, 23.0f, 42.0f, 500.0f, 0.8f));

    world.ko = world.scene->CreateChild()
            ->CreateComponent<KO>();

    players_.Push(SharedPtr<Player>(new Player(1, context_)));
    INPUTMASTER->SetPlayerControl(GetPlayer(1), world.ko);

    new Dungeon(context_, this);

    Node* camNode{ world.scene->CreateChild("Camera") };
    world.camera = camNode->CreateComponent<KOCam>();
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap &eventData)
{
}

void MasterControl::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace Update;
    float timeStep{ eventData[P_TIMESTEP].GetFloat() };
    world.voidNode->SetPosition(LucKey::Scale(world.camera->GetWorldPosition(), Vector3::ONE - Vector3::UP));
    UpdateCursor(timeStep);
}

void MasterControl::UpdateCursor(double timeStep)
{
    /*world.cursor.sceneCursor->Rotate(Quaternion(0.0f,100.0f*timeStep,0.0f));
    world.cursor.sceneCursor->SetScale((world.cursor.sceneCursor->GetWorldPosition() - world.camera->GetWorldPosition()).Length()*0.05f);
    if (CursorRayCast(250.0f, world.cursor.hitResults))
    {
        for (int i = 0; i < world.cursor.hitResults.Size(); i++)
        {
            if (world.cursor.hitResults[i].node_->GetNameHash() == N_VOID)
            {
                Vector3 camHitDifference = world.camera->translationNode_->GetWorldPosition() - world.cursor.hitResults[i].position_;
                camHitDifference /= world.camera->translationNode_->GetWorldPosition().y_ - world.voidNode->GetPosition().y_;
                camHitDifference *= world.camera->translationNode_->GetWorldPosition().y_;
                world.cursor.sceneCursor->SetWorldPosition(world.camera->translationNode_->GetWorldPosition()-camHitDifference);
            }
        }
    }
    */
}

bool MasterControl::CursorRayCast(double maxDistance, PODVector<RayQueryResult> &hitResults)
{
    Ray cameraRay{ world.camera->camera_->GetScreenRay(0.5f,0.5f) };
    RayOctreeQuery query(hitResults, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    world.scene->GetComponent<Octree>()->Raycast(query);
    if (hitResults.Size()) return true;
    else return false;
}

void MasterControl::Exit()
{
    engine_->Exit();
}

void MasterControl::CreateSineLookupTable()
{
    //Generate sine lookup array
    int maxi{256};
    for (int i{0}; i < maxi; ++i){
        sine_.Push((float)sin((i/(float)maxi)*2.0f*M_PI));
    }
}

float MasterControl::Sine(float x) {
    return sine_[(int)round(sine_.Size() * LucKey::Cycle(x/M_PI, 0.0f, 1.0f))%sine_.Size()];
}

float MasterControl::Sine(float freq, float min, float max, float shift)
{
    float phase{ freq * world.scene->GetElapsedTime() + shift };
    float add{ 0.5f*(min+max) };
    return Sine(phase) * 0.5f * (max - min) + add;
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap &eventData)
{
    //world.scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}

Vector<SharedPtr<Player> > MasterControl::GetPlayers() const {
    return players_;
}

Player* MasterControl::GetPlayer(int playerID) const
{
    for (Player* p : players_) {

        if (p->GetPlayerId() == playerID)
            return p;
    }
    return nullptr;
}

Material* MasterControl::GetMaterial(String name) const
{
    return CACHE->GetResource<Material>("Materials/" + name + ".xml");
}
Model* MasterControl::GetModel(String name) const
{
    return CACHE->GetResource<Model>("Models/" + name + ".mdl");
}

Texture* MasterControl::GetTexture(String name) const
{
    return CACHE->GetResource<Texture>("Textures/" + name + ".png");
}
