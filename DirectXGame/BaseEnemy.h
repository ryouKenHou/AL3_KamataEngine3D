#pragma once
#include "KamataEngine.h"
#include "helper.hpp"

class GameScene;
class Player;

class BaseEnemy {
public:
	virtual ~BaseEnemy() = default;

	virtual void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene) = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;
	virtual void OnCollision( Player* player) = 0;

	virtual AABB GetAABB() = 0;

	virtual bool IsCollisionDisabled() const = 0;
	virtual bool IsDead() const = 0;
};