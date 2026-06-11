#pragma once

#include <string>
#include "../Sprite.h"
#include <cmath>

struct Transform {
  // Position
  float fPosX = .0f;
  float fPosY = .0f;
  
  // Constructors
  Transform() = default;
  Transform(float _fPosX, float _fPosY)
    : fPosX(_fPosX)
    , fPosY(_fPosY)
  {}
};

struct Velocity {
  // Velocity
  float fVelocityX = .0f;
  float fVelocityY = .0f;

  // Constructors
  Velocity() = default;
  Velocity(float _fVelocityX, float _fVelocityY)
    : fVelocityX(_fVelocityX)
    , fVelocityY(_fVelocityY)
  {}
};

struct SpriteComponent {
  // Sprite Info
  std::string sFilePath;
  int iWidth = 16;
  int iHeight = 16;

  // Constructors
  SpriteComponent() = default;
  SpriteComponent(const std::string& _rFilePath, int _iWidth = 16, int _iHeight = 16)
    : sFilePath(_rFilePath)
    , iWidth(_iWidth)
    , iHeight(_iHeight)
  {}
};

struct PlayerTag {};
struct EnemyTag {};

struct Collider {
  // Dimensions (AABB)
  float fWidth = 8;
  float fHeight = 8;

  // Constructors
  Collider() = default;
  Collider(float _fWidth, float _fHeight)
    : fWidth(_fWidth)
    , fHeight(_fHeight)
  {}
};

struct Lifetime {
  // Duration (seconds)
  float fDuration;

  // Constructors
  Lifetime() = default;
  Lifetime(float _fDuration)
    : fDuration(_fDuration)
  {}
};

struct SpawnTimer {
  // Time Info (seconds)
  float fElapsed = .0f;
  float fInterval = 5.f;

  // Constructors
  SpawnTimer() = default;
  SpawnTimer(float _fElapsed, float _fInterval)
    : fElapsed(_fElapsed)
    , fInterval(_fInterval)
  {}
};

struct Score {
  // Score Info
  float fCurrentTime = .0f;
  float fHighScore = .0f;
  bool bRunning = false;
  
  // Constructors
  Score() = default;
  Score(float _fCurrentTime, float _fHighScore, bool _bRunning)
    : fCurrentTime(_fCurrentTime)
    , fHighScore(_fHighScore)
    , bRunning(_bRunning)
  {}
};