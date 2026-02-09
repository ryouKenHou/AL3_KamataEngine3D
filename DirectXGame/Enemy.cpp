#include "Enemy.h"
#include "MapChipField.h"

#include "Player.h"
#include <algorithm>
#include <array>
#include <numbers>

Enemy::Enemy() {}
Enemy::~Enemy() {
	// 解放処理
	delete model_;
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
	walkTimer_ += 1 / 60.f;
	float param = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.f) / 2.f;
	float radian = degree * std::numbers::pi_v<float> / 180.f;
	worldTransform_.rotation_.x = radian;
	// worldTransform_.rotation_.z = radian;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.f / 2.f;
	worldTransform_.translation_.x += velocity_.x;

	// ワールドトランスフォームの更新
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
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

void Enemy::OnCollision(Player* player) { (void)player; }