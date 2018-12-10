#include "TransformSystem.h"
#include "../components/Transform.h"
#include "glm/gtx/string_cast.hpp"

namespace rcube {

void updateHierarchy(Transform *comp, bool force=false) {
    if (comp->dirty() || force) {
        glm::mat4 loc_tr = glm::toMat4(comp->orientation());
        loc_tr = glm::scale(loc_tr, comp->scale());
        loc_tr = glm::translate(loc_tr, comp->position());
        comp->setLocalTransform(loc_tr);

        // Set world transformation := local transformation if there is no parent
        if (comp->parent() == nullptr) {
            comp->setWorldTransform(comp->localTransform());
        }
        else {
            comp->setWorldTransform(comp->parent()->worldTransform() * comp->localTransform());
        }
        comp->setDirty(false);
        for (auto child : comp->children()) {
            updateHierarchy(child, true);
        }
    }
}

TransformSystem::TransformSystem() {
    ComponentMask transform_filter;
    transform_filter.set(Transform::family());
    addFilter(transform_filter);
}

void TransformSystem::initialize() {
}

void TransformSystem::cleanup() {
}
void TransformSystem::update(bool force) {
    const auto &transformable_entities = registered_entities_[filters_[0]];
    for (const Entity &ent : transformable_entities) {
        Transform *comp = world_->getComponent<Transform>(ent);
        // Update hierarchy from root level nodes which do not have a parent
        if (comp->parent() == nullptr) {
            updateHierarchy(comp, force);
        }
    }
}

} // namespace rcube