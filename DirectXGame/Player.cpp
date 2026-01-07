#include "Player.h"
#include "helper.hpp"

using namespace KamataEngine;

Player::Player() {}
Player::~Player() {
	// 解放処理
	delete model_;
}

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	assert(model);
	assert(camera);

	// テクスチャの読み込み
	textureHandle_ = textureHandle;

	// 3D モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {1.0f, 1.0f, 0.0f};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 1.57f, 0.0f};

	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();

	// カメラのセット
	camera_ = camera;
}

void Player::Update() {
	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_);
}