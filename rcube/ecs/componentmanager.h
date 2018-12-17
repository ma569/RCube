#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <map>
#include <array>
#include <vector>
#include "entity.h"

class BaseComponentManager {
public:
    virtual ~BaseComponentManager() = default;
};

template <typename T>
class ComponentManager : public BaseComponentManager {
public:
    typedef unsigned int ComponentIndex;
    virtual ~ComponentManager() = default;
    ComponentManager() {
        component_data_.data.resize(1024);
    }
    void add(Entity e, const T &component) {
        ComponentIndex new_index = ComponentIndex{ component_data_.size };
        component_data_.size += 1;
        entity_map_[e] = new_index;
        component_data_.data[new_index] = component;
        // return new_index;
    }
    void remove(Entity e) {
        if (entity_map_.find(e) == entity_map_.end()) {
            return;
        }
        ComponentIndex to_remove = entity_map_[e];
        component_data_.data[to_remove] = component_data_.data[component_data_.size - 1];
        entity_map_.erase(e);
        component_data_.size -= 1;
    }
    void clear() {
        entity_map_.clear();
        component_data_.data.clear();
        component_data_.size = 0;
    }
    T * get(Entity e) {
        if (entity_map_.find(e) == entity_map_.end()) {
            throw std::runtime_error("Entity does not have requested component");
        }
        return &(component_data_.data[entity_map_[e]]);
    }

private:
    struct ComponentData {
        unsigned int size = 1;
        //std::array<T, 1024> data;
        std::vector<T> data;
    };
    std::map<Entity, ComponentIndex> entity_map_;
    ComponentData component_data_;
};



#endif // COMPONENTMANAGER_H
