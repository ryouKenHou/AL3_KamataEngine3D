#pragma once

#include "KamataEngine.h"
#include "helper.hpp"

class HitEffect {
private:
	enum class Status {
		kFadeIn,
		kFadeOut,
		kFinished,
	};

	KamataEngine::WorldTransform circleWorldTransform_;
	std::array<KamataEngine::WorldTransform, 2> ellipseWorldTransforms_;
	Status status_ = Status::kFadeIn;
	float timer_ = 0;
	float duration_ = 20.f; // フェードの継続時間（フレーム）
	float alpha_ = 1.f;

	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;


public:
	void Initialize(KamataEngine::Vector3 position);
	void Update();
	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; }	
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }
	static HitEffect* Create(KamataEngine::Vector3 position);

	bool IsFinished() const { return status_ == Status::kFinished; }
};
