#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "vector"
#include "CameraController.h"
#include "helper.hpp"
#include "list"
#include "DeathParticles.h"
#include "Fade.h"

class GameScene {
public:
	enum class Phase{
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

private:
	// ゲームシーンの状態
	Phase phase_;

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* playerAttackModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* deathParticleModel_ = nullptr;

	// ブロックのモデル
	KamataEngine::Model* blockModel_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> blockWorldTransforms_;

	// 敵キャラクター
	int enemyMax_ = 1;
	std::list<Enemy*> enemies_;
	KamataEngine::Model* enemyModel_ = nullptr;

	// カメラ
	CameraController* cameraController_ = nullptr;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;

	// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	bool isFinished_ = false;

	Fade* fade_ = nullptr;

public:
	GameScene();
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return isFinished_; }
};
