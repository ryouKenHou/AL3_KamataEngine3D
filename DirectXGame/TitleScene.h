#pragma once
#include "2d/ImGuiManager.h"
#include "KamataEngine.h"
#include "helper.hpp"
#include "vector"
#include "Fade.h"

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

private:
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	KamataEngine::Model* titleModel_ = nullptr;
	KamataEngine::WorldTransform titleWorldTransform_;

	KamataEngine::Camera* camera_ = nullptr;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;

	int frameCounter_ = 0;
	bool isFinished_ = false;

public:
	~TitleScene() {
		delete playerModel_;
		delete titleModel_;
		delete camera_;
		delete fade_;
	}

	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return isFinished_; }
};