#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <memory>
#include "item.h"

namespace Inventory {
    class Storage
    {
    public:
        enum class Error
        {
            Success,
            NoSpace,
            InvalidItem,
            ItemNotFound,
        };

        explicit Storage(float maxWeight = -1.0f);

        inline float maxWeight()        const { return maxWeight_; }
        inline float currentWeight()    const { return currentWeight_; }

        bool    hasItem(uint32_t id)                const;
        Item*   findItemById(uint32_t id)           const;
        Error   canAddItem(const Item* item)        const;
        Error   addItem(std::unique_ptr<Item> item);

        void                    clear();
        std::unique_ptr<Item>   removeItem(Item* item);
        std::unique_ptr<Item>   removeItemById(uint32_t id);

    private:
        using Container = std::vector<std::unique_ptr<Item>>;

        float       maxWeight_;
        float       currentWeight_;
        Container   items_;
    };
}

#endif // STORAGE_H
