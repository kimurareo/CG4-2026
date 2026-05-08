#pragma once
#include "KamataEngine.h"
#include "Model2.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {

	public:
	GameScene();
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// カメラ
	Camera camera_;
	
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	Model2* model2 = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;
};
