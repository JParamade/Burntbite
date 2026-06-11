#pragma once

#include "../Engine/ECS.h"
#include <string>

namespace EntityFactory {
  // Creates a player entity based on the provided XML file path. The XML file should have the following structure:
  // <entity>
  //   <transform x="240" y="440" />
  //   <velocity vx="0" vy="0" />
  //   <sprite file="./Data/Sprites/Character.png" w="16" h="16" />
  //   <collider w="16" h="16" />
  // </entity>
  Entity CreatePlayerFromXML(ECS& _rEcs, const std::string& _rXmlPath);
}