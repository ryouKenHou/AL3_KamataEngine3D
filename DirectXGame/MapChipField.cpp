#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank}, 
    {"1", MapChipType::kBlock},
};

}

MapChipField::MapChipField() { ResetMapChipData(); }
MapChipField::~MapChipField() {}

void MapChipField::ResetMapChipData() {
	mapChipData.data.clear();
	mapChipData.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& row : mapChipData.data) {
		row.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "Failed to open CSV file");

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData.data[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3{static_cast<float>(xIndex) * kBlockWidth, static_cast<float>(yIndex) * kBlockHeight, 0.0f};
}

 MapChipField::IndexSet MapChipField::GetMapChipIndexByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet;
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth/2) / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t>((position.y + kBlockHeight/2) / kBlockHeight);
	return indexSet;
}

 MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) { 
	 KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	 Rect rect;
	 rect.left = center.x - kBlockWidth / 2;
	 rect.right = center.x + kBlockWidth / 2;
	 rect.bottom = center.y - kBlockHeight / 2;
	 rect.top = center.y + kBlockHeight / 2;

	 return rect;
 }
