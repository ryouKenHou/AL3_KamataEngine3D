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

	// 敵キャラクターの解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	// デバッグカメラの解放
	delete debugCamera_;

	// スカイドームの解放
	delete skydome_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// カメラコントローラーの解放
	delete cameraController_;

	// デスパーティクルの解放
	delete deathParticles_;
}

void GameScene::Initialize() {
	// ゲームシーンの状態を初期化
	phase_ = Phase::kPlay;

	// カメラコントローラーの生成
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetMode(CameraController::Mode::kForcedScroll);

	// マップチップフィールドの生成と初期化
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/Blocks.csv");
	Rect CameraMovableArea = {11.0f, (MapChipField::GetNumBlockHorizontal() - 11.f) * mapChipField_->GetBlockWidth(), 6.0f, MapChipField::GetNumBlockVirtical() * mapChipField_->GetBlockHeight()};
	cameraController_->SetMovableArea(CameraMovableArea);

	// プレイヤーの生成と初期化
	player_ = new Player();
	playerModel_ = Model::CreateFromOBJ("player", true);
	Vector3 playerStartPosition = mapChipField_->GetMapChipPositionByIndex(1, 1);
	player_->Initialize(playerModel_, cameraController_->GetCamera(), playerStartPosition);
	player_->SetMapChipField(mapChipField_);

	cameraController_->SetTarget(player_);
	cameraController_->Reset();

	// 敵キャラクターの生成と初期化
	enemyModel_ = Model::CreateFromOBJ("enemy", true);
	for (int i = 0; i < enemyMax_; ++i) {
		Enemy* enemy = new Enemy();
		Vector3 enemyStartPosition = mapChipField_->GetMapChipPositionByIndex(10 + i * 5, 1);
		enemy->Initialize(enemyModel_, cameraController_->GetCamera(), enemyStartPosition);
		enemies_.push_back(enemy);
	}

	// ブロックモデルの生成と初期化
	blockModel_ = Model::CreateFromOBJ("block", true);
	GenerateBlocks();

	//  デバッグカメラの生成と初期化
	debugCamera_ = new DebugCamera(1280, 720);

	// スカイドームの生成と初期化
	skydomeModel_ = Model::CreateFromOBJ("skyDome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, cameraController_->GetCamera());

	// デスパーティクルの生成と初期化
	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);
	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(deathParticleModel_, cameraController_->GetCamera(), player_->GetWorldPosition());
}

void GameScene::Update() {

	switch (phase_) {
	case Phase::kPlay: {
		// プレイヤーの更新
		player_->Update();

		// 敵キャラクターの更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		CheckAllCollisions();

		// カメラコントローラーの更新
		cameraController_->Update();

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
			cameraController_->GetCamera()->matView = debugCamera_->GetCamera().matView;
			cameraController_->GetCamera()->matProjection = debugCamera_->GetCamera().matProjection;
			cameraController_->GetCamera()->TransferMatrix();
		} else {
			// 通常カメラ更新
			cameraController_->GetCamera()->UpdateMatrix();
		}

		if (player_->IsAlive() == false) {
			phase_ = Phase::kDeath;
			deathParticles_->Initialize(deathParticleModel_, cameraController_->GetCamera(), player_->GetWorldPosition());
		}

		break;
	}
	case Phase::kDeath: {
		// スカイドームの更新
		skydome_->Update();

		// 敵キャラクターの更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバッグカメラ更新
			debugCamera_->Update();
			// カメラにデバッグカメラをセット
			cameraController_->GetCamera()->matView = debugCamera_->GetCamera().matView;
			cameraController_->GetCamera()->matProjection = debugCamera_->GetCamera().matProjection;
			cameraController_->GetCamera()->TransferMatrix();
		} else {
			// 通常カメラ更新
			cameraController_->GetCamera()->UpdateMatrix();
		}

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

		if (deathParticles_ && deathParticles_->IsFinished()) {
			isFinished_ = true;
		}

		break;
	}
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
			blockModel_->Draw(*transform, *cameraController_->GetCamera());
		}
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// プレイヤーの描画
	player_->Draw();

	// 敵キャラクターの描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

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

void GameScene::CheckAllCollisions() {
#pragma region Player vs MapChipField
	{
		AABB aabb1, aabb2;
		aabb1 = player_->GetAABB();

		for (Enemy* e : enemies_) {
			aabb2 = e->GetAABB();
			if (AABB::CheckAABBCollision(aabb1, aabb2)) {
				player_->OnCollision(e);
				e->OnCollision(player_);
			}
		}
	}
#pragma endregion

#pragma region Player vs items

#pragma endregion

#pragma region PlayerBullet vs enemies

#pragma endregion
}