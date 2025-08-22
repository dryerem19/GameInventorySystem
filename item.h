#ifndef ITEM_H
#define ITEM_H

#include <cstdint>
#include <string>

namespace Inventory {
    class Item
    {
    public:
        Item(uint32_t id = 0, const std::string& name = std::string(),
             const std::string& description = std::string(), float weight = 0.0f)
            : id_(id)
            , name_(name)
            , description_(description)
            , weight_(weight) {}

        virtual ~Item() = default;

        void setName(const std::string& name) {
            name_ = name;
        }

        void setDescription(const std::string& description) {
            description_ = description;
        }

        void setWeight(float weight) {
            weight_ = weight;
        }

        inline uint32_t             id()            const { return id_; }
        inline const std::string    name()          const { return name_; }
        inline const std::string    description()   const { return description_; }
        inline float                weight()        const { return weight_; }
        inline uint32_t             stackCount()    const { return stackCount_; }

        bool canStackWith(const Item* other) const {
            return id_ == other->id_;
        }

        void addToStack(uint32_t count = 1) {
            stackCount_ += count;
        }

        void removeFromStack(uint32_t count = 1) {
            stackCount_ -= count;
        }

        void removeAllFromStack() {
            stackCount_ = 0;
        }

    protected:
        std::string name_       {};
        std::string description_{};
        float       weight_     {0};
        uint32_t    id_         {0};
        uint32_t    stackCount_ {1};
    };
}
#endif // ITEM_H
