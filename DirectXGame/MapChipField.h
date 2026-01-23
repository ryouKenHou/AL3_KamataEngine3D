#pragma once

#include "KamataEngine.h"
#include <map>
#include <vector>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData;

public:
	MapChipField();
	~MapChipField();

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipField::IndexSet GetMapChipIndexByPosition(const KamataEngine::Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	float GetBlockWidth() const { return kBlockWidth; }
	float GetBlockHeight() const { return kBlockHeight; }
	static uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
	static uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }

	// operator == and != for IndexSet
	friend bool operator==(const IndexSet& lhs, const IndexSet& rhs) { return lhs.xIndex == rhs.xIndex && lhs.yIndex == rhs.yIndex; }
	friend bool operator!=(const IndexSet& lhs, const IndexSet& rhs) { return !(lhs == rhs); }
};