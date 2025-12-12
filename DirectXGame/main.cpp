#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// KamataEngineの初期化
	KamataEngine::Initialize(L"LC1A_33_リョウ_ケン_ホウ");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ゲームシーンの生成と初期化
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	// ゲームループ
	while (true) {
		if (KamataEngine::Update()) {		
			break;
		}

		// ゲームシーンの更新
		gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	gameScene = nullptr;

	// KamataEngineの終了処理
	KamataEngine::Finalize();

	return 0;
}
