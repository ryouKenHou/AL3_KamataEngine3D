#pragma once
#include "KamataEngine.h"

class GameScene {
private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// サウンドハンドル
	uint32_t soundHandle_ = 0;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

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
