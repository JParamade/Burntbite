#include "GameState.h"

#include "EntityFactory.h"
#include <fstream>
#include <iostream>

GameState::GameState(ECS& _rEcs, const std::string& _rHighScoreFile)
  : m_rEcs(_rEcs)
  , m_sHighScoreFile(_rHighScoreFile)
{}

GameState::~GameState() {
  SaveHighScore();
}

void GameState::Init() {
  // Create Manager Entity.
  m_iManager = m_rEcs.CreateEntity();

  // Add Spawn Timer Component to Manager Entity.
  SpawnTimer oSpawnTimer;
  oSpawnTimer.fInterval = 5.f;
  oSpawnTimer.fElapsed = .0f;

  m_rEcs.AddComponent<SpawnTimer>(m_iManager, oSpawnTimer);

  // Add Score Component to Manager Entity.
  Score oScore;
  oScore.fCurrentTime = .0f;
  oScore.fHighScore = .0f;
  oScore.bRunning = false;

  m_rEcs.AddComponent<Score>(m_iManager, oScore);

  LoadHighScore();

  // Initialize Enemy Sprite Paths.
  m_lEnemySpritePaths.push_back("./data/sprites/enemy1.png"); 
  m_lEnemySpritePaths.push_back("./data/sprites/enemy2.png"); 
  m_lEnemySpritePaths.push_back("./data/sprites/enemy3.png");
  m_lEnemySpritePaths.push_back("./data/sprites/enemy4.png");
  m_lEnemySpritePaths.push_back("./data/sprites/enemy5.png");
  m_lEnemySpritePaths.push_back("./data/sprites/enemy6.png");
}

void GameState::CreatePlayerFromXML(const std::string& _rXmlPath) {
  // Store the player XML path for future resets.
  m_sPlayerXmlPath = _rXmlPath;
  // Create the player entity from the XML path.
  Entity iEntity = EntityFactory::CreatePlayerFromXML(m_rEcs, _rXmlPath);
}

void GameState::OnPlayerDeath(ECS& _rEcs) {
  // Stop the score timer.
  Score* pScore = _rEcs.GetComponent<Score>(m_iManager);
  if (pScore) pScore->bRunning = false;

  SaveHighScore();
  RequestReset();
}

void GameState::ResetForNewGame(ECS& _rEcs) {
  // Remove all entities except the manager entity and its components.
  auto& rPlayers = _rEcs.Storage<PlayerTag>();
  std::vector<Entity> lPlayersToRemove;
  for (auto& rKeyValue : rPlayers) if (rKeyValue.first != m_iManager) lPlayersToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lPlayersToRemove) rPlayers.erase(iEntity);

  auto& rEnemies = _rEcs.Storage<EnemyTag>();
  std::vector<Entity> lEnemiesToRemove;
  for (auto& rKeyValue : rEnemies) if (rKeyValue.first != m_iManager) lEnemiesToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lEnemiesToRemove) rEnemies.erase(iEntity);

  // Remove all components except those attached to the manager entity.
  auto& rTransforms = _rEcs.Storage<Transform>();
  std::vector<Entity> lTransformsToRemove;
  for (auto& rKeyValue : rTransforms) if (rKeyValue.first != m_iManager) lTransformsToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lTransformsToRemove) rTransforms.erase(iEntity);

  auto& rVelocities = _rEcs.Storage<Velocity>();
  std::vector<Entity> lVelocitiesToRemove;
  for (auto& rKeyValue : rVelocities) if (rKeyValue.first != m_iManager) lVelocitiesToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lVelocitiesToRemove) rVelocities.erase(iEntity);

  auto& rSprites = _rEcs.Storage<SpriteComponent>();
  std::vector<Entity> lSpritesToRemove;
  for (auto& rKeyValue : rSprites) if (rKeyValue.first != m_iManager) lSpritesToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lSpritesToRemove) rSprites.erase(iEntity);

  auto& rCollisions = _rEcs.Storage<Collider>();
  std::vector<Entity> lCollisionsToRemove;
  for (auto& rKeyValue : rCollisions) if (rKeyValue.first != m_iManager) lCollisionsToRemove.push_back(rKeyValue.first);
  for (Entity iEntity : lCollisionsToRemove) rCollisions.erase(iEntity);

  // Reset the score and spawn timer components on the manager entity.
  Score* pScore = _rEcs.GetComponent<Score>(m_iManager);
  if (pScore) {
    pScore->fCurrentTime = .0f;
    pScore->bRunning = false;
  }
  SpawnTimer* pSpawnTimer = _rEcs.GetComponent<SpawnTimer>(m_iManager);
  if (pSpawnTimer) pSpawnTimer->fElapsed = .0f;

  // Create a new player entity from the stored XML path.
  CreatePlayerFromXML(m_sPlayerXmlPath);
}

Entity GameState::GetManagerEntity() const {
  return m_iManager;
}

void GameState::SaveHighScore() {
  // Get the current score from the manager entity.
  Score* pScore = m_rEcs.GetComponent<Score>(m_iManager);
  if (!pScore) return;

  // Only save if the current score is a new high score.
  std::ofstream oOutputStream(m_sHighScoreFile, std::ios::trunc);
  if (!oOutputStream.good()) return;

  // Write the high score to the file.
  oOutputStream << pScore->fHighScore;
}

void GameState::LoadHighScore() {
  std::ifstream oInputStream(m_sHighScoreFile);

  if (!oInputStream.good()) return;

  // Read the high score from the file.
  float fValue = .0f;
  oInputStream >> fValue;

  // Update the Score component with the loaded high score value.
  Score* pScore = m_rEcs.GetComponent<Score>(m_iManager);
  if (pScore) pScore->fHighScore = fValue;
}

void GameState::RequestReset() {
  m_bResetRequested = true;
}

bool GameState::NeedsReset() const {
  return m_bResetRequested;
}

void GameState::ClearReset() {
  m_bResetRequested = false;
}

const std::string& GameState::GetRandomEnemySpritePath() const {
  // If there are no enemy sprite paths available, return a default path.
  if (m_lEnemySpritePaths.empty()) {
    static const std::string sDefaultPath = "./data/sprites/enemy1.png";
    return sDefaultPath;
  }

  // Select a random index from the list of enemy sprite paths and return the corresponding path.
  int iRandomIndex = rand() % m_lEnemySpritePaths.size();
  return m_lEnemySpritePaths[iRandomIndex];
}