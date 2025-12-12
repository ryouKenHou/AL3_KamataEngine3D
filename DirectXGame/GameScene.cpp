#include "GameScene.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {}

void GameScene::Initialize() { 
	textureHandle_ = TextureManager::Load("sample.png"); 
}

void GameScene::Update() {}

void GameScene::Draw() {}