#include "storage.h"
#include <algorithm>

namespace Inventory {
    Storage::Storage(int rows, int cols, float maxWeight)
    : rows_(rows)
    , cols_(cols)
    , maxWeight_(maxWeight)
    , currentWeight_(0.0f) {
        items_.resize(rows_ * cols_);
    }

    bool Storage::hasItem(uint32_t id) const
    {
        return std::any_of(items_.begin(), items_.end(),
                           [id](const std::unique_ptr<Item>& ptr) { return ptr->id() == id; });
    }

    Item *Storage::findItemById(uint32_t id) const
    {
        auto it = std::find_if(items_.begin(), items_.end(),
                               [id](const std::unique_ptr<Item>& ptr) { return ptr->id() == id; });

        return it != items_.end() ? it->get() : nullptr;
    }

    Item *Storage::getItem(int row, int col) const
    {
        if (!isValidPosition(row, col)) return nullptr;
        return items_[row * cols_ + col].get();
    }

    int Storage::getFreeCell() const
    {
        for (int row = 0; row < rows_; row++) {
            for (int col = 0; col < cols_; col++)
            {
                int index = isFreeCell(row, col);
                if (index >= 0) {
                    return index;
                }
            }
        }

        return -1;
    }

    int Storage::isFreeCell(int row, int col) const
    {
        if (!isValidPosition(row, col)) {
            return -1;
        }

        int index = row * cols_ + col;
        if (!items_[index]) {
            return index;
        }

        return -1;
    }

    bool Storage::isValidPosition(int row, int col) const
    {
        return row >= 0 && row < rows_ && col >= 0 && col < cols_;
    }

    Storage::Error Storage::canAddItem(const Item *item) const
    {
        if (!item) {
            return Error::InvalidItem;
        }

        if (maxWeight_ > 0 && currentWeight_ + item->weight() > maxWeight_) {
            return Error::NoSpace;
        }

        return Error::Success;
    }

    Storage::Error Storage::addItem(std::unique_ptr<Item> item)
    {
        auto error = canAddItem(item.get());
        if (error != Error::Success) {
            return error;
        }

        bool isSpaceLimited = maxWeight_ > 0;
        bool isSpaceWillBeExceeded = currentWeight_ + item->weight() > maxWeight_;

        if (isSpaceLimited && isSpaceWillBeExceeded) {
            return Error::NoSpace;
        }

        for (int row = 0; row < rows_; row++) {
            for (int col = 0; col < cols_; col++)
            {
                int index = row * cols_ + col;
                auto existingItem = items_[index].get();
                if (!existingItem) {
                    continue;
                }

                if (existingItem->canStackWith(item.get())) {
                    existingItem->addToStack(item->stackCount());
                    currentWeight_ += item->weight();
                    return Error::Success;
                }
            }
        }

        int index = getFreeCell();
        if (index >= 0)
        {
            currentWeight_ += item->weight();
            items_[index] = std::move(item);
            return Error::Success;
        }

        return Error::NoSpace;
    }

    void Storage::clear()
    {
        items_.clear();
        currentWeight_ = 0.0f;
    }

    std::unique_ptr<Item> Storage::removeItem(Item *item)
    {
        auto it = std::find_if(items_.begin(), items_.end(),
                               [item](const std::unique_ptr<Item>& ptr) { return ptr.get() == item; });

        if (it != items_.end())
        {
            auto result = std::move(*it);
            currentWeight_ -= result->weight();

            items_.erase(it);
            return result;
        }

        return nullptr;
    }

    std::unique_ptr<Item> Storage::removeItemById(uint32_t id)
    {
        auto it = std::find_if(items_.begin(), items_.end(),
                               [id](const std::unique_ptr<Item>& ptr) { return ptr->id() == id; });

        if (it != items_.end())
        {
            auto result = std::move(*it);
            currentWeight_ -= result->weight();

            items_.erase(it);
            return result;
        }

        return nullptr;
    }
}
