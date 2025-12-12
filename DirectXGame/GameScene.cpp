#include "GameScene.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	// スプライト解放
	delete sprite_;
}

void GameScene::Initialize() { 
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("sample.png"); 

	// スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100.0f, 50.0f});
}

void GameScene::Update() {}

void GameScene::Draw() {}