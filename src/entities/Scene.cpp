#include "Scene.h"
#include "Logger.h"
#include "assets/SceneAsset.h"

char* Scene::Name = nullptr;

void Scene::Update(const float_t timeDelta)
{
    //  TODO: update scene children.
}

void Scene::Render(SDL_Renderer* renderer, const float_t timeDelta)
{
    //  TODO: call render method on every scene child.
}

bool Scene::Init()
{
    if (!SceneAsset::ActiveScene.empty())
        Scene::Name = SDL_strdup(SceneAsset::ActiveScene.c_str());

    DebugUI::AddPanel("Scene");

    DebugUI::AddPanelItem("Scene", DebugUI::Item::TEXT, DebugUI::TextItem::TextData("Scene properties"));
    DebugUI::AddPanelItem("Scene", DebugUI::Item::TEXT_RAW, DebugUI::TextItem::TextData("Scene name: ", Scene::Name));

    return true;
}

void Scene::Shutdown()
{
    if (Scene::Name)
        delete Scene::Name;
}
