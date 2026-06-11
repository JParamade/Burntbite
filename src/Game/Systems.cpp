#include "Systems.h"

#include "GameState.h"
#include "EntityFactory.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <sstream>
#include "../../lib/Tigr/tigr.h"

static std::string format_time_float(float t) {
  int iTotalMs = (int)std::round(t * 1000.0f);
  int iMilliseconds = iTotalMs % 1000;
  int iSeconds = (iTotalMs / 1000) % 60;
  int iMinutes = (iTotalMs / 1000) / 60;
  char cBuffer[64];
  std::snprintf(cBuffer, sizeof(cBuffer), "%02d:%02d.%03d", iMinutes, iSeconds, iMilliseconds);
  return std::string(cBuffer);
}

// ---------------- InputSystem ----------------
InputSystem::InputSystem(CEngine* _pEngine, GameState* _pGameState)
  : m_pEngine(_pEngine)
  , m_pGameState(_pGameState) 
{}

void InputSystem::Update(ECS& _rEcs, float _fDeltaTime) {
  if (!m_pEngine || !m_pGameState) return;

  // Get manager entity to access score component for game state management.
  Entity iManager = m_pGameState->GetManagerEntity();
  if (iManager == INVALID_ENTITY) return;

  // Get score component to check game state and manage game start.
  Score* pScore = _rEcs.GetComponent<Score>(iManager);
  if (!pScore) return;

  // If the game isn't running, check for spacebar press to start the game and reset the spawn timer.
  if (!pScore->bRunning) {
    if (tigrKeyDown(m_pEngine->GetWindow(), TK_SPACE)) {
      pScore->bRunning = true;
      pScore->fCurrentTime = .0f;

      SpawnTimer* pSpawn = _rEcs.GetComponent<SpawnTimer>(m_pGameState->GetManagerEntity());
      if (pSpawn) pSpawn->fElapsed = .0f;
    }

    return;
  }

  // Get player entity.
  auto& rPlayers = _rEcs.Storage<PlayerTag>();
  if (rPlayers.empty()) return;
  Entity iPlayer = rPlayers.begin()->first;

  // Get player velocity component.
  Velocity* pPlayerVelocity = _rEcs.GetComponent<Velocity>(iPlayer);
  if (!pPlayerVelocity) return;

  // Update player velocity based on input.
  float fSpeed = 160.f; 
  pPlayerVelocity->fVelocityX = .0f;
  if (tigrKeyHeld(m_pEngine->GetWindow(), TK_LEFT) || tigrKeyHeld(m_pEngine->GetWindow(), 'A')) pPlayerVelocity->fVelocityX = -fSpeed;
  if (tigrKeyHeld(m_pEngine->GetWindow(), TK_RIGHT) || tigrKeyHeld(m_pEngine->GetWindow(), 'D')) pPlayerVelocity->fVelocityX = fSpeed;
}

// ---------------- MovementSystem ----------------
MovementSystem::MovementSystem(CEngine* _pEngine)
  : m_pEngine(_pEngine)
{
  m_iWindowWidth = 500;
  m_iWindowHeight = 500;
}

void MovementSystem::Update(ECS& _rEcs, float _fDeltaTime) {
  auto& rVelocities = _rEcs.Storage<Velocity>();
  auto& rTransforms = _rEcs.Storage<Transform>();
  auto& rSprites = _rEcs.Storage<SpriteComponent>();

  for (auto& rKeyValue : rVelocities) {
    // Get the entity and its velocity component.
    Entity iEntity = rKeyValue.first;
    Velocity& rVelocity = rKeyValue.second;
    
    // Get the corresponding transform component for the entity.
    auto oIterator = rTransforms.find(iEntity);
    if (oIterator == rTransforms.end()) continue;
    Transform& rTransform = oIterator->second;

    // Apply gravity to entities with EnemyTag.
    if (_rEcs.HasComponent<EnemyTag>(iEntity)) rVelocity.fVelocityY += m_fGravity * _fDeltaTime;

    // Update position based on velocity and delta time.
    rTransform.fPosX += rVelocity.fVelocityX * _fDeltaTime;
    rTransform.fPosY += rVelocity.fVelocityY * _fDeltaTime;

    // Get sprite dimensions for clamping, defaulting to 16x16 if no sprite component is found.
    int iSpriteWidth = 16, iSpriteHeight = 16;
    auto oSpriteIterator = rSprites.find(iEntity);
    if (oSpriteIterator != rSprites.end()) {
      iSpriteWidth = oSpriteIterator->second.iWidth;
      iSpriteHeight = oSpriteIterator->second.iHeight;
    }

    // Clamp position to window bounds.
    if (rTransform.fPosX < 0) rTransform.fPosX = 0;
    if (rTransform.fPosX > (float)(m_iWindowWidth - iSpriteWidth)) rTransform.fPosX = (float)(m_iWindowWidth - iSpriteWidth);
    if (rTransform.fPosY < -100.f) rTransform.fPosY = -100.f; 
    if (rTransform.fPosY > (float)(m_iWindowHeight - iSpriteHeight)) rTransform.fPosY = (float)(m_iWindowHeight - iSpriteHeight);
  }
}

// ---------------- SpawnSystem ----------------
SpawnSystem::SpawnSystem(GameState* _pGameState)
  : m_pGameState(_pGameState) 
{
  std::srand((unsigned)std::time(nullptr));
}

void SpawnSystem::Update(ECS& _rEcs, float _fDeltaTime) {
  auto& rTimers = _rEcs.Storage<SpawnTimer>();
  if (rTimers.empty()) return;
  
  Entity iManager = m_pGameState->GetManagerEntity();
  if (iManager == INVALID_ENTITY) return;

  Score* pScore = _rEcs.GetComponent<Score>(iManager);
  if (!pScore || !pScore->bRunning) return; 

  // Update the spawn timer and check if it's time to spawn a new enemy.
  for (auto& rKeyValue : rTimers) {
    rKeyValue.second.fElapsed += _fDeltaTime;
    if (rKeyValue.second.fElapsed >= rKeyValue.second.fInterval) {
      rKeyValue.second.fElapsed = .0f;

      // Create a new enemy entity with random position¡.
      Entity iEntity = _rEcs.CreateEntity();
      float fPosX = (float)(rand() % 500);
      float fPosY = -25.f;

      Transform oTransform; oTransform.fPosX = fPosX; oTransform.fPosY = fPosY;
      _rEcs.AddComponent<Transform>(iEntity, oTransform);

      // Generate random velocity parameters.
      float fBaseSpeedX = 100.0f + (rand() % 101);
      int iDirectionX = (rand() % 2 == 0) ? 1 : -1;
      float fVelocityX = fBaseSpeedX * iDirectionX;
      float fVelocityY = (float)((rand() % 101) - 50);
      Velocity oVelocity; oVelocity.fVelocityX = fVelocityX; oVelocity.fVelocityY = fVelocityY;
      _rEcs.AddComponent<Velocity>(iEntity, oVelocity);

      // Generate a random size for the collider between 12 and 36 pixels, and add a Collider component to the entity with that size.
      Collider oCollider; oCollider.fWidth = 12.f + (rand() % 24); oCollider.fHeight = oCollider.fWidth;
      _rEcs.AddComponent<Collider>(iEntity, oCollider);

      int iSize = (int)oCollider.fWidth;

      // Get a random sprite path from the GameState's list of enemy sprites and add a SpriteComponent to the entity with that path and size.
      std::string sRandomSpritePath = m_pGameState->GetRandomEnemySpritePath();
      SpriteComponent oSprite(sRandomSpritePath, iSize, iSize);
      _rEcs.AddComponent<SpriteComponent>(iEntity, oSprite);

      // Add EnemyTag to identify this entity as an enemy for collision and game logic purposes.
      _rEcs.AddComponent<EnemyTag>(iEntity, EnemyTag());
    }
  }
}

// ---------------- CollisionSystem ----------------
CollisionSystem::CollisionSystem(GameState* _pGameState)
  : m_pGameState(_pGameState) {
}

bool CollisionSystem::AABBOverlap(const Transform& _rTransformA, const Collider& _rColliderA, const Transform& _rTransformB, const Collider& _rColliderB) {
  return !(_rTransformA.fPosX + _rColliderA.fWidth < _rTransformB.fPosX || _rTransformB.fPosX + _rColliderB.fWidth < _rTransformA.fPosX || _rTransformA.fPosY + _rColliderA.fHeight < _rTransformB.fPosY || _rTransformB.fPosY + _rColliderB.fHeight < _rTransformA.fPosY);
}

void CollisionSystem::Update(ECS& _rEcs, float _fDeltaTime) {
  auto& rPlayers = _rEcs.Storage<PlayerTag>();
  if (rPlayers.empty()) return;

  Entity iPlayer = rPlayers.begin()->first;
  Transform* pPlayeTransform = _rEcs.GetComponent<Transform>(iPlayer);
  Collider* pPlayerCollider = _rEcs.GetComponent<Collider>(iPlayer);
  if (!pPlayeTransform || !pPlayerCollider) return;

  auto& rEnemies = _rEcs.Storage<EnemyTag>();
  for (auto& rKeyValue : rEnemies) {
    Entity iEntity = rKeyValue.first;

    Transform* pEnemyTransform = _rEcs.GetComponent<Transform>(iEntity);
    Velocity* pEnemyVelocity = _rEcs.GetComponent<Velocity>(iEntity);
    Collider* pEnemyCollider = _rEcs.GetComponent<Collider>(iEntity);
    if (!pEnemyTransform || !pEnemyVelocity || !pEnemyCollider) continue;

    float fBottomY = pEnemyTransform->fPosY + pEnemyCollider->fHeight;
    
    // Floor Collision
    int iWindowHeight = 500; 
    if (fBottomY >= iWindowHeight) {
      pEnemyTransform->fPosY = (float)iWindowHeight - pEnemyCollider->fHeight;
      pEnemyVelocity->fVelocityY *= -1.0f;
    }

    // Wall Collision
    int iWindowWidth = 500;
    if (pEnemyTransform->fPosX <= 0.0f) {
      pEnemyTransform->fPosX = 0.0f;
      pEnemyVelocity->fVelocityX *= -1.0f;
    }
    if (pEnemyTransform->fPosX + pEnemyCollider->fWidth >= iWindowWidth) {
      pEnemyTransform->fPosX = (float)iWindowWidth - pEnemyCollider->fWidth;
      pEnemyVelocity->fVelocityX *= -1.0f;
    }

    // Player Collision
    if (AABBOverlap(*pPlayeTransform, *pPlayerCollider, *pEnemyTransform, *pEnemyCollider)) {
      if (m_pGameState) {
        if (!m_pGameState->NeedsReset()) m_pGameState->OnPlayerDeath(_rEcs);
      }
      return;
    }
  }
}

// ---------------- RenderSystem ----------------
RenderSystem::RenderSystem(CEngine* _pEngine, GameState* _pGameState)
  : m_pEngine(_pEngine) 
  , m_pGameState(_pGameState)
{}

RenderSystem::~RenderSystem() {
  for (auto& rKeyValue : m_mCache) delete rKeyValue.second;
  m_mCache.clear();
}

void RenderSystem::Update(ECS& ecs, float dt) {
  if (!m_pEngine) return;

  auto& rSprites = ecs.Storage<SpriteComponent>();
  auto& rTransforms = ecs.Storage<Transform>();

  // Iterate through all entities with SpriteComponent and Transform, draw their sprites at the corresponding positions.
  for (auto& rKeyValue : rSprites) {
    Entity iEntity = rKeyValue.first;

    auto oTransformIterator = rTransforms.find(iEntity);
    if (oTransformIterator == rTransforms.end()) continue;

    SpriteComponent& rSprite = rKeyValue.second;
    Transform& rTransform = oTransformIterator->second;

    CSprite* pSprite = nullptr;
    auto oSpriteIterator = m_mCache.find(rSprite.sFilePath);
    if (oSpriteIterator == m_mCache.end()) {
      CSprite* pAuxSprite = new CSprite(rSprite.sFilePath);
      m_mCache[rSprite.sFilePath] = pAuxSprite;
      pSprite = pAuxSprite;
    }
    else pSprite = oSpriteIterator->second;

    pSprite->m_iPosX = (int)std::round(rTransform.fPosX);
    pSprite->m_iPosY = (int)std::round(rTransform.fPosY);
    pSprite->Draw(m_pEngine->GetWindow());
  }

  Score* pScore = nullptr;
  auto& rScores = ecs.Storage<Score>();
  if (!rScores.empty()) pScore = &rScores.begin()->second;

  // If a score component exists, display the current time and high score in the top-left corner of the screen.
  if (pScore) {
    std::string sScore = "Time: " + format_time_float(pScore->fCurrentTime) + "   High Score: " + format_time_float(pScore->fHighScore);
    m_pEngine->Print(sScore);
  }

  // If the game isn't running, display the "PRESS SPACE TO START" message centered on the screen with "SPACE" in an accent color.
  if (pScore && !pScore->bRunning) {
    const int iWindowWidth = 500;
    const int iWindowHeight = 500;

    const std::string sPart1 = "PRESS ";
    const std::string sPart2 = "SPACE";
    const std::string sPart3 = " TO START";

    TPixel white = tigrRGB(0xff, 0xff, 0xff); 
    TPixel accentColor = tigrRGB(0xff, 0xd7, 0x00);
    
    int iTotalTextWidth = tigrTextWidth(tfont, (sPart1 + sPart2 + sPart3).c_str());
    int iTextHeight = tigrTextHeight(tfont, sPart1.c_str());

    int iStartX = (iWindowWidth / 2) - (iTotalTextWidth / 2);
    int iStartY = (iWindowHeight / 2) - (iTextHeight / 2);

    tigrPrint(m_pEngine->GetWindow(), tfont, iStartX, iStartY, white, sPart1.c_str());

    int iPart2X = iStartX + tigrTextWidth(tfont, sPart1.c_str());
    tigrPrint(m_pEngine->GetWindow(), tfont, iPart2X, iStartY, accentColor, sPart2.c_str());

    int iPart3X = iPart2X + tigrTextWidth(tfont, sPart2.c_str());
    tigrPrint(m_pEngine->GetWindow(), tfont, iPart3X, iStartY, white, sPart3.c_str());
  }
}

// ---------------- ScoreSystem ----------------
ScoreSystem::ScoreSystem(GameState* _pGameState)
  : m_pGameState(_pGameState) {
}

void ScoreSystem::Update(ECS& _rEcs, float _fDeltaTime) {
  // Get the score component associated with the manager entity to update the current time and manage high score.
  auto& rScoreComponent = _rEcs.Storage<Score>();
  if (rScoreComponent.empty()) return;

  // Assuming there's only one score component associated with the manager entity, we can directly access it.
  Score& rScore = rScoreComponent.begin()->second;
  if (rScore.bRunning) {
    rScore.fCurrentTime += _fDeltaTime;

    // If the current time exceeds the high score, update the high score to the current time.
    if (rScore.fCurrentTime > rScore.fHighScore) rScore.fHighScore = rScore.fCurrentTime;
  }
}