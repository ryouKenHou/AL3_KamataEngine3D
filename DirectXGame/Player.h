#pragma once
#include "KamataEngine.h"


class Player {
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;

	KamataEngine::Camera* camera_ = nullptr;

public:
	Player();
	~Player();

	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera);

	void Update();

	void Draw();
};