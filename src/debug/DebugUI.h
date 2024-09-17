/*
* File: DebugUI.h
* Purpose: a complete wrapper around Dear ImGUI to visualise and assist with debugging.
*/
#include "Generic.h"
#include "Logger.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

namespace DebugUI
{
    inline ImGuiIO* gIO = nullptr;
    inline SDL_Renderer* gRenderer = nullptr;

    class Item
    {
    public:
        enum ItemType
        {
            UNSPECIFIED,
            TEXT,
            BUTTON,
            SLIDER,
        };

    protected:
        ItemType    Type = UNSPECIFIED;
        std::string Name;

    public:
        virtual ~Item()
        {
        }

        inline const ItemType  GetType() const { return Type; }
        virtual inline const std::string& GetTitle() const = 0;

        Item()
        {
        }
    };

    class TextItem : public Item
    {
    public:
        struct TextData
        {
            std::string Title;

            TextData(std::string s)
                :Title(std::move(s))
            {
            };

            ~TextData()
            {
            }
        };

    protected:
        std::string     Title;

    public:
        virtual ~TextItem() override = default;
        virtual inline const std::string& GetTitle() const override { return Title; }

        inline TextItem(std::string title)
            :Title(std::move(title))
        {
            Type = TEXT;
        }
    };

    class ButtonItem : public Item
    {
    public:
        struct ButtonData
        {
            std::string Title;

            ButtonData(std::string s)
                :Title(std::move(s))
            {};
        };

    protected:
        std::string     Title;

    public:
        virtual ~ButtonItem() override = default;
        virtual inline const std::string& GetTitle() const override { return Title; }

        inline ButtonItem(std::string title)
            :Title(std::move(title))
        {
            Type = BUTTON;
        }
    };

    class SliderItem : public Item
    {
    public:
        enum ItemType
        {
            INT,
            FLOAT,
        };

        template<typename T>
        struct SliderData
        {
            SliderItem::ItemType    Type;
            std::string             Title;
            uint32_t                Size;
            T*                      Items;
            T                       Min;
            T                       Max;

            SliderData(SliderItem::ItemType t, std::string ts, uint32_t s, T* i, T mi, T ma)
                :Type(t), Title(std::move(ts)), Size(s), Items(i), Min(mi), Max(ma)
            {};
        };

    protected:
        uint32_t    NumberOfItems = 0;
        ItemType    Type = INT;
        std::string SliderTitle;

        SliderItem() = default;

    public:
        inline const auto GetNumberOfItems() const { return NumberOfItems; }
        inline const auto GetSliderType() const { return Type; }
        virtual inline const std::string& GetTitle() const override { return SliderTitle; }
    };

    class SliderItemInt : public SliderItem
    {
    protected:
        int32_t*        Items;
        int32_t         Min = -1;
        int32_t         Max = 1;

    public:
        inline SliderItemInt(std::string title, const uint32_t itemsCount, int32_t* items, const int32_t min = -1, const int32_t max = 1)
        {
            SliderTitle = title;
            Item::Type = SLIDER;
            NumberOfItems = itemsCount;
            Type = INT;
            Items = items;
            Min = min;
            Max = max;
        }

        inline int32_t* GetItems() { return Items; }
        inline const int32_t GetMin() const { return Min; }
        inline const int32_t GetMax() const { return Max; }
    };

    class SliderItemFloat : public SliderItem
    {
    protected:
        float_t*        Items;
        float_t         Min = -1.f;
        float_t         Max = 1.f;

    public:
        inline SliderItemFloat(std::string title, const uint32_t itemsCount, float_t* items, const float_t min = -1.f, const float_t max = 1.f)
        {
            SliderTitle = title;
            Item::Type = SLIDER;
            NumberOfItems = itemsCount;
            Type = FLOAT;
            Items = items;
            Min = min;
            Max = max;
        }

        inline float_t* GetItems() { return Items; }
        inline const float_t GetMin() const { return Min; }
        inline const float_t GetMax() const { return Max; }
    };

    class Panel
    {
    public:
        std::string                         Title;
        std::tuple<uint32_t, uint32_t>      Position;
        std::tuple<uint32_t, uint32_t>      Dimensions;
        std::list<Item*>                    Items;

    public:
        inline Panel(std::string title, std::tuple<uint32_t, uint32_t> position, std::tuple<uint32_t, uint32_t> dimensions)
        {
            Title = std::move(title);
            Position = position;
            Dimensions = dimensions;
            Items = {};
        }

        inline Panel(std::string title)
        {
            Title = std::move(title);
            Position = { 0, 0 };
            Dimensions = { 0, 0 };
            Items = {};
        }

        ~Panel()
        {
            if (!Items.size())
                return;

            Items.clear();
        }

        inline const auto GetTitle() const { return Title; }
        inline auto& GetItems() { return Items; }

        inline void Render()
        {
            ImGui::Begin(Title.c_str());

            //  Go through all items in this panel and put them here.
            for (auto item : Items)
            {
                switch (item->GetType())
                {
                case Item::ItemType::TEXT:
                {
                    const auto itemText = reinterpret_cast<TextItem*>(item);
                    assert(itemText);

                    ImGui::Text(itemText->GetTitle().c_str());
                    break;
                }
                case Item::ItemType::BUTTON:
                {
                    const auto itemButton = reinterpret_cast<ButtonItem*>(item);
                    assert(itemButton);

                    ImGui::Button(itemButton->GetTitle().c_str());
                    break;
                }
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
    };

    inline std::vector<Panel>   Panels = {};

    template<typename T>
    inline void AddPanelItem(const std::string panelName, Item::ItemType type, T data)
    {
        auto panelRef = std::find_if(Panels.begin(), Panels.end(), [&](const Panel& panel) { return panel.GetTitle() == panelName; });
        if (panelRef == Panels.end())
            return;

        auto& panelItemsRef = panelRef->GetItems();
        switch (type)
        {
        case Item::ItemType::TEXT:
        {
            const auto& textRef = reinterpret_cast<const TextItem::TextData&>(data);
            panelItemsRef.emplace_back(new TextItem(textRef.Title));
            break;
        }
        case Item::ItemType::BUTTON:
        {
            const auto& buttonRef = reinterpret_cast<const ButtonItem::ButtonData&>(data);
            panelItemsRef.emplace_back(new ButtonItem(buttonRef.Title));
            break;
        }
        case Item::ItemType::SLIDER:
        {
            const auto sliderType = (SliderItem::ItemType)(reinterpret_cast<const uint32_t&>(data));
            switch (sliderType)
            {
            case SliderItem::ItemType::INT:
            {
                const auto& sliderRef = reinterpret_cast<const SliderItem::SliderData<int32_t>&>(data);
                panelItemsRef.emplace_back(new SliderItemInt(sliderRef.Title, sliderRef.Size, sliderRef.Items, sliderRef.Min, sliderRef.Max));
                break;
            }
            case SliderItem::ItemType::FLOAT:
            {
                const auto& sliderRef = reinterpret_cast<const SliderItem::SliderData<float_t>&>(data);
                panelItemsRef.emplace_back(new SliderItemFloat(sliderRef.Title, sliderRef.Size, sliderRef.Items, sliderRef.Min, sliderRef.Max));
                break;
            }
            }

            break;
        }
        default:
            break;
        }
    }

    inline void AddPanel(const std::string& panelName)
    {
        Panels.emplace_back(panelName);
    }

    inline void RemovePanel(const std::string& panelName)
    {
        auto it = std::remove_if(Panels.begin(), Panels.end(), [&](const Panel& panel) { return panel.GetTitle() == panelName; });
        Panels.erase(it);
    }

    inline bool Init(SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer)
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

    inline void Update(const float_t delta)
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

    inline void UnInit()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
}