#pragma once

#include "../lib/Tigr/tigr.h"

#include <string>

class CSprite {
public:
	CSprite() = default;
	CSprite(const std::string& _rFile);
	~CSprite();

	void Draw(Tigr* pWindow);
	bool Save(std::string& _rDataSource, const CSprite& _rTexture);
	bool Load(CSprite& _rTexture, std::string& _rDataSource);
	
	// Serialized (should be private in the future)
	int m_iPosX, m_iPosY;
	std::string sFile;
private:
	// Private
	Tigr* pTexture = nullptr;
};

