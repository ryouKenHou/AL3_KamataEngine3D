#pragma once

#include "KamataEngine.h"
#include <vector>
#include <map>



enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
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


	float GetBlockWidth() const { return kBlockWidth; }
	float GetBlockHeight() const { return kBlockHeight; }
	static uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
	static uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }


};

