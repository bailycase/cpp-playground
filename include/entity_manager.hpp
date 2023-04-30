#pragma once

#include "sparse_set.hpp"
#include <any>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
#include "entt.hpp"


using Entity = entt::entity;

class EntityManager {
  public:
  Entity create() {
    return INTERNAL_ECS.create();
  }
  
  template<typename Type, typename... Args>
  auto &add(const Entity entity, Args &&...args) {
    return INTERNAL_ECS.emplace<Type>(entity, args...);
  }

  entt::registry INTERNAL_ECS;
};
