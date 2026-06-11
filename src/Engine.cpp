// JAIME PÁRAMO BENÍTEZ

#include "Engine.h"

// Engine Execution

bool CEngine::Init() {
  // Create Window
	m_pWindow = tigrWindow(500, 500, "Burntbite Test Game", 0);
	if (!m_pWindow) return false;

  // Set flag to indicate the engine is running.
	m_bIsRunning = true;

  // Return success.
	return true;
}

void CEngine::Update() {
	// Update the window.
	tigrUpdate(m_pWindow);
}

void CEngine::ClearWindow() {
  // Clear the window.
	tigrClear(m_pWindow, tigrRGB(135, 206, 235));
}

bool CEngine::IsRunning() {
  // Check if the window has been closed.
	if (tigrClosed(m_pWindow)) m_bIsRunning = false;

  // Return the running state.
	return m_bIsRunning;
}

bool CEngine::Quit() {
  // Free the window resources.
	tigrFree(m_pWindow);
	m_pWindow = nullptr;

  // Return success.
	return true;
}

// Print Functions

void CEngine::Log(const std::string _sText) {
  // Log the text to the console.
	puts(_sText.c_str());
}

void CEngine::Print(const std::string _sText) {
  // Print the text to the window at a default position (10, 10) with white color.
	tigrPrint(m_pWindow, tfont, 10, 10, tigrRGB(0xff, 0xff, 0xff), _sText.c_str());
}

void CEngine::PrintWithOffset(const std::string _sText, int _iOffsetX, int _iOffsetY) {
  // Print the text to the window at the specified offset position with white color.
	tigrPrint(m_pWindow, tfont, _iOffsetX, _iOffsetY, tigrRGB(0xff, 0xff, 0xff), _sText.c_str());
}

// Temporization Functions

float CEngine::Wait(float _fMs) {
	float fElapsedTime = 0;
	
  // Loop until the specified time has elapsed, accumulating the elapsed time using tigrTime().
	for (tigrTime(); fElapsedTime < _fMs; fElapsedTime += tigrTime());

  // Return the total elapsed time.
	return fElapsedTime;
}

// Getters/Setters

Tigr* CEngine::GetWindow() {
  // Return the pointer to the window.
	return m_pWindow; 
}