#pragma once
#include "KamataEngine.h"

class GameScene {
private:
	uint32_t textureHandle_ = 0;	
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
