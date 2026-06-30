#pragma once
#include "2d/ImGuiManager.h"
#include "KamataEngine.h"
#include "helper.hpp"
#include "vector"

class Fade {
public:
	enum class Status {
		kNone,
		kFadeIn,
		kFadeOut,
	};

private:
	KamataEngine::Sprite* sprite_ = nullptr;
	Status status_ = Status::kNone;

	float duration_ = 0.0f; // フェードの継続時間（秒）
	float counter_ = 0.0f;  // フェードの経過時間（秒）

public:
	~Fade() { delete sprite_; }

	void Initialize();

	void Update();

	void Draw();

	void Start(Status status, float duartion);

	void Stop();

	bool IsFinished() const;
};