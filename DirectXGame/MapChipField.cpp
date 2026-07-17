#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

namespace {

std::map<char, MapChipType> mapChipTypeTable = {
    {'B', MapChipType::kBlock},
    {'P', MapChipType::kPlayer},
    {'E', MapChipType::kEnemy },
};

}

MapChipField::MapChipField() { ResetMapChipData(); }
MapChipField::~MapChipField() {}

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipDataUnit>& row : mapChipData_.data) {
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

			if (word.empty()) {
				continue;
			}

			if (!mapChipTypeTable.contains(word[kChipType])) {
				continue;
			}

			mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			if (word.size() <= kChipSubID) {
				continue;
			}

			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');

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
	return mapChipData_.data[yIndex][xIndex].type;
}

uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return 0;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return 0;
	}
	return mapChipData_.data[yIndex][xIndex].subID;
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3{static_cast<float>(xIndex) * kBlockWidth, (kNumBlockVirtical - 1 - static_cast<float>(yIndex)) * kBlockHeight, 0.0f};
}

MapChipField::IndexSet MapChipField::GetMapChipIndexByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet;
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(((position.y + kBlockHeight / 2) / kBlockHeight));
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
