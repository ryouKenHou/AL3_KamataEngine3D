#pragma once
#include "KamataEngine.h"
#include "Player.h"

class GameScene {
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// プレイヤー
	Player* player_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

public:
	GameScene();
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};
