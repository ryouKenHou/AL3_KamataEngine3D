#include "Skydome.h"

using namespace KamataEngine;

Skydome::Skydome() {}
Skydome::~Skydome() {
	// 解放処理
	delete model_;
}

void Skydome::Initialize(Model* model, Camera* camera) {
	assert(model);
	assert(camera);

	// 3D モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのセット
	camera_ = camera;
}

void Skydome::Update() {
	// 更新処理
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 描画処理
	model_->Draw(worldTransform_, *camera_);
}