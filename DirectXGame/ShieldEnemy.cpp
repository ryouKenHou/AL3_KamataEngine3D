#include "ShieldEnemy.h"
#include "MapChipField.h"

#include "GameScene.h"
#include "Player.h"
#include <algorithm>
#include <array>
#include <numbers>

ShieldEnemy::ShieldEnemy() {}
ShieldEnemy::~ShieldEnemy() {}

void ShieldEnemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene) {
	assert(model);
	assert(camera);
	assert(gameScene);

	gameScene_ = gameScene;

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

	// 初速度の設定
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.f;
}

void ShieldEnemy::Update() {
	if (knockedRequest_) {
		behaviorRequest_ = Behavior::kKnocked;
		knockedRequest_ = false;
	}

	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeadInitialize();
			break;
		case Behavior::kKnocked:
			BehaviorKnockedInitialize();
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDead:
		BehaviorDeadUpdate();
		break;
	case Behavior::kKnocked:
		BehaviorKnockedUpdate();
	}

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void ShieldEnemy::BehaviorKnockedInitialize() {
	knockedTimer_ = 0;
	velocity_.x = 0;
}

void ShieldEnemy::BehaviorKnockedUpdate() {
	knockedTimer_++;
	if (knockedTimer_ >= kKnockedDuration) {
		behaviorRequest_ = Behavior::kWalk;
		knockedTimer_ = 0;
	}
	float t = sinf(3.14159f / kKnockedDuration * knockedTimer_) * kKnockedAnlge * std::numbers::pi_v<float> / 180.f;
	worldTransform_.rotation_.x = t;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.f / 2.f;

}

void ShieldEnemy::BehaviorDeadUpdate() {
	// 死亡時の挙動を実装する場合はここに記述
	deadTimer_ += 1 / 60.f;
	if (deadTimer_ >= deadDuration_) {
		isDead_ = true;
	}

	float t = sinf(3.14159f / deadDuration_ * deadTimer_) * 3.14159f / 2.f;
	worldTransform_.rotation_.x = t;
	worldTransform_.rotation_.y = t;
	worldTransform_.translation_.y -= 0.01f; // 落下するようにY座標を減少させる
}

void ShieldEnemy::BehaviorDeadInitialize() {
	// 死亡時の初期化処理を実装する場合はここに記述
	deadTimer_ = 0.f;
	isCollisionDiabled_ = true; // 衝突判定を無効化
}

void ShieldEnemy::BehaviorWalkUpdate() {
	walkTimer_ += 1 / 60.f;
	float param = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.f) / 2.f;
	float radian = degree * std::numbers::pi_v<float> / 180.f;
	//worldTransform_.rotation_.z = radian;
	// worldTransform_.rotation_.z = radian;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.f / 2.f + radian;
	worldTransform_.translation_.x += velocity_.x;
}

void ShieldEnemy::BehaviorWalkInitialize() {
	// 歩行時の初期化処理を実装する場合はここに記述
	velocity_ = {-kWalkSpeed, 0, 0};
}

void ShieldEnemy::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}

AABB ShieldEnemy::GetAABB() {
	AABB aabb;
	aabb.min.x = worldTransform_.translation_.x - kWidth / 2.f;
	aabb.min.y = worldTransform_.translation_.y - kHeight / 2.f;
	aabb.min.z = worldTransform_.translation_.z - kWidth / 2.f;
	aabb.max.x = worldTransform_.translation_.x + kWidth / 2.f;
	aabb.max.y = worldTransform_.translation_.y + kHeight / 2.f;
	aabb.max.z = worldTransform_.translation_.z + kWidth / 2.f;
	return aabb;
}

void ShieldEnemy::OnCollision(Player* player) {
	if (player->IsAttacking()) {
		if (player->GetLRDirection() == LRDirection::kRight) {
			gameScene_->CreateGuardEffect((worldTransform_.translation_ + player->GetWorldPosition()) / 2.f);

			player->KnockBackRequst();
			knockedRequest_ = true;
			return;
		}

		behaviorRequest_ = Behavior::kDead;

		KamataEngine::Vector3 effectPosition = (worldTransform_.translation_ + player->GetWorldPosition()) / 2.f;
		gameScene_->CreateHitEffect(effectPosition);
	}
}