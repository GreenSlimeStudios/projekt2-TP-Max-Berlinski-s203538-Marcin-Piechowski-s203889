#pragma once
#include <stack>
#include <vector>
#include <stdexcept>
#include <algorithm>

struct Item {
    std::string id;
    float weight;
};

class CarStack {
private:
    std::stack<Item> items;
    float capacity;
    float current_load;

public:
    CarStack(float cap) : capacity(cap), current_load(0.0f) {
        if (cap <= 0) throw std::invalid_argument("Capacity must be positive");
    }

    bool canAdd(float weight) const {
        return (current_load + weight) <= capacity;
    }

    bool push(const Item& item) {
        if (canAdd(item.weight)) {
            items.push(item);
            current_load += item.weight;
            return true;
        }
        return false;
    }

    std::vector<Item> getItems() const {
        std::vector<Item> result;
        auto temp = items;
        
        while (!temp.empty()) {
            result.push_back(temp.top());
            temp.pop();
        }
        
        std::reverse(result.begin(), result.end());
        return result;
    }

    float getCapacity() const { return capacity; }
    float getCurrentLoad() const { return current_load; }
};