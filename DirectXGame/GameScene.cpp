#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	delete debugCamera_;
	delete player_;
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の有効化
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクション行列を設定
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());

	// プレイヤーの生成と初期化
	player_ = new Player();
	player_->Initialize(Model::Create(), TextureManager::Load("sample.png"), &camera_);
}

void GameScene::Update() {
	// デバッグカメラの更新
	debugCamera_->Update();

	player_->Update();
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	player_->Draw();

	//  3Dモデル描画後処理
	Model::PostDraw();
}