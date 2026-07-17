#include "HitEffect.h"
#include <algorithm>

KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(KamataEngine::Vector3 position) {
	HitEffect* instance = new HitEffect();
	assert(instance);
	instance->Initialize(position);

	return instance;
}

void HitEffect::Initialize(KamataEngine::Vector3 position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {1.f, 1.f, 1.f};
	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

	// ellipseWorldTransforms_
	Random random;
	random.Initialize();
	for (KamataEngine::WorldTransform& transform : ellipseWorldTransforms_) {
		transform.translation_ = position;
		transform.scale_ = {0.1f, 2.f, 1.f};
		transform.rotation_ = {0.f, 0.f, random.GetRandomFloat(-3.14159f, 3.14159f)};
		transform.matWorld_ = CreateAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
		transform.Initialize();
	}
}

void HitEffect::Update() {
	timer_++;
	switch (status_) {
	case HitEffect::Status::kFadeIn: {
		
		alpha_ = 1.f;
		float scale = EaseOut(0.f, 1.0f, timer_ / duration_);
		float scale1 = EaseOut(0.f, 2.0f, timer_ / duration_);
		float scale2 = EaseOut(0.f, 0.1f, timer_ / duration_);

		circleWorldTransform_.scale_ = {scale, scale, 1.f};

		for (KamataEngine::WorldTransform& transform : ellipseWorldTransforms_) {
			transform.scale_ = {scale2, scale1, 1.0f};
		}

		if (timer_ >= duration_) {
			timer_ = 0;
			status_ = HitEffect::Status::kFadeOut;
		}

		break;
	}
	case HitEffect::Status::kFadeOut:
		alpha_ = 1.f-  std::clamp(float(timer_ / duration_), 0.f, 1.f);

		if (timer_ >= duration_) {
			timer_ = 0;
			status_ = HitEffect::Status::kFinished;
		}

		
		
		break;
	}

	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

	for (KamataEngine::WorldTransform& transform : ellipseWorldTransforms_) {
		transform.matWorld_ = CreateAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
		transform.TransferMatrix();
	}
}

void HitEffect::Draw() {
	if (model_ && camera_) {
		model_->SetAlpha(alpha_);
		KamataEngine::DebugText::GetInstance()->ConsolePrintf("draw\n");
		model_->Draw(circleWorldTransform_, *camera_);
		for (const KamataEngine::WorldTransform& transform : ellipseWorldTransforms_) {
			model_->Draw(transform, *camera_);
		}
	}
}