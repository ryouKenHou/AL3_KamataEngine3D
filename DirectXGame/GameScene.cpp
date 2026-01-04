#include "GameScene.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {

	delete model_;
}

void GameScene::Initialize() { 
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("sample.png"); 

	// 3D モデルの生成
	model_ = KamataEngine::Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();
}

void GameScene::Update() {
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	// 3Dモデルの描画
	model_->Draw(worldTransform_, camera_, textureHandle_);

	//  3Dモデル描画後処理
	Model::PostDraw();
}