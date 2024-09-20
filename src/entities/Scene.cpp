#include "Scene.h"
#include "Logger.h"
#include "assets/SceneAsset.h"
#include "Node.h"

char* Scene::Name = nullptr;
std::vector<Node*> Scene::Nodes = {};

void Scene::Update(const float_t timeDelta)
{
    if (!Nodes.size())
        return;

    for (size_t i = 0; i < Nodes.size(); i++)
    {
        if (Nodes[i])
            Nodes[i]->UpdateLogic();
    }
}

void Scene::Render(SDL_Renderer* renderer, const float_t timeDelta)
{
    if (!Nodes.size())
        return;

    for (size_t i = 0; i < Nodes.size(); i++)
    {
        if (Nodes[i])
            Nodes[i]->Render();
    }
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
