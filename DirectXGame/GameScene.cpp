#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	// プレイヤーの解放
	delete player_;

	// ブロックの解放
	for (auto& row : blockWorldTransforms_) {
		for (auto& transform : row) {
			delete transform;
		}
	}
	blockWorldTransforms_.clear();
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// プレイヤーの生成と初期化
	player_ = new Player();
	playerModel_ = Model::Create();
	playerTextureHandle_ = TextureManager::Load("uvChecker.png");
	player_->Initialize(playerModel_, playerTextureHandle_, &camera_);

	// ブロックモデルの生成と初期化
	const uint32_t kNumBlockHorizontal = 20;
	const uint32_t kNumBlockVertical = 10;
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	blockWorldTransforms_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		blockWorldTransforms_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockWorldTransforms_[i][j] = new WorldTransform();
			blockWorldTransforms_[i][j]->Initialize();
			blockWorldTransforms_[i][j]->translation_.x = kBlockWidth * j;
			blockWorldTransforms_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
	blockModel_ = Model::Create();
}

void GameScene::Update() {
	// プレイヤーの更新
	player_->Update();

	// ブロックの更新
	for (auto& row : blockWorldTransforms_) {
		for (WorldTransform* transform : row) {
			// アフィン変換行列の作成
			transform->scale_ = {1.0f, 1.0f, 1.0f};
			transform->rotation_ = {0.0f, 0.0f, 0.0f};

			transform->matWorld_ = CreateAffineMatrix(transform->scale_, transform->rotation_, transform->translation_);
			// 定数バッファに転送する
			transform->TransferMatrix();
		}
	}

}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	// ブロックの描画
	for (auto& row : blockWorldTransforms_) {
		for (WorldTransform* transform : row) {
			blockModel_->Draw(*transform, camera_);
		}
	}
	// プレイヤーの描画
	// player_->Draw();

	//  3Dモデル描画後処理
	Model::PostDraw();
}