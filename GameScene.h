#pragma once

#include "Effect.h"
#include "KamataEngine.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// カメラ
	Camera camera_;

	// 3Dモデル エフェクト
	Model* modelEffect_ = nullptr;

	// エフェクト
	std::list<Effect*> effects_;

	// エフェクト発生
	void EffectBorn(KamataEngine::Vector3 position);
};
