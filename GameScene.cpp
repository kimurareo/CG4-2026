#include "GameScene.h"
#include <random>

using namespace KamataEngine;
using namespace MathUtility;

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());
std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

// デストラクタ
GameScene::~GameScene() {
	// エフェクト
	for (Effect* effect : effects_) {
		delete effect;
	}
	effects_.clear();
}

// 初期化
void GameScene::Initialize() {
	// 乱数の初期化
	srand((unsigned)time(NULL));

	// 3Dモデルデータ生成
	modelEffect_ = Model::CreateFromOBJ("plane");

	// カメラの初期化
	camera_.Initialize();
}

// 更新
void GameScene::Update() {
	// エフェクト発生
	if (rand() % 5 == 0) {
		Vector3 position = {distribution(randomEngine), distribution(randomEngine), 0};
		position *= 10;
		EffectBorn(position);
	}

	// エフェクト更新
	// effect_->Update();
	for (Effect* effect : effects_) {
		effect->Update();
	}

	// デスフラグの立ったエフェクトを削除
	effects_.remove_if([](Effect* effect) {
		if (effect->IsFinished()) {
			delete effect;
			return true;
		}
		return false;
	});
}

// 描画
void GameScene::Draw() {
	// DirectXCommon インスタンスの取得
	//	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	//	Model::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw(); // 仕様変更

	// エフェクト描画
	for (Effect* effect : effects_) {
		effect->Draw(camera_);
	}

	// 3Dモデル描画後処理
	Model::PostDraw();
}

// エフェクト発生
void GameScene::EffectBorn(Vector3 position) {
	Vector3 color = {abs(distribution(randomEngine)), abs(distribution(randomEngine)), abs(distribution(randomEngine))};
	for (int32_t i = 0; i < 15; i++) {
		Effect* effect = new Effect();
		float rotate = distribution(randomEngine) * 3.14f;
		float size = 1.0f + abs(distribution(randomEngine)) * 4;
		effect->Initialize(modelEffect_, rotate, size, position, color);
		effects_.push_back(effect);
	}
}