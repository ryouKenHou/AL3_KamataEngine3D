#include "Player.h"
#include "helper.hpp"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

Player::Player() {}
Player::~Player() {
	// 解放処理
	delete model_;
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	// 3D モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.f;

	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();

	// カメラのセット
	camera_ = camera;
}

void Player::Update() {
	// 移動入力
	if (onGround_) {
		

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_UP)) {

			// 左右移動
			Vector3 acceleration = {0.0f, 0.0f, 0.0f};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.f) {
					velocity_.x *= (1.f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.f) {
					velocity_.x *= (1.f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}		

			if (Input::GetInstance()->PushKey(DIK_UP)) {
				velocity_ += Vector3(0, kJumpAcceleration, 0);
			}

			velocity_ += acceleration;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velocity_.x *= (1.f - kAttenuation);
		}
	} else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = max(velocity_.y, -kLimitFallSpeed);
	}

	bool landing = false;

	if (velocity_.y < 0.f) {
		if (worldTransform_.translation_.y <= 1.f) {
			landing = true;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 1.f;
			velocity_.y = 0.f;
			velocity_.x *= (1.f - kAttenuation);
			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.f) {
		turnTimer_ -= 1.f/60.f;

		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.f, 
			std::numbers::pi_v<float>*3.f / 2.f 
		};

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = (kTimeTurn - turnTimer_) / kTimeTurn;
		worldTransform_.rotation_.y = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * t;
	}
	
	// 移動処理
	worldTransform_.translation_ += velocity_;

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}