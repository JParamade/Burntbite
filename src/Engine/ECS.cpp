#include "ECS.h"

#include "../Game/Systems.h"
#include "../Game/Components.h"

#include <cstdint>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <vector>
#include <memory>
#include <stdexcept>

ECS::ECS() {}

Entity ECS::CreateEntity() {
  return m_iNext++;
}

void ECS::DestroyEntity(Entity _iEntity) {
  // If entity is invalid, do nothing.
  if (_iEntity == INVALID_ENTITY) return;

  // Remove entity from all storages.
  for (auto& rKeyValue : m_mStorages) {
    rKeyValue.second->EraseEntity(_iEntity);
  }
}

template<typename C>
void ECS::AddComponent(Entity _iEntity, const C& _rComponent) {
  // Add component to storage for type C, creating storage if it doesn't exist.
  Storage<C>()[_iEntity] = _rComponent;
}

template<typename C, typename... Args>
void ECS::EmplaceComponent(Entity _iEntity, Args&&... _rArgs) {
  // Emplace component in storage for type C, creating storage if it doesn't exist.
  Storage<C>().emplace(std::piecewise_construct,
    std::forward_as_tuple(_iEntity),
    std::forward_as_tuple(std::forward<Args>(_rArgs)...));
}

template<typename C>
bool ECS::HasComponent(Entity _iEntity) const {
  auto oIdx = std::type_index(typeid(C));
  
  // Check if storage for component type C exists.
  auto oIterator = m_mStorages.find(oIdx);
  // If storage doesn't exist, then entity can't have component of type C.
  if (oIterator == m_mStorages.end()) return false;
  
  // Check if entity has component of type C in storage.
  auto* pStorageImpl = static_cast<StorageImpl<C>*>(oIterator->second.get());
  return pStorageImpl->mMap.find(_iEntity) != pStorageImpl->mMap.end();
}

template<typename C>
C* ECS::GetComponent(Entity _iEntity) {
  auto oIdx = std::type_index(typeid(C));
  
  // Check if storage for component type C exists.
  auto oIterator = m_mStorages.find(oIdx);
  // If storage doesn't exist, then entity can't have component of type C.
  if (oIterator == m_mStorages.end()) return nullptr;
  
  // Check if entity has component of type C in storage.
  auto* pStorageImpl = static_cast<StorageImpl<C>*>(oIterator->second.get());
  auto oIterator2 = pStorageImpl->mMap.find(_iEntity);
  // If entity doesn't have component of type C, return nullptr.
  if (oIterator2 == pStorageImpl->mMap.end()) return nullptr;
  
  // Return pointer to component of type C for entity.
  return &oIterator2->second;
}

template<typename C>
std::unordered_map<Entity, C>& ECS::Storage() {
  auto oIdx = std::type_index(typeid(C));

  // Check if storage for component type C exists.
  auto oIterator = m_mStorages.find(oIdx);
  // If storage doesn't exist, create it and return reference to new storage map.
  if (oIterator == m_mStorages.end()) {
    auto pStorageImpl = std::make_unique<StorageImpl<C>>();

    auto* pRaw = pStorageImpl.get();
    m_mStorages[oIdx] = std::move(pStorageImpl);
    
    return pRaw->mMap;
  }

  // Storage for component type C already exists, return reference to existing storage map.
  return static_cast<StorageImpl<C>*>(oIterator->second.get())->mMap;

}

// ---------------- AddComponent ----------------
template void ECS::AddComponent<Transform>(Entity, const Transform&);
template void ECS::AddComponent<Velocity>(Entity, const Velocity&);
template void ECS::AddComponent<SpriteComponent>(Entity, const SpriteComponent&);
template void ECS::AddComponent<PlayerTag>(Entity, const PlayerTag&);
template void ECS::AddComponent<EnemyTag>(Entity, const EnemyTag&);
template void ECS::AddComponent<Collider>(Entity, const Collider&);
template void ECS::AddComponent<Lifetime>(Entity, const Lifetime&);
template void ECS::AddComponent<SpawnTimer>(Entity, const SpawnTimer&);
template void ECS::AddComponent<Score>(Entity, const Score&);

// ---------------- EmplaceComponent ----------------
template void ECS::EmplaceComponent<Transform>(Entity, const Transform&);
template void ECS::EmplaceComponent<Velocity>(Entity, const Velocity&);
template void ECS::EmplaceComponent<SpriteComponent, const std::string&>(Entity, const std::string&);
template void ECS::EmplaceComponent<PlayerTag>(Entity, const PlayerTag&);
template void ECS::EmplaceComponent<EnemyTag>(Entity, const EnemyTag&);
template void ECS::EmplaceComponent<Collider>(Entity, const Collider&);
template void ECS::EmplaceComponent<Lifetime>(Entity, const Lifetime&);
template void ECS::EmplaceComponent<SpawnTimer>(Entity, const SpawnTimer&);
template void ECS::EmplaceComponent<Score>(Entity, const Score&);

// ---------------- GetComponent ----------------
template Transform* ECS::GetComponent<Transform>(Entity);
template Velocity* ECS::GetComponent<Velocity>(Entity);
template SpriteComponent* ECS::GetComponent<SpriteComponent>(Entity);
template PlayerTag* ECS::GetComponent<PlayerTag>(Entity);
template EnemyTag* ECS::GetComponent<EnemyTag>(Entity);
template Collider* ECS::GetComponent<Collider>(Entity);
template Lifetime* ECS::GetComponent<Lifetime>(Entity);
template SpawnTimer* ECS::GetComponent<SpawnTimer>(Entity);
template Score* ECS::GetComponent<Score>(Entity);

// ---------------- HasComponent ----------------
template bool ECS::HasComponent<Transform>(Entity) const;
template bool ECS::HasComponent<Velocity>(Entity) const;
template bool ECS::HasComponent<SpriteComponent>(Entity) const;
template bool ECS::HasComponent<PlayerTag>(Entity) const;
template bool ECS::HasComponent<EnemyTag>(Entity) const;
template bool ECS::HasComponent<Collider>(Entity) const;
template bool ECS::HasComponent<Lifetime>(Entity) const;
template bool ECS::HasComponent<SpawnTimer>(Entity) const;
template bool ECS::HasComponent<Score>(Entity) const;

// ---------------- Storage ----------------
template std::unordered_map<Entity, Transform>& ECS::Storage<Transform>();
template std::unordered_map<Entity, Velocity>& ECS::Storage<Velocity>();
template std::unordered_map<Entity, SpriteComponent>& ECS::Storage<SpriteComponent>();
template std::unordered_map<Entity, PlayerTag>& ECS::Storage<PlayerTag>();
template std::unordered_map<Entity, EnemyTag>& ECS::Storage<EnemyTag>();
template std::unordered_map<Entity, Collider>& ECS::Storage<Collider>();
template std::unordered_map<Entity, Lifetime>& ECS::Storage<Lifetime>();
template std::unordered_map<Entity, SpawnTimer>& ECS::Storage<SpawnTimer>();
template std::unordered_map<Entity, Score>& ECS::Storage<Score>();