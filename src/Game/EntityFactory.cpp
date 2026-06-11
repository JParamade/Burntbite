#include "EntityFactory.h"

#include "../Sprite.h"
#include "Components.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../../lib/PugiXML/Header Files/pugixml.hpp"

using namespace std;

Entity EntityFactory::CreatePlayerFromXML(ECS& _rEcs, const std::string& _rXmlPath) {
  pugi::xml_document oDocument;
  pugi::xml_parse_result oResult = oDocument.load_file(_rXmlPath.c_str());
  if (!oResult) {
    cerr << "EntityFactory: failed to load " << _rXmlPath << " (" << oResult.description() << ")\n";
    return INVALID_ENTITY;
  }

  pugi::xml_node oNode = oDocument.child("entity");
  if (!oNode) {
    cerr << "EntityFactory: no <entity> node in " << _rXmlPath << "\n";
    return INVALID_ENTITY;
  }

  Entity iEntity = _rEcs.CreateEntity();

  // Transform
  pugi::xml_node oTransformNode = oNode.child("transform");
  if (oTransformNode) {
    Transform oTransform;
    oTransform.fPosX = oTransformNode.attribute("x").as_float(240.f);
    oTransform.fPosY = oTransformNode.attribute("y").as_float(440.f);

    _rEcs.AddComponent<Transform>(iEntity, oTransform);
  }
  else {
    Transform oTransform; 
    oTransform.fPosX = 240.f; 
    oTransform.fPosY = 440.f;

    _rEcs.AddComponent<Transform>(iEntity, oTransform);
  }

  // Velocity
  pugi::xml_node oVelocityNode = oNode.child("velocity");
  if (oVelocityNode) {
    Velocity oVelocity; 
    oVelocity.fVelocityX = oVelocityNode.attribute("vx").as_float(0.f); 
    oVelocity.fVelocityY = oVelocityNode.attribute("vy").as_float(0.f);

    _rEcs.AddComponent<Velocity>(iEntity, oVelocity);
  }
  else _rEcs.AddComponent<Velocity>(iEntity, Velocity());

  // Sprite
  pugi::xml_node oSpriteNode = oNode.child("sprite");
  if (oSpriteNode) {
    std::string sFilePath = oSpriteNode.attribute("file").as_string("./Data/Sprites/Character.png");
    int iWidth = oSpriteNode.attribute("w").as_int(16);
    int iHeight = oSpriteNode.attribute("h").as_int(16);
    
    SpriteComponent oSprite(sFilePath, iWidth, iHeight);

    _rEcs.AddComponent<SpriteComponent>(iEntity, oSprite);
  }
  else {
    SpriteComponent oSprite("./Data/Sprites/Character.png", 16, 16);
    _rEcs.AddComponent<SpriteComponent>(iEntity, oSprite);
  }

  // Collider
  pugi::xml_node oColliderNode = oNode.child("collider");
  if (oColliderNode) {
    Collider oCollider; 
    oCollider.fWidth = oColliderNode.attribute("w").as_float(16.f); 
    oCollider.fHeight = oColliderNode.attribute("h").as_float(16.f);

    _rEcs.AddComponent<Collider>(iEntity, oCollider);
  }
  else _rEcs.AddComponent<Collider>(iEntity, Collider());

  // Player Tag
  _rEcs.AddComponent<PlayerTag>(iEntity, PlayerTag());

  return iEntity;
}