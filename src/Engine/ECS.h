#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>

using Entity = uint32_t;
const Entity INVALID_ENTITY = 0;

class ISystem;

class ECS {
public:
  ECS();
  ~ECS() = default;

  // Entity Management
  Entity CreateEntity();
  void DestroyEntity(Entity _iEntity);

  // Component Management
  template<typename C>
  void AddComponent(Entity _iEntity, const C& _rComponent);

  template<typename C, typename... Args>
  void EmplaceComponent(Entity _iEntity, Args&&... _rArgs);

  template<typename C>
  bool HasComponent(Entity _iEntity) const;

  template<typename C>
  C* GetComponent(Entity _iEntity);

  // Storage Access
  template<typename C>
  std::unordered_map<Entity, C>& Storage();

private:
  // Interface for component storage, allowing for type-erased storage management.
  struct IStorage {
    virtual ~IStorage() = default;
    virtual void EraseEntity(Entity e) = 0;
  };

  // Concrete storage implementation for components of type C, using an unordered map to associate entities with their components.
  template<typename C>
  struct StorageImpl : IStorage {
    std::unordered_map<Entity, C> mMap;
    void EraseEntity(Entity e) override { mMap.erase(e); }
  };

  // Next entity ID to be assigned, incremented with each new entity creation to ensure unique identifiers.
  Entity m_iNext = 1;
  // Map associating component type indices with their corresponding storage implementations.
  std::unordered_map<std::type_index, std::unique_ptr<IStorage>> m_mStorages;
};