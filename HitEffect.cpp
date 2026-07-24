#include "HitEffect.h"

using namespace KamataEngine;

// 初期化
void HitEffect::Initialize(Model* model, Vector3 position) {

	model_ = model;

	worldTransform_.Initialize();

	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	worldTransform_.UpdateMatrix();

	timer_ = 60;
	isFinished_ = false;
}

// 更新
void HitEffect::Update() {

	// 少しずつ大きくする
	worldTransform_.scale_.x += 0.02f;
	worldTransform_.scale_.y += 0.02f;
	worldTransform_.scale_.z += 0.02f;

	worldTransform_.UpdateMatrix();

	timer_--;

	if (timer_ <= 0) {
		isFinished_ = true;
	}
}

// 描画
void HitEffect::Draw(Camera& camera) {
	model_->Draw(worldTransform_, camera); 
}

// 終了判定
bool HitEffect::IsFinished() const { return isFinished_; }