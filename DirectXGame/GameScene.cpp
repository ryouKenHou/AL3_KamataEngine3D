#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	// プレイヤーの解放
	delete player_;

	// ブロックの解放
	for (auto& row : blockWorldTransforms_) {
		for (auto& transform : row) {
			delete transform;
		}
	}
	blockWorldTransforms_.clear();

	// デバッグカメラの解放
	delete debugCamera_;

	// スカイドームの解放
	delete skydome_;

	// マップチップフィールドの解放
	delete mapChipField_;
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// プレイヤーの生成と初期化
	player_ = new Player();
	playerModel_ = Model::CreateFromOBJ("player", true);
	Vector3 playerStartPosition = mapChipField_->GetMapChipPositionByIndex(1,1);
	player_->Initialize(playerModel_, &camera_, playerStartPosition);

	// マップチップフィールドの生成と初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/Blocks.csv");

	// ブロックモデルの生成と初期化
	blockModel_ = Model::CreateFromOBJ("block", true);
	GenerateBlocks();

	//  デバッグカメラの生成と初期化
	debugCamera_ = new DebugCamera(1280, 720);

	// スカイドームの生成と初期化
	skydomeModel_ = Model::CreateFromOBJ("skyDome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, &camera_);
}

void GameScene::Update() {
	// プレイヤーの更新
	player_->Update();

	// ブロックの更新
	for (auto& row : blockWorldTransforms_) {
		for (WorldTransform* transform : row) {
			if (!transform) {
				continue;
			}
			// アフィン変換行列の作成
			transform->scale_ = {1.0f, 1.0f, 1.0f};
			transform->rotation_ = {0.0f, 0.0f, 0.0f};

			transform->matWorld_ = CreateAffineMatrix(transform->scale_, transform->rotation_, transform->translation_);
			// 定数バッファに転送する
			transform->TransferMatrix();
		}
	}

	// スカイドームの更新
	skydome_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG

	if (isDebugCameraActive_) {
		// デバッグカメラ更新
		debugCamera_->Update();
		// カメラにデバッグカメラをセット
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		// 通常カメラ更新
		camera_.UpdateMatrix();
	}
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	// スカイドームの描画
	skydome_->Draw();

	// ブロックの描画
	for (auto& row : blockWorldTransforms_) {
		for (WorldTransform* transform : row) {
			if (!transform) {
				continue;
			}
			blockModel_->Draw(*transform, camera_);
		}
	}

	// プレイヤーの描画
	player_->Draw();

	//  3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::GenerateBlocks() {
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	const uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVirtical();

	blockWorldTransforms_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		blockWorldTransforms_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t y = 0; y < kNumBlockVertical; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			if (mapChipField_->GetMapChipTypeByIndex(x, y) == MapChipType::kBlock) {
				WorldTransform* transform = new WorldTransform();
				transform->Initialize();
				blockWorldTransforms_[y][x] = transform;
				blockWorldTransforms_[y][x]->translation_ = mapChipField_->GetMapChipPositionByIndex(x, y);
			}
		}
	}
}