/*
* File: DebugUI.cpp
* Purpose: a complete wrapper around Dear ImGUI to visualise and assist with debugging.
*/
#include "DebugUI.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

//  This namespace serves rendering debug stuff.
namespace DebugUI
{
    static ImGuiIO* gIO = nullptr;
    static SDL_Renderer* gRenderer = nullptr;
    static std::vector<Panel>   Panels = {};

    Panel::~Panel()
    {
        if (!Items.size())
            return;

        for (auto item : Items)
        {
            delete item;
        }
    }

    void Panel::Render()
    {
        ImGui::Begin(Title.c_str());

        //  Go through all items in this panel and put them here.
        for (auto item : Items)
        {
            switch (item->GetType())
            {
                case Item::ItemType::TEXT:
                    ImGui::Text(reinterpret_cast<TextItem*>(item)->GetTitle().c_str());
                    break;
                case Item::ItemType::BUTTON:
                    ImGui::Button(reinterpret_cast<ButtonItem*>(item)->GetTitle().c_str());
                    break;
                case Item::ItemType::SLIDER:
                    auto sliderRef = reinterpret_cast<SliderItem*>(item);
                    const auto sliderType = sliderRef->GetSliderType();
                    const auto sliderElementsCount = sliderRef->GetNumberOfItems();

                    switch (sliderType)
                    {
                        case SliderItem::FLOAT:
                        {
                            auto sliderFloatRef = reinterpret_cast<SliderItemFloat*>(sliderRef);

                            switch (sliderElementsCount)
                            {
                            case 1:
                                ImGui::SliderFloat(sliderRef->GetTitle().c_str(), sliderFloatRef->GetItems(), sliderFloatRef->GetMin(), sliderFloatRef->GetMax());
                                break;
                            case 2:
                                ImGui::SliderFloat2(sliderRef->GetTitle().c_str(), sliderFloatRef->GetItems(), sliderFloatRef->GetMin(), sliderFloatRef->GetMax());
                                break;
                            case 3:
                                ImGui::SliderFloat3(sliderRef->GetTitle().c_str(), sliderFloatRef->GetItems(), sliderFloatRef->GetMin(), sliderFloatRef->GetMax());
                                break;
                            case 4:
                                ImGui::SliderFloat4(sliderRef->GetTitle().c_str(), sliderFloatRef->GetItems(), sliderFloatRef->GetMin(), sliderFloatRef->GetMax());
                                break;
                            }

                            break;
                        }
                        case SliderItem::INT:
                        {
                            auto sliderIntRef = reinterpret_cast<SliderItemInt*>(sliderRef);

                            switch (sliderElementsCount)
                            {
                            case 1:
                                ImGui::SliderInt(sliderRef->GetTitle().c_str(), sliderIntRef->GetItems(), sliderIntRef->GetMin(), sliderIntRef->GetMax());
                                break;
                            case 2:
                                ImGui::SliderInt2(sliderRef->GetTitle().c_str(), sliderIntRef->GetItems(), sliderIntRef->GetMin(), sliderIntRef->GetMax());
                                break;
                            case 3:
                                ImGui::SliderInt3(sliderRef->GetTitle().c_str(), sliderIntRef->GetItems(), sliderIntRef->GetMin(), sliderIntRef->GetMax());
                                break;
                            case 4:
                                ImGui::SliderInt4(sliderRef->GetTitle().c_str(), sliderIntRef->GetItems(), sliderIntRef->GetMin(), sliderIntRef->GetMax());
                                break;
                            }

                            break;
                        }
                        default:
                            break;
                    }
                    break;
            }
        }

        ImGui::End();
    }


    void AddPanel(const std::string& panelName)
    {
        Panels.push_back({ panelName });
    }

    void RemovePanel(const std::string& panelName)
    {
        auto it = std::remove_if(Panels.begin(), Panels.end(), [&](const Panel& panel) { return panel.GetTitle() == panelName; });
        Panels.erase(it);
    }

    void AddPanelItem(const std::string& panelName, const Item::ItemType type, const void* data)
    {
        auto panelRef = std::find_if(Panels.begin(), Panels.end(), [&](const Panel& panel) { return panel.GetTitle() == panelName; });
        if (panelRef == Panels.end())
            return;

        auto& panelItemsRef = panelRef->GetItems();
        switch (type)
        {
            case Item::ItemType::TEXT:
                {
                    const auto textRef = reinterpret_cast<const TextItem::TextData*>(data);
                    panelItemsRef.emplace_back(new TextItem(textRef->Title));
                    break;
                }
            case Item::ItemType::BUTTON:
                {
                    const auto buttonRef = reinterpret_cast<const ButtonItem::ButtonData*>(data);
                    panelItemsRef.emplace_back(new ButtonItem(buttonRef->Title));
                    break;
                }
            case Item::ItemType::SLIDER:
                {
                    const auto sliderType = (SliderItem::ItemType)(*reinterpret_cast<const uint32_t*>(data));
                    switch (sliderType)
                    {
                    case SliderItem::ItemType::INT:
                        {
                            const auto sliderRef = reinterpret_cast<const SliderItem::SliderData<int32_t>*>(data);
                            panelItemsRef.emplace_back(new SliderItemInt(sliderRef->Title, sliderRef->Size, sliderRef->Items, sliderRef->Min, sliderRef->Max));
                            break;
                        }
                        case SliderItem::ItemType::FLOAT:
                        {
                            const auto sliderRef = reinterpret_cast<const SliderItem::SliderData<float_t>*>(data);
                            panelItemsRef.emplace_back(new SliderItemFloat(sliderRef->Title, sliderRef->Size, sliderRef->Items, sliderRef->Min, sliderRef->Max));
                            break;
                        }
                    }

                    break;
                }
            default:
                break;
        }
    }

    bool Init(SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        gIO = &ImGui::GetIO();

        ImGui::StyleColorsLight();
        if (!ImGui_ImplSDL3_InitForSDLRenderer(SDLWindow, SDLRenderer))
            return false;

        if (!ImGui_ImplSDLRenderer3_Init(SDLRenderer))
            return false;

        gRenderer = SDLRenderer;

        return true;
    }

    void Update(const float_t delta)
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //  Go through all panels and put them here.
        for (auto& panel : Panels)
        {
            panel.Render();
        }

        ImGui::Render();

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), gRenderer);
    }

    void UnInit()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
}