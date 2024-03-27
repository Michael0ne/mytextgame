
namespace DebugUI
{
    class Item
    {
    public:
        enum ItemType
        {
            TEXT,
            BUTTON,
            SLIDER,
        };

    protected:
        ItemType    Type;
        std::string Name;

    public:
        inline const ItemType  GetType() const { return Type; }
        virtual inline const std::string& GetTitle() const = 0;
    };

    class TextItem : public Item
    {
    public:
        struct TextData
        {
            std::string Title;
        };

    protected:
        std::string     Title;

    public:
        virtual inline const std::string& GetTitle() const override { return Title; }

        inline TextItem(const std::string& title)
        {
            Type = TEXT;
            Title = title;
        }
    };

    class ButtonItem : public Item
    {
    public:
        struct ButtonData
        {
            std::string Title;
        };

    protected:
        std::string     Title;

    public:
        virtual inline const std::string& GetTitle() const override { return Title; }

        inline ButtonItem(const std::string& title)
        {
            Type = BUTTON;
            Title = title;
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
        };

    protected:
        uint32_t    NumberOfItems = 0;
        ItemType    Type = INT;
        std::string Title;

        SliderItem() = default;

    public:
        inline const auto GetNumberOfItems() const { return NumberOfItems; }
        inline const auto GetSliderType() const { return Type; }
        virtual inline const std::string& GetTitle() const override { return Title; }
    };

    class SliderItemInt : public SliderItem
    {
    protected:
        int32_t*        Items;
        int32_t         Min = -1;
        int32_t         Max = 1;

    public:
        inline SliderItemInt(const std::string& title, const uint32_t itemsCount, int32_t* items, const int32_t min = -1, const int32_t max = 1) { Item::Type = SLIDER; NumberOfItems = itemsCount; Title = title; Type = INT; Items = items; Min = min; Max = max; }

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
        inline SliderItemFloat(const std::string& title, const uint32_t itemsCount, float_t* items, const float_t min = -1.f, const float_t max = 1.f) { Item::Type = SLIDER; NumberOfItems = itemsCount; Title = title; Type = FLOAT; Items = items; Min = min; Max = max; }

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
        inline Panel(const std::string& title, std::tuple<uint32_t, uint32_t> position, std::tuple<uint32_t, uint32_t> dimensions)
        {
            Title = title;
            Position = position;
            Dimensions = dimensions;
            Items = {};
        }

        inline Panel(const std::string& title)
        {
            Title = title;
            Position = { 0, 0 };
            Dimensions = { 0, 0 };
            Items = {};
        }

        ~Panel();

        inline const auto GetTitle() const { return Title; }
        inline auto& GetItems() { return Items; }

        void Render();
    };

    extern void AddPanel(const std::string& panelName);
    extern void RemovePanel(const std::string& panelName);
    extern void AddPanelItem(const std::string& panelName, const Item::ItemType type, const void* data = nullptr);

    extern bool Init(SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer);
    extern void Update(const float_t delta);
    extern void UnInit();
}