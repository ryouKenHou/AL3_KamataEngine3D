#pragma once
#include <KamataEngine.h>

inline KamataEngine::Matrix4x4 operator*(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	KamataEngine::Matrix4x4 result = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

inline KamataEngine::Matrix4x4 CreateAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotation, const KamataEngine::Vector3& translation) {
	KamataEngine::Matrix4x4 result;
	// スケーリング行列
	KamataEngine::Matrix4x4 scaleMatrix = KamataEngine::MathUtility::MakeScaleMatrix(scale);

	// 回転行列（オイラー角から）
	KamataEngine::Matrix4x4 rotationMatrix = KamataEngine::MathUtility::MakeRotateXMatrix(rotation.x);
	KamataEngine::Matrix4x4 rotationMatrixY = KamataEngine::MathUtility::MakeRotateYMatrix(rotation.y);
	KamataEngine::Matrix4x4 rotationMatrixZ = KamataEngine::MathUtility::MakeRotateZMatrix(rotation.z);

	// 平行移動行列
	KamataEngine::Matrix4x4 translationMatrix = KamataEngine::MathUtility::MakeTranslateMatrix(translation);

	// アフィン変換行列の合成
	result = scaleMatrix * rotationMatrixZ * rotationMatrixY * rotationMatrix * translationMatrix;

	return result;
}