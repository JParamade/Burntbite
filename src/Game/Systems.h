#pragma once

#include "../Engine/ECS.h"
#include "../Engine.h"
#include "Components.h"
#include <memory>
#include <unordered_map>
#include <string>

class ISystem {
public:
  virtual ~ISystem() = default;
  virtual void Update(ECS& _rEcs, float _fDeltaTime) = 0;
};

class GameState;

class InputSystem : public ISystem {
public:
  InputSystem(CEngine* _pEngine, GameState* _pGameState);
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  CEngine* m_pEngine;
  GameState* m_pGameState;
};


class MovementSystem : public ISystem {
public:
  MovementSystem(CEngine* _pEngine);
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  const float m_fGravity = 400.0f;
  
  CEngine* m_pEngine;
  int m_iWindowWidth = 500;
  int m_iWindowHeight = 500;
};

class SpawnSystem : public ISystem {
public:
  SpawnSystem(GameState* _pGameState);
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  GameState* m_pGameState;
};

class CollisionSystem : public ISystem {
public:
  CollisionSystem(GameState* _pGameState);
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  GameState* m_pGameState;
  static bool AABBOverlap(const Transform& _rTransformA, const Collider& _rColliderA, const Transform& _rTransformB, const Collider& _rColliderB);
};

class RenderSystem : public ISystem {
public:
  RenderSystem(CEngine* _pEngine, GameState* _pGameState);
  ~RenderSystem();
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  CEngine* m_pEngine;
  GameState* m_pGameState;
  std::unordered_map<std::string, CSprite*> m_mCache;
};

class ScoreSystem : public ISystem {
public:
  ScoreSystem(GameState* _pGameState);
  void Update(ECS& _rEcs, float _fDeltaTime) override;
private:
  GameState* m_pGameState;
};