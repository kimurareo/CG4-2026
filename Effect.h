#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

class Effect {
public: // メンバ関数
	/// 初期化
	// void Initialize(Model* model, Vector3 position, float rotate, float size);
	// void Initialize(KamataEngine::Model* model, float rotate, float size);
	void Initialize(Model* model, float rotate, float size, Vector3 position, Vector3 color);
	// void Initialize(KamataEngine::Model* model);

	/// 毎フレーム処理
	void Update();

	/// 描画
	void Draw(Camera& camera);

	// デスフラグのgetter
	bool IsFinished() const { return isFinished_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// 存続時間（消滅までの時間）<秒>
	static inline const float kDuration = 0.5f;

	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_ = {};
};
