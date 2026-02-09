#include "DeathParticles.h"
#include "helper.hpp"
#include <algorithm>

using namespace KamataEngine;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Vector3 position) {
	for (auto& transform : worldTransforms_) {
		transform.Initialize();
		transform.translation_ = position;
	}

	// 3D モデルの生成
	model_ = model;

	// カメラのセット
	camera_ = camera;

	objectColor_.Initialize();
	color_ = {1.f, 1.f, 1.f, 1.f};
}

void DeathParticles::Update() {
	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		Vector3 velocity = {kSpeed, 0, 0};

		float angle = kAngleUnit * static_cast<float>(i);

		Matrix4x4 rotationMatrixZ = MathUtility::MakeRotateZMatrix(angle);

		velocity = MathUtility::TransformNormal(velocity, rotationMatrixZ);

		worldTransforms_[i].translation_ += velocity;
	}

	counter_ += 1.f / 60.f;

	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	color_.w = std::clamp(1.f-(counter_ / kDuration), 0.f, 1.f);
	objectColor_.SetColor(color_);

	for (auto& transform : worldTransforms_) {
		transform.matWorld_ = CreateAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
		transform.TransferMatrix();
	}
}

void DeathParticles::Draw() {
	if (isFinished_) {
		return;
	}

	for (auto& transform : worldTransforms_) {
		// 3Dモデルの描画
		model_->Draw(transform, *camera_, &objectColor_);
	}
}