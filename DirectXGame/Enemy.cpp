#include "Enemy.h"
#include "MapChipField.h"

#include "Player.h"
#include <algorithm>
#include <array>
#include <numbers>

Enemy::Enemy() {}
Enemy::~Enemy() {
}

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
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

	// 初速度の設定
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.f;
}

void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDead:
			BehaviorDeadInitialize();
			break;
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
	}

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Enemy::BehaviorDeadUpdate() {
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

void Enemy::BehaviorDeadInitialize() {
	// 死亡時の初期化処理を実装する場合はここに記述
	deadTimer_ = 0.f;
	isCollisionDiabled_ = true; // 衝突判定を無効化
}

void Enemy::BehaviorWalkUpdate() {
	walkTimer_ += 1 / 60.f;
	float param = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.f) / 2.f;
	float radian = degree * std::numbers::pi_v<float> / 180.f;
	worldTransform_.rotation_.x = radian;
	// worldTransform_.rotation_.z = radian;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.f / 2.f;
	worldTransform_.translation_.x += velocity_.x;
}

void Enemy::BehaviorWalkInitialize() {
	// 歩行時の初期化処理を実装する場合はここに記述
}


void Enemy::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}

AABB Enemy::GetAABB() {
	AABB aabb;
	aabb.min.x = worldTransform_.translation_.x - kWidth / 2.f;
	aabb.min.y = worldTransform_.translation_.y - kHeight / 2.f;
	aabb.min.z = worldTransform_.translation_.z - kWidth / 2.f;
	aabb.max.x = worldTransform_.translation_.x + kWidth / 2.f;
	aabb.max.y = worldTransform_.translation_.y + kHeight / 2.f;
	aabb.max.z = worldTransform_.translation_.z + kWidth / 2.f;
	return aabb;
}

void Enemy::OnCollision(Player* player) { 
	(void)player;
	behaviorRequest_ = Behavior::kDead;
}