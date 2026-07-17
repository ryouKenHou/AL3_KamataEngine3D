#pragma once
#include "KamataEngine.h"
#include "helper.hpp"

class BaseEffect {
public:
	virtual ~BaseEffect() = default;
	virtual void Initialize(KamataEngine::Vector3 position) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsFinished() const = 0;

};