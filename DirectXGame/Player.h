#pragma once
#include "KamataEngine.h"
#include "helper.hpp"

enum Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,	
	kLeftTop,

	kNumCorner
};

struct CollisionMapInfo {
	bool ceilingCollided = false;
	bool grounded = false;
	bool wallCollided = false;
	KamataEngine::Vector3 moveValue;
};

class MapChipField;

class Player {
private:
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kAttenuationWall = 0.3f;
	static inline const float kLimitRunSpeed = 0.2f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.02f;
	static inline const float kLimitFallSpeed = 0.3f;
	static inline const float kJumpAcceleration = 0.35f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	bool onGround_ = true;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	MapChipField* mapChipField_ = nullptr;

public:
	Player();
	~Player();

	void MoveInput();

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	void MapCollision(CollisionMapInfo* info);
	void MapCollisionUp(CollisionMapInfo* info);
	void MapCollisionDown(CollisionMapInfo* info);
	void MapCollisionLeft(CollisionMapInfo* info);
	void MapCollisionRight(CollisionMapInfo* info);

	void MoveAfterMapCollisionCheck(const CollisionMapInfo& info);
	void OnCeilingCollided(const CollisionMapInfo& info);
	void OnGroundCollided(const CollisionMapInfo& info);
	void OnWallCollided(const CollisionMapInfo& info);

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; } 
};