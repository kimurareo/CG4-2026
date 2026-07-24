#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

class HitEffect {
public:
	// 初期化
	void Initialize(Model* model, Vector3 position);

	// 更新
	void Update();

	// 描画
	void Draw(Camera& camera);

	// 終了判定
	bool IsFinished() const;

private:
	// モデル
	Model* model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// 寿命
	int timer_ = 60;

	// 終了フラグ
	bool isFinished_ = false;
};