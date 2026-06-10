#pragma once

#include "KamataEngine.h"
#include "Model2.h"

// ゲームシーン
class GameScene {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// カメラ
	KamataEngine::Camera camera_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 四角形
	KamataEngine::Model2* modelSquare_ = nullptr;
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
};
