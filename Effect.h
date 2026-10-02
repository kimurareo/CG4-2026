#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

class Effect {
public:
	// エフェクトを初期化する
	void Initialize(Model* model, float rotate, float size, Vector3 position, Vector3 color);

	// 毎フレーム更新する
	void Update();

	// エフェクトを描画する
	void Draw(Camera& camera);

	// エフェクトが終了したか取得する
	bool IsFinished() const { return isFinished_; }

private:
	// エフェクトのワールド変換情報
	WorldTransform worldTransform_;

	// エフェクトに使用するモデル
	Model* model_ = nullptr;

	// エフェクト終了フラグ
	bool isFinished_ = false;

	// エフェクト生成からの経過時間
	float counter_ = 0.0f;

	// エフェクトの生存時間
	static inline const float kDuration = 0.5f;

	// 色変更用オブジェクト
	ObjectColor objectColor_;

	// 現在の色
	Vector4 color_ = {};

	// エフェクトの移動速度
	Vector3 velocity_ = {};
};