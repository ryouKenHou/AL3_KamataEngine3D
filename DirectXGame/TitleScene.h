#pragma once
#include "2d/ImGuiManager.h"
#include "KamataEngine.h"
#include "helper.hpp"
#include "vector"
class TitleScene {
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	KamataEngine::Model* titleModel_ = nullptr;
	KamataEngine::WorldTransform titleWorldTransform_;

	KamataEngine::Camera* camera_ = nullptr;

	int frameCounter_ = 0;
	bool isFinished_ = false;

public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return isFinished_; }
};