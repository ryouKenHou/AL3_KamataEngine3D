#pragma once
#include "KamataEngine.h"
#include "helper.hpp"
#include "BaseEnemy.h"

class Enemy final : public BaseEnemy {
public:
	enum class Behavior {
		kWalk,
		kDead,
		kUnknown,
	};


private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	static inline const float kWalkSpeed = 0.02f;
	static inline const float kWalkMotionAngleStart = 0.f;
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

public:
	Enemy();
	~Enemy();
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene) override;
	void Update() override;
	void Draw() override;

	AABB GetAABB();

	void OnCollision(Player* player) override;

	bool IsDead() const override { return isDead_; }

	void BehaviorDeadUpdate();
	void BehaviorDeadInitialize();

	void BehaviorWalkUpdate();
	void BehaviorWalkInitialize();

	bool IsCollisionDisabled() const override { return isCollisionDiabled_; }
};

