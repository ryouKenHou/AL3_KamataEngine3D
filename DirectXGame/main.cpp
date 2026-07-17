#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

enum class Scene {
	kUnkown = 0,

	kTitle,
	kGame,
};

Scene scene = Scene::kUnkown;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			scene = Scene::kGame;

			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		} else if (gameScene->IsReloadRequested()) {
			// ゲームシーンをリロードする
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// KamataEngineの初期化
	KamataEngine::Initialize(L"LC1A_33_リョウ_ケン_ホウ");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	titleScene = new TitleScene();
	titleScene->Initialize();

	scene = Scene::kTitle;

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

	// ゲームループ
	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imGuiManager->Begin();

		// ゲームシーンの更新
		ChangeScene();
		UpdateScene();

		// ImGui受付終了
		imGuiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		DrawScene();

		// ImGuiの描画
		imGuiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	delete titleScene;
	titleScene = nullptr;

	// KamataEngineの終了処理
	KamataEngine::Finalize();

	return 0;
}
