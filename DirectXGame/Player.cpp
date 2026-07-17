#include "Player.h"

#include "Enemy.h"
#include "ShieldEnemy.h"
#include "MapChipField.h"
#include <algorithm>
#include <array>
#include <numbers>

using namespace KamataEngine;

Player::Player() {}
Player::~Player() {
	// 解放処理
	delete model_;
}

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack,KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	// 3D モデルの生成
	model_ = model;
	modelAttack_ = modelAttack;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.f;

	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();

	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;
	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.f;
	worldTransformAttack_.matWorld_ = CreateAffineMatrix(worldTransformAttack_.scale_, worldTransformAttack_.rotation_, worldTransformAttack_.translation_);
	worldTransformAttack_.TransferMatrix();
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
	    center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
}

void Player::MapCollisionUp(CollisionMapInfo* info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info->moveValue, static_cast<Corner>(i));
	}

	if (info->moveValue.y <= 0) {
		return;
	}

	MapChipType mapchipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_, kRightTop));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float playerMoveY = rect.bottom - (worldTransform_.translation_.y) - (kHeight / 2.f + 0.1f);
			info->moveValue.y = playerMoveY;
			info->ceilingCollided = true;
			DebugText::GetInstance()->ConsolePrintf("ceiling collided\n");
		}
	}
}

void Player::MoveAfterMapCollisionCheck(const CollisionMapInfo& info) { worldTransform_.translation_ += info.moveValue; }

void Player::OnCeilingCollided(const CollisionMapInfo& info) {
	if (info.ceilingCollided) {
		// DebugText::GetInstance()->ConsolePrintf("Ceiling Collided\n");
		velocity_.y = 0.f;
	}
}

void Player::MapCollisionDown(CollisionMapInfo* info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info->moveValue, static_cast<Corner>(i));
	}

	if (info->moveValue.y >= 0) {
		return;
	}

	MapChipType mapchipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	// 左下
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	//DebugText::GetInstance()->ConsolePrintf("- checking|| position indexX:%d indexY:%d|| position x:%f y:%f\n", indexSet.xIndex, indexSet.yIndex, positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapchipType == MapChipType::kBlock  && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float playerMoveY = rect.top - (worldTransform_.translation_.y) + (kHeight / 2.f);
			info->moveValue.y = min(0.f, playerMoveY);

			info->grounded = true;
			DebugText::GetInstance()->ConsolePrintf("Grounded\n");
			/*DebugText::GetInstance()->ConsolePrintf("\n\n===================================\nGrounded, TopY:%f BottomY:%f\n", positionsNew[kLeftTop].y ,positionsNew[kLeftBottom].y );
			DebugText::GetInstance()->ConsolePrintf("Grounded, position new indexX:%d indexY:%d\n", indexSet.xIndex, indexSet.yIndex);
			DebugText::GetInstance()->ConsolePrintf("Grounded, translation X:%f Y:%f\n===================================\n\n", worldTransform_.translation_.x, worldTransform_.translation_.y);*/
		}
	}
}

void Player::OnGroundCollided(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.f) {
			onGround_ = false;
			DebugText::GetInstance()->ConsolePrintf("Jumped\n");

		} else {
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveValue, static_cast<Corner>(i));
			}

			MapChipType mapchipType;
			MapChipType mapChipTypeNext;
			bool hit = false;

			Vector3 gapY = {0.f, -1.0f, 0.f};

			// 左下
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom] + gapY);
			//DebugText::GetInstance()->ConsolePrintf("- checking|| position indexX:%d indexY:%d|| position x:%f y:%f\n", indexSet.xIndex, indexSet.yIndex, positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y);
			mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
			if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
				hit = true;
			}

			// 右下
			indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom] + gapY);
			mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
			if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				//DebugText::GetInstance()->ConsolePrintf("lefted ground, translation X:%f Y:%f\n", worldTransform_.translation_.x, worldTransform_.translation_.y);
				onGround_ = false;
				DebugText::GetInstance()->ConsolePrintf("lefted ground\n");
			}
		}
		canAttack_ = true;
	} else {
		if (info.grounded) {
			velocity_.y = 0.f;
			velocity_.x *= (1.f - kAttenuationLanding);
			onGround_ = true;
			canAttack_ = true;
			DebugText::GetInstance()->ConsolePrintf("Landed\n");
			//DebugText::GetInstance()->ConsolePrintf("Landed, translation X:%f Y:%f\n", worldTransform_.translation_.x, worldTransform_.translation_.y);
		}
	}
}

void Player::MapCollisionLeft(CollisionMapInfo* info) {
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info->moveValue, static_cast<Corner>(i));
	}

	if (info->moveValue.x <= 0) {
		return;
	}

	MapChipType mapchipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	// 右上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightTop]);
	// DebugText::GetInstance()->ConsolePrintf("RightTop Index X:%d Y:%d\n", indexSet.xIndex, indexSet.yIndex);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kRightBottom]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float playerMoveX = rect.left - (worldTransform_.translation_.x) - (kWidth / 2.f + 0.05f);
			info->moveValue.x = playerMoveX;
			info->wallCollided = true;

			//DebugText::GetInstance()->ConsolePrintf("left wall collided\n");
		}
	}
}

void Player::MapCollisionRight(CollisionMapInfo* info) {
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info->moveValue, static_cast<Corner>(i));
	}
	if (info->moveValue.x >= 0) {
		return;
	}
	MapChipType mapchipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftTop]);
	// DebugText::GetInstance()->ConsolePrintf("LeftTop Index X:%d Y:%d\n", indexSet.xIndex, indexSet.yIndex);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左下
	indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	if (hit) {
		indexSet = mapChipField_->GetMapChipIndexByPosition(positionsNew[kLeftBottom]);
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexByPosition(CornerPosition(worldTransform_.translation_, kRightBottom));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			float playerMoveX = rect.right - (worldTransform_.translation_.x) + (kWidth / 2.f + 0.05f);
			info->moveValue.x = playerMoveX;
			info->wallCollided = true;
			//DebugText::GetInstance()->ConsolePrintf("right wall collided\n");
		}
	}
}

void Player::OnWallCollided(const CollisionMapInfo& info) {
	if (info.wallCollided) {
		velocity_.x *= 1.f - kAttenuationWall;
		if (pushed_) {
			isDead_ = true;
		}
	}
}

void Player::MapCollision(CollisionMapInfo* info) {

	MapCollisionUp(info);
	MapCollisionDown(info);
	MapCollisionLeft(info);
	MapCollisionRight(info);
}

void Player::Update() {
	//DebugText::GetInstance()->ConsolePrintf("translation X:%f Y:%f\n", worldTransform_.translation_.x, worldTransform_.translation_.y);
	worldTransform_.translation_.z = -0.2f;

	if (knockBackRequest_) {
		behaviorRequest_ = Behavior::kNockBack;
		knockBackRequest_ = false;
	}

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kNockBack:
			BehaviorKnockBackInitialize();
			break;
		}
		behaviorRequest_= Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kNockBack:
		BehaviorKnockBackUpdate();
		break;
	}

	// マップチップとの当たり判定
	CollisionMapInfo collisionInfo;
	collisionInfo.moveValue = velocity_;
	MapCollision(&collisionInfo);
	// 移動処理
	MoveAfterMapCollisionCheck(collisionInfo);	
	// 地面衝突時の処理
	OnGroundCollided(collisionInfo);
	// 天井衝突時の処理
	OnCeilingCollided(collisionInfo);
	// 壁衝突時の処理
	OnWallCollided(collisionInfo);
	//worldTransform_.translation_.y = 0.09f;
	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Player::BehaviorKnockBackInitialize() {
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	knockBackParameter_ = 0;
	velocity_.x = 0;
}

void Player::KnockBackRequst() { knockBackRequest_ = true; }

void Player::BehaviorKnockBackUpdate() {
	knockBackParameter_++;
	velocity_.x = -0.2f;
	if (knockBackParameter_ >= knockBackParameterMax_) {
		behaviorRequest_ = Behavior::kRoot;
		knockBackParameter_ = 0;
	}
}

void Player::BehaviorRootUpdate() {
	// 入力処理
	MoveInput();

	pushed_ = false;
	// 旋回制御
	if (turnTimer_ > 0.f) {
		turnTimer_ -= 1.f / 60.f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.f, std::numbers::pi_v<float> * 3.f / 2.f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float t = (kTimeTurn - turnTimer_) / kTimeTurn;
		worldTransform_.rotation_.y = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * t;
	}

	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE) && canAttack_) {
		behaviorRequest_ = Behavior::kAttack; 
		canAttack_ = false;
	}
}

void Player::BehaviorAttackUpdate() {
	attackParameter_++;

	velocity_.y = 0.f;

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	worldTransformAttack_.matWorld_ = CreateAffineMatrix(worldTransformAttack_.scale_, worldTransformAttack_.rotation_, worldTransformAttack_.translation_);
	worldTransformAttack_.TransferMatrix();

	switch (attackPhace_) {
	case AttackPhace::kStart: {

		float t = static_cast<float>(attackParameter_) / kattackStartDuration;
		worldTransform_.scale_.z = EaseOut(1.f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.f, 1.6f, t);
		velocity_.x = 0;
		if (attackParameter_ >= kattackStartDuration) {
			attackPhace_ = AttackPhace::kAttack;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhace::kAttack: {

		float t = static_cast<float>(attackParameter_) / kattackAttackDuration;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);
		velocity_.x = EaseIn(0.f, kLimitRunSpeed * 5.f * (lrDirection_ == LRDirection::kRight ? 1.f : -1.f), t);
		if (attackParameter_ >= kattackAttackDuration) {
			attackPhace_ = AttackPhace::kEnd;
			attackParameter_ = 0;
		}
		break;
	}
	case AttackPhace::kEnd: {
		float t = static_cast<float>(attackParameter_) / kattackEndDuration;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.6f, 1.0f, t);
		velocity_.x = EaseOut(kLimitRunSpeed * 5.f * (lrDirection_ == LRDirection::kRight ? 1.f : -1.f), 0.f, t);

		if (attackParameter_ >= kattackEndDuration) {
			attackPhace_ = AttackPhace::kEnd;
			behaviorRequest_ = Behavior::kRoot;
			attackParameter_ = 0;
		}
		break;
	}
	}
}

void Player::BehaviorRootInitialize() {
	// 初期化処理
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	isAttacking_ = false;
}

void Player::BehaviorAttackInitialize() {
	// 初期化処理
	attackParameter_ = 0;
	attackPhace_ = AttackPhace::kStart;
	isAttacking_ = true;
}

void Player::Draw() {
	// 3Dモデルの描画
	if (isDead_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);
	if (behavior_ == Behavior::kAttack) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min.x = worldPos.x - kWidth / 2.f;
	aabb.min.y = worldPos.y - kHeight / 2.f;
	aabb.min.z = worldPos.z - kWidth / 2.f;
	aabb.max.x = worldPos.x + kWidth / 2.f;
	aabb.max.y = worldPos.y + kHeight / 2.f;
	aabb.max.z = worldPos.z + kWidth / 2.f;
	return aabb;
}

void Player::OnCollision(BaseEnemy* enemy) {

	(void)enemy;
	// velocity_ += Vector3(0, kJumpAcceleration, 0);

	if (isAttacking_) {
		return;
	}

	isDead_ = true;
}
