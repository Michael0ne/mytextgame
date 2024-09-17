#include "Scene.h"

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
    DebugUI::AddPanel("Scene");

    DebugUI::AddPanelItem("Scene", DebugUI::Item::TEXT, DebugUI::TextItem::TextData("Scene properties"));
    DebugUI::AddPanelItem("Scene", DebugUI::Item::TEXT, DebugUI::TextItem::TextData("Scene name: "));

    return true;
}