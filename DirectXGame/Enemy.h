#pragma once
#include "KamataEngine.h"

class Enemy {
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	static inline const float kWalkSpeed = 0.02f;

	static inline const float kWalkMotionAngleStart =0.f;

	static inline const float kWalkMotionAngleEnd = 45.f;

	static inline const float kWalkMotionTime = 2.f;
	float walkTimer_ = 0.f;

public:
	Enemy();
	~Enemy();
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
};