#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	assert(model);
	assert(camera);

	// テクスチャの読み込み
	textureHandle_ = textureHandle;

	// 3D モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのセット
	camera_ = camera;
}

void Player::Update() {
	// ワールドトランスフォームの更新
	worldTransform_.TransferMatrix();
}

void Player::Draw() {
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}