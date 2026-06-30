#pragma once
#include <KamataEngine.h>
#include "helper.hpp"

class Player;

struct Rect {
	float left = 0.f;
	float right = 1.f;
	float bottom = 0.f;
	float top = 1.f;
};

class CameraController {
public:
	enum class Mode {
		kFollow,
		kForcedScroll,
	};

private:
	KamataEngine::Camera* camera_ = nullptr;

	Player* target_ = nullptr;
	KamataEngine::Vector3 offset_ = {0.f, 0.f, -15.f};

	Mode mode_ = Mode::kFollow;
	Rect movableArea_ = {0, 20, 0, 100};
	KamataEngine::Vector3 targetPosition_ = {0.f, 0.f, 0.f};

	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 15.f;
	static inline const Rect marginArea = {-10.f, 10.f, -10.f, 10.f};
	static inline const float kForcedScrollSpeed = 0.02f;

public:
	CameraController();
	~CameraController();
	void Initialize();
	void Update();
	void Reset();

	KamataEngine::Camera* GetCamera() const { return camera_; }
	void SetTarget(Player* target) { target_ = target; }
	void SetMovableArea(const Rect& area) { movableArea_ = area; }

	void SetMode(Mode mode) { mode_ = mode; }
	Mode GetMode() const { return mode_; }
};
