#include "TitleScene.h"

void TitleScene::Initialize() {
	// タイトルシーンの初期化処理
	playerModel_ = KamataEngine::Model::CreateFromOBJ("player", true);
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ = {0.f, -2.f, -40.f};
	playerWorldTransform_.rotation_.y = 3.141592654f;

	titleModel_ = KamataEngine::Model::CreateFromOBJ("titleFont", true);
	titleWorldTransform_.Initialize();
	titleWorldTransform_.translation_ = {0.f, 5.f, -1.f};
	// titleWorldTransform_.rotation_.y = 3.141592654f;
	titleWorldTransform_.scale_ = {1.5f, 1.5f, 1.5f};

	camera_ = new KamataEngine::Camera();
	camera_->Initialize();
	camera_->translation_.z = -15.f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::kFadeIn, 1.0f);
}

void TitleScene::Update() {
	frameCounter_++;
	// タイトルシーンの更新処理
	playerWorldTransform_.matWorld_ = CreateAffineMatrix(playerWorldTransform_.scale_, playerWorldTransform_.rotation_, playerWorldTransform_.translation_);
	playerWorldTransform_.TransferMatrix();

	titleWorldTransform_.translation_.y += sinf(frameCounter_ * 0.05f) * 0.08f;

	titleWorldTransform_.matWorld_ = CreateAffineMatrix(titleWorldTransform_.scale_, titleWorldTransform_.rotation_, titleWorldTransform_.translation_);
	titleWorldTransform_.TransferMatrix();

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::kFadeOut, 1.0f);
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}

	fade_->Update();
}

void TitleScene::Draw() {
	KamataEngine::Model::PreDraw();
	// タイトルシーンの描画処理
	playerModel_->Draw(playerWorldTransform_, *camera_);
	titleModel_->Draw(titleWorldTransform_, *camera_);
	KamataEngine::Model::PostDraw();

	fade_->Draw();
}