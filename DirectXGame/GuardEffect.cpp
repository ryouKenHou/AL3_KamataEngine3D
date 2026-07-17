#include "GuardEffect.h"
#include <algorithm>

KamataEngine::Model* GuardEffect::model_ = nullptr;
KamataEngine::Camera* GuardEffect::camera_ = nullptr;
GuardEffect* GuardEffect::Create(KamataEngine::Vector3 position) {
	GuardEffect* instance = new GuardEffect();
	assert(instance);
	instance->Initialize(position);

	return instance;
}

void GuardEffect::Initialize(KamataEngine::Vector3 position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {1.f, 1.f, 1.f};
	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();


}

void GuardEffect::Update() {
	timer_++;
	switch (status_) {
	case GuardEffect::Status::kFadeIn: {
		
		alpha_ = 1.f;
		float scale = EaseOut(0.f, 1.0f, timer_ / duration_);

		circleWorldTransform_.scale_ = {scale, scale, 1.f};


		if (timer_ >= duration_) {
			timer_ = 0;
			status_ = GuardEffect::Status::kFadeOut;
		}

		break;
	}
	case GuardEffect::Status::kFadeOut:
		alpha_ = 1.f-  std::clamp(float(timer_ / duration_), 0.f, 1.f);

		if (timer_ >= duration_) {
			timer_ = 0;
			status_ = GuardEffect::Status::kFinished;
		}

		
		
		break;
	}

	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();


}

void GuardEffect::Draw() {
	if (model_ && camera_) {
		model_->SetAlpha(alpha_);
		KamataEngine::DebugText::GetInstance()->ConsolePrintf("draw\n");
		model_->Draw(circleWorldTransform_, *camera_);

	}
}