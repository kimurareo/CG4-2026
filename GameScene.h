#pragma once

#include "Effect.h"
#include "KamataEngine.h"
#include <list>
#include <vector>

using namespace KamataEngine;

// シーン定義
enum class Scene {
	kTitle, // タイトル画面
	kGame,  // ゲーム画面
	kClear  // クリア画面
};

class GameScene {
public:
	~GameScene();
	void Initialize();
	void Update();
	void Draw();


private:
	// 現在のシーン
	Scene scene_ = Scene::kTitle;

	// カメラ
	Camera camera_;

	// 3Dモデル
	Model* modelPlayer_ = nullptr;
	Model* modelItem_ = nullptr;
	Model* modelEffect_ = nullptr;
	Model* modelBlock_ = nullptr;

	// プレイヤー関連
	WorldTransform playerTransform_;
	Vector3 playerSize_ = {1.0f, 1.0f, 1.0f};

	// 移動速度
	Vector3 playerVelocity_ = {0.0f, 0.0f, 0.0f};
	// 接地フラグ
	bool isGrounded_ = false;

	// 重力加速度
	const float kGravity_ = -0.01f;
	// ジャンプ初速
    const float kJumpVelocity = 0.3f;

	// プレイヤーの描画サイズ
	Vector3 playerScale_ = {0.5f, 0.5f, 0.5f};


	// アイテム関連
	WorldTransform itemTransform_;
	Vector3 itemSize_ = {1.0f, 1.0f, 1.0f};
	bool isItemActive_ = true;

	// ポインタ型の vector
	std::vector<WorldTransform*> blockTransforms_;
	Vector3 blockSize_ = {2.0f, 2.0f, 2.0f};

	// ブロックの描画サイズ
	Vector3 blockScale_ = {1.0f, 1.0f, 1.0f};

	// エフェクトリスト
	std::list<Effect*> effects_;

	// ブロックとの当たり判定と押し戻し処理（X軸用・Y軸用）
	void ResolveBlockCollisionX(WorldTransform& playerTransform, const Vector3& playerSize, const WorldTransform& blockTransform, const Vector3& blockSize, float moveX);

	void ResolveBlockCollisionY(WorldTransform& playerTransform, const Vector3& playerSize, const WorldTransform& blockTransform, const Vector3& blockSize, float moveY);


	// 内部処理関数
	void EffectBorn(KamataEngine::Vector3 position);
	void EffectBornTrail(KamataEngine::Vector3 position);
	bool CheckCollision(const WorldTransform& a, const Vector3& sizeA, const WorldTransform& b, const Vector3& sizeB);
	void Reset();
	void GenerateStage();
};