#include "Engine.h"
#include "Sprite.h"

#include "Engine/ECS.h"
#include "Game/Components.h"
#include "Game/Systems.h"
#include "Game/GameState.h"
#include "Game/EntityFactory.h"

#include <memory>
#include <vector>

int main() {
  // Initialize Engine.
	CEngine oEngine;
	if (!oEngine.Init()) return -1;

  // Create ECS and GameState.
	ECS oEcs;
	GameState oGameState(oEcs, "./data/highscore.txt");

  // Initialize GameState.
	oGameState.Init();
	oGameState.CreatePlayerFromXML("./data/entities/player.xml");

  /// Initialize Systems.
	auto oInputSystem = std::make_shared<InputSystem>(&oEngine, &oGameState);
	auto oMovementSystem = std::make_shared<MovementSystem>(&oEngine);
	auto oSpawnSystem = std::make_shared<SpawnSystem>(&oGameState);
	auto oCollisionSystem = std::make_shared<CollisionSystem>(&oGameState);
	auto oRenderSystem = std::make_shared<RenderSystem>(&oEngine, &oGameState);
	auto oScoreSystem = std::make_shared<ScoreSystem>(&oGameState);

	// Store Systems.
	std::vector<std::shared_ptr<ISystem>> lSystems;
	lSystems.push_back(oInputSystem);
	lSystems.push_back(oMovementSystem);
	lSystems.push_back(oSpawnSystem);
	lSystems.push_back(oCollisionSystem);
	lSystems.push_back(oScoreSystem);
	lSystems.push_back(oRenderSystem);

  // Main Game Loop.
	while (oEngine.IsRunning()) {
    // Calculate Delta Time.
		float fDeltaTime = oEngine.Wait(1.0f / 60.0f);
    // Limit FPS to 60 avoiding division by zero.
		float fFPS = 1.0f / (fDeltaTime + !fDeltaTime);

    // Clean Buffer.
		oEngine.ClearWindow();

    // Update Systems
		for (auto& rSystem : lSystems) rSystem->Update(oEcs, fDeltaTime);

    // Check if Game Reset is needed.
		if (oGameState.NeedsReset()) {
      // Reset GameState and ECS for new game.
			oGameState.ResetForNewGame(oEcs);
			oGameState.ClearReset();
		}

    // Print FPS.
		oEngine.PrintWithOffset(oEngine.ToString(fFPS) + " fps", 10, 20);
    // Update Graphics.
		oEngine.Update();
	}

  // Quit Engine.
	oEngine.Quit();

	return 0;
}