#pragma once
#include <array>
#include "KamataEngine.h"
#include <numbers>
class DeathParticles {
private:
	static inline const uint32_t kNumParticles = 8;

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kDuration = 2.0f;
	static inline const float kSpeed = 0.05f;
	static inline const float kAngleUnit = std::numbers::pi_v<float> * 2.0f / static_cast<float>(kNumParticles);

	bool isFinished_ = false;
	float counter_ = 0.f;
	
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_;


public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3 position);
	void Update();
	void Draw();

	bool IsFinished() const { return isFinished_; }
};
