#pragma once

#include "../Engine/ECS.h"
#include "Components.h"
#include <string>

class GameState {
public:
  // Constructor & Destructor
  GameState(ECS& _rEcs, const std::string& _rHighScoreFile = "./Data/highscore.txt");
  ~GameState();

  // Public Methods
  void Init(); 
  void CreatePlayerFromXML(const std::string& _rXmlPath);
  void OnPlayerDeath(ECS& _rEcs);
  void ResetForNewGame(ECS& _rEcs);
  Entity GetManagerEntity() const;

  // High Score Management
  void SaveHighScore();
  void LoadHighScore();

  // Reset Management
  void RequestReset();
  bool NeedsReset() const;
  void ClearReset();

  // Enemy Sprite Management
  const std::string& GetRandomEnemySpritePath() const;

private:
  ECS& m_rEcs;
  Entity m_iManager = INVALID_ENTITY;
  std::string m_sHighScoreFile;
  std::string m_sPlayerXmlPath = "./Data/Entities/player.xml";

  bool m_bResetRequested = false;

  std::vector<std::string> m_lEnemySpritePaths;
};