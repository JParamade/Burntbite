// JAIME PÁRAMO BENÍTEZ

#pragma once

#include "../lib/Tigr/tigr.h"
#include <string>
#include <sstream>

class CEngine {
public:
	// Engine Execution
	bool Init();
	void Update();
	void ClearWindow();
	bool IsRunning();
	bool Quit();
	
	// Print Functions
	void Log(const std::string _sText);
	void Print(const std::string _sText);
	void PrintWithOffset(const std::string _sText, int _iOffsetX, int _iOffsetY);
	
	// Temporization Functions
	float Wait(float _fMs);

	// Utils
	template<typename T>
	std::string ToString(const T& _rObject) {
		std::stringstream ss;
		ss << _rObject;
		return ss.str();
	}

	// Getters/Setters
	Tigr* GetWindow();
private:
	bool m_bIsRunning = false;
	Tigr* m_pWindow = nullptr;
};

