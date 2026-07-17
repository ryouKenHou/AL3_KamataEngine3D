#pragma once
#include "KamataEngine.h"
#include "helper.hpp"

class Player;
class GameScene;

class ShieldEnemy {
public:
	enum class Behavior {
		kWalk,
		kDead,
		kKnocked,
		kUnknown,
	};


private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	static inline const float kWalkSpeed = 0.02f;
	static inline const float kWalkMotionAngleStart = -45.f;
	static inline const float kWalkMotionAngleEnd = 45.f;
	static inline const float kWalkMotionTime = 2.f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	float walkTimer_ = 0.f;

	bool isDead_ = false;
	float deadTimer_ = 0.f;
	float deadDuration_ = 0.5f;

	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	bool isCollisionDiabled_ = false;
	GameScene* gameScene_ = nullptr;

	static inline const int kKnockedDuration = 20;
	static inline const float kKnockedAnlge = -45.f;
	int knockedTimer_ = 0;
	bool knockedRequest_ = false;

public:
	ShieldEnemy();
	~ShieldEnemy();
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene);
	void Update();
	void Draw();

	AABB GetAABB();

	void OnCollision(Player* player);

	bool IsDead() const { return isDead_; }

	void BehaviorDeadUpdate();
	void BehaviorDeadInitialize();

	void BehaviorWalkUpdate();
	void BehaviorWalkInitialize();

	void BehaviorKnockedUpdate();
	void BehaviorKnockedInitialize();

	bool IsCollisionDisabled() const { return isCollisionDiabled_; }
};

