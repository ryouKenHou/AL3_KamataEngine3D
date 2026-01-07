#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "vector"
#include "helper.hpp"

class GameScene {
private:
	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;
	uint32_t playerTextureHandle_ = 0;

	// ブロックのモデル
	KamataEngine::Model* blockModel_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> blockWorldTransforms_;
	uint32_t blockTextureHandle_ = 0;

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
