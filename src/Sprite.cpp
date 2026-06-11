#include "Sprite.h"

#include "../lib/PugiXML/Header Files/pugixml.hpp"

#include<sstream>

CSprite::CSprite(const std::string& _rFile) {
	sFile = _rFile;
	pTexture = tigrLoadImage(sFile.c_str());
}

CSprite::~CSprite() {
	if (pTexture) {
		tigrFree(pTexture);
		pTexture = nullptr;
	}
}

void CSprite::Draw(Tigr* pWindow) {
	if (!pTexture) return;

	tigrBlitAlpha(pWindow, pTexture, m_iPosX, m_iPosY, 0, 0, pTexture->iWidth, pTexture->iHeight, 1);
}

bool CSprite::Save(std::string& _rDataSource, const CSprite& _rTexture) {
	pugi::xml_document oDocument;
	pugi::xml_node oNode = oDocument.append_child("sprite");
	oNode.append_attribute("x").set_value(m_iPosX);
	oNode.append_attribute("y").set_value(m_iPosY);
	oNode.append_attribute("file_name").set_value(sFile.c_str());

	try {	
		std::stringstream ss;
		oDocument.save(ss);
		_rDataSource = ss.str();
		return true;
	}
	catch (...) { return false; }
}

bool CSprite::Load(CSprite& _rTexture, std::string& _rDataSource) {
	try {
		pugi::xml_document oDocument;
		pugi::xml_parse_result oResult = oDocument.load_string(_rDataSource.c_str());

		if (oResult) {
			std::string sFilePath = oDocument.child("sprite").attribute("file_name").value();
			
			_rTexture = CSprite(sFilePath);
			_rTexture.m_iPosX = oDocument.child("sprite").attribute("x").as_float();
			_rTexture.m_iPosY = oDocument.child("sprite").attribute("y").as_float();

			return true;
		}
	}
	catch (...) { return false; }
}