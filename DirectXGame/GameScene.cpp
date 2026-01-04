#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() { 
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("sample.png"); 

	// サウンドの読み込み
	soundHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	// サウンドの再生
	Audio::GetInstance()->PlayWave(soundHandle_, false, 1.0f);

	// 3D モデルの生成
	model_ = KamataEngine::Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の有効化
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクション行列を設定
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

void GameScene::Update() {
	// デバッグテキストの表示
#ifdef _DEBUG
	ImGui::Begin("Debug1");
	ImGui::Text("ryou %d.%d.%d", 2026, 1, 5);
	ImGui::End();
#endif

	// デバッグカメラの更新
	debugCamera_->Update();
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	// 3Dモデルの描画
	//model_->Draw(worldTransform_, camera_, textureHandle_);
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	//  3Dモデル描画後処理
	Model::PostDraw();
}