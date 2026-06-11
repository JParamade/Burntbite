## 🛠Key Features

* **Data-Oriented ECS Architecture:** A custom-built Entity Component System where Entities are just `uint32_t` IDs, Components are Plain Old Data (POD) structs, and Systems contain all gameplay logic.
* **Type-Indexed Storage:** The core registry uses `std::type_index` mapped to an abstract `IStorage` interface, allowing the engine to store any component type in contiguous memory maps without hardcoding lists.
* **Data-Driven XML Instantiation:** Integrated with **PugiXML** to act as an `EntityFactory`. Entities, along with their precise component configurations (Transforms, Velocities, Colliders), are constructed dynamically at runtime from external `.xml` files, drastically improving iteration times.
* **The "Manager Entity" Pattern:** Solves the classic ECS global state problem. A headless manager entity exclusively holds global components (like `Score` and `SpawnTimer`), allowing systems to query global rules without attaching them to physical actors.
* **Aggressive Memory Sweeping:** Board resets are instantaneous and memory-leak-free. The engine iterates through component maps and systematically erases any physical actor ID while preserving the headless Manager Entity.

---

## Architecture Overview

The engine strictly separates the core framework from the game logic:

### Engine
* **[`ECS.h`](./src/Engine/ECS.h) / [`ECS.cpp`](./src/Engine/ECS.cpp):** The beating heart of the engine. Manages entity ID generation and polymorphic component storage (`StorageImpl<C>`).
* **[`Engine.h`](./src/Engine.h) / [`Engine.cpp`](./src/Engine.cpp):** The core wrapper handling window creation, time-stepping, and rendering abstraction.

### Game
* **[`Components.h`](./src/Game/Components.h):** Defines all POD structs (`Transform`, `Velocity`, `Collider`, `PlayerTag`, etc.).
* **[`Systems.h`](./src/Game/Systems.h) / [`Systems.cpp`](./src/Game/Systems.cpp):** Contains all the logic loops (`MovementSystem`, `CollisionSystem`, `RenderSystem`). Every system inherits from `ISystem` for clean polymorphic updates in the main game loop.
* **[`EntityFactory.h`](./src/Game/EntityFactory.h) / [`EntityFactory.cpp`](./src/Game/EntityFactory.cpp):** The XML parser that reads external files and translates them into ECS components.
* **[`GameState.h`](./src/Game/GameState.h) / [`GameState.cpp`](./src/Game/GameState.cpp):** Handles the Manager Entity, high-score file serialization, and game-reset memory sweeps.

---

## Dependencies

Burntbite has some external dependencies:

* **[Tigr](https://github.com/erkkah/tigr):** A tiny, cross-platform graphics library used for window creation, input polling, and basic rendering.
* **[PugiXML](https://pugixml.org/):** A light-weight, fast C++ XML processing library used for data-driven entity parsing.