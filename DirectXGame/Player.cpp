#include "Player.h"
#include "helper.hpp"
#include "MapChipField.h"
#include <numbers>
#include <algorithm>
#include <array>

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

void Player::MoveInput() {
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

	
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	KamataEngine::Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.f, -kHeight / 2.f, 0.f}, // kRightBottom
	    {-kWidth / 2.f, -kHeight / 2.f, 0.f}, // kLeftBottom
	    {+kWidth / 2.f, +kHeight / 2.f, 0.f}, // kRightTop
	    {-kWidth / 2.f, +kHeight / 2.f, 0.f}  // kLeftTop
	};
	return KamataEngine::Vector3{
	    center.x + offsetTable[static_cast<uint32_t>(corner)].x, 
		center.y + offsetTable[static_cast<uint32_t>(corner)].y, 
		center.z + offsetTable[static_cast<uint32_t>(corner)].z
	};
}

void Player::MapCollsionUp(CollisionMapInfo* info) { 
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info->moveValue, static_cast<Corner>(i));
	}

	if (info->moveValue.y <= 0) {
		return;
	}
	
	MapChipType mapchipType;
	bool hit = false;

	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	DebugText::GetInstance()->ConsolePrintf("RightTop Index X:%d Y:%d\n", indexSet.xIndex, indexSet.yIndex);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		float playerMoveY = rect.bottom - (worldTransform_.translation_.y) - (kHeight / 2.f + 0.1f); 
		info->moveValue.y = playerMoveY;
		info->ceilingCollided = true;

	}

}

void Player::MoveAfterMapCollsionCheck(const CollisionMapInfo& info) { 
	worldTransform_.translation_ += info.moveValue; 
}

void Player::OnCeilingCollided(const CollisionMapInfo& info) {
	if (info.ceilingCollided) {
		DebugText::GetInstance()->ConsolePrintf("Ceiling Collided\n");
		velocity_.y = 0.f;
	}
}

void Player::MapCollsionDown(CollisionMapInfo* info) { (void)info; }
void Player::MapCollsionLeft(CollisionMapInfo* info) { (void)info; }
void Player::MapCollsionRight(CollisionMapInfo* info) { (void)info; }

void Player::MapCollision(CollisionMapInfo* info) {
	MapCollsionUp(info);
	MapCollsionDown(info);
	MapCollsionLeft(info);
	MapCollsionRight(info);
}

void Player::Update() {
	// 入力処理
	MoveInput();	

	// マップチップとの当たり判定
	CollisionMapInfo collisionInfo;
	collisionInfo.moveValue = velocity_;
	MapCollision(&collisionInfo);

	// 移動処理
	MoveAfterMapCollsionCheck(collisionInfo);	

	// 天井衝突時の処理
	OnCeilingCollided(collisionInfo);

	// 着地判定
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

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}