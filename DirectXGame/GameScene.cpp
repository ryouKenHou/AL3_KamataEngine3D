#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	delete player_;
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// プレイヤーの生成と初期化
	player_ = new Player();
	player_->Initialize(Model::Create(), TextureManager::Load("sample.png"), &camera_);
}

void GameScene::Update() {
	player_->Update();
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	player_->Draw();

	//  3Dモデル描画後処理
	Model::PostDraw();
}