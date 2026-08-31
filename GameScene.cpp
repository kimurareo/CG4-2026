#include "GameScene.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace KamataEngine;

// ============================================================
// デストラクタ
// ============================================================
// GameSceneでnewしたモデル・ブロック・エフェクトを全て解放する。
// 解放し忘れるとメモリリークの原因になる。
GameScene::~GameScene() {

	// 3Dモデルのメモリを解放
	delete modelPlayer_;
	delete modelItem_;
	delete modelEffect_;
	delete modelBlock_;

	// --------------------------------------------------------
	// ステージのブロックを解放
	// --------------------------------------------------------
	// blockTransforms_にはnewで生成したWorldTransformの
	// ポインタが入っているため、1つずつdeleteする。
	for (WorldTransform* block : blockTransforms_) {
		delete block;
	}

	// 解放済みのポインタをvectorから全て削除
	blockTransforms_.clear();

	// --------------------------------------------------------
	// エフェクトを解放
	// --------------------------------------------------------
	// effects_にもnewで生成したEffectのポインタが入っている。
	for (Effect* effect : effects_) {
		delete effect;
	}

	// 解放済みのポインタをlistから全て削除
	effects_.clear();
}

// ============================================================
// 初期化処理
// ============================================================
// ゲーム開始時に1回だけ呼び出して、
// モデル・カメラ・ステージ・プレイヤーなどを初期化する。
void GameScene::Initialize() {

	// 乱数の初期化
	// 現在時刻を種として設定することで、
	// 毎回同じ乱数にならないようにする。
	srand(static_cast<unsigned int>(time(NULL)));

	// --------------------------------------------------------
	// 3Dモデルの生成
	// --------------------------------------------------------
	modelPlayer_ = Model::CreateFromOBJ("cube");
	modelItem_ = Model::CreateFromOBJ("cube");
	modelEffect_ = Model::CreateFromOBJ("plane");
	modelBlock_ = Model::CreateFromOBJ("cube");

	// --------------------------------------------------------
	// カメラの初期化
	// --------------------------------------------------------
	camera_.Initialize();

	// --------------------------------------------------------
	// ステージの生成
	// --------------------------------------------------------
	// map配列の内容をもとにブロックを生成する。
	GenerateStage();

	// --------------------------------------------------------
	// プレイヤー・ゴール・エフェクトなどの初期状態を設定
	// --------------------------------------------------------
	Reset();
}

// ============================================================
// ステージ生成
// ============================================================
// map配列を読み取り、値が1の場所にブロックを生成する。
//
// 1 = ブロックあり
// 0 = ブロックなし
//
// 1マスの間隔は2.0f。
// そのため、ブロックの中心座標も2.0fずつ離れている。
void GameScene::GenerateStage() {

	// --------------------------------------------------------
	// 以前のステージが存在する場合は全て削除
	// --------------------------------------------------------
	for (WorldTransform* block : blockTransforms_) {
		delete block;
	}

	blockTransforms_.clear();

	// --------------------------------------------------------
	// ステージマップ
	// --------------------------------------------------------
	// 縦6マス × 横40マスのステージ。
	//
	// 配列の上側が高い位置、
	// 配列の下側が低い位置になる。
	//
	// 1が書かれている場所にブロックを配置する。
	const int map[6][40] = {
	    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

	    // 空中に浮いているブロック
	    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

	    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

	    // 段差
	    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},

	    // 床（一部穴あり）
	    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

	    // 床の土台
	    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	};

	// --------------------------------------------------------
	// ステージの基準座標
	// --------------------------------------------------------
	// x方向の開始位置
	float startX = -10.0f;

	// y方向の開始位置
	float startY = 4.0f;

	// --------------------------------------------------------
	// マップ配列を左上から順番に確認
	// --------------------------------------------------------
	for (int y = 0; y < 6; ++y) {

		for (int x = 0; x < 40; ++x) {

			// 1なら、その場所にブロックを生成する
			if (map[y][x] == 1) {

				// ブロック用のWorldTransformを動的生成
				WorldTransform* block = new WorldTransform();

				// WorldTransformを初期化
				block->Initialize();

				// ------------------------------------------------
				// ブロックの位置を設定
				// ------------------------------------------------
				// x方向は1マスにつき2.0f右へ移動。
				// y方向は1マスにつき2.0f下へ移動。
				block->translation_ = {startX + x * 2.0f, startY - y * 2.0f, 0.0f};

				// ------------------------------------------------
				// ブロックの描画サイズを設定
				// ------------------------------------------------
				block->scale_ = blockScale_;

				// 設定した位置・サイズを行列に反映
				block->UpdateMatrix();

				// 生成したブロックをvectorに登録
				blockTransforms_.push_back(block);
			}
		}
	}
}

// ============================================================
// リセット処理
// ============================================================
// プレイヤー、ゴール、エフェクトなどを
// ゲーム開始時の状態に戻す。
void GameScene::Reset() {

	// --------------------------------------------------------
	// プレイヤーの初期化
	// --------------------------------------------------------
	playerTransform_.Initialize();

	// プレイヤーの初期位置
	// 床の上に立つようにY座標を設定している。
	playerTransform_.translation_ = {-8.0f, -2.5f, 0.0f};

	// プレイヤーの描画サイズを設定
	playerTransform_.scale_ = playerScale_;

	// 設定した位置・サイズを行列に反映
	playerTransform_.UpdateMatrix();

	// --------------------------------------------------------
	// ゴールアイテムの初期化
	// --------------------------------------------------------
	itemTransform_.Initialize();

	// ゴールの位置
	itemTransform_.translation_ = {60.0f, -2.0f, 0.0f};

	// ゴールの描画サイズ
	itemTransform_.scale_ = itemSize_;

	// 行列を更新
	itemTransform_.UpdateMatrix();

	// ゴールを有効状態にする
	isItemActive_ = true;

	// --------------------------------------------------------
	// エフェクトを全て削除
	// --------------------------------------------------------
	for (Effect* effect : effects_) {
		delete effect;
	}

	effects_.clear();
}

// ============================================================
// 更新処理
// ============================================================
// 毎フレーム呼び出される。
// シーンごとの処理、プレイヤー移動、衝突判定、
// カメラ更新、エフェクト更新などを行う。
void GameScene::Update() {

	Input* input = Input::GetInstance();

	// ========================================================
	// 現在のシーンによって処理を分ける
	// ========================================================
	switch (scene_) {

	// --------------------------------------------------------
	// タイトル画面
	// --------------------------------------------------------
	case Scene::kTitle:

		// SPACEキーを押した瞬間にゲーム開始
		if (input->TriggerKey(DIK_SPACE)) {

			// ゲームの状態を初期化
			Reset();

			// ゲームシーンへ移動
			scene_ = Scene::kGame;
		}

		break;

	// --------------------------------------------------------
	// ゲーム画面
	// --------------------------------------------------------
	case Scene::kGame: {

		// ====================================================
		// プレイヤーの移動量を計算
		// ====================================================
		Vector3 move = {0.0f, 0.0f, 0.0f};

		// 1フレームあたりの基本移動速度
		float moveSpeed = 0.25f;

		// 左移動
		if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_A)) {
			move.x -= moveSpeed;
		}

		// 右移動
		if (input->PushKey(DIK_RIGHT) || input->PushKey(DIK_D)) {
			move.x += moveSpeed;
		}

		// 上移動
		if (input->PushKey(DIK_UP) || input->PushKey(DIK_W)) {
			move.y += moveSpeed;
		}

		// 下移動
		if (input->PushKey(DIK_DOWN) || input->PushKey(DIK_S)) {
			move.y -= moveSpeed;
		}

		// ====================================================
		// サブステップ処理
		// ====================================================
		// 1フレーム分の移動を複数回に分割して処理する。
		//
		// 一度に大きく移動すると、ブロックを飛び越えて
		// すり抜ける可能性がある。
		//
		// そこで移動量を4分割し、
		// 「少し移動 → 衝突判定」を4回繰り返す。
		const int kSubSteps = 4;

		// 1回のサブステップで移動する量
		Vector3 subMove = {move.x / static_cast<float>(kSubSteps), move.y / static_cast<float>(kSubSteps), 0.0f};

		// 4回に分けて移動・衝突判定を行う
		for (int i = 0; i < kSubSteps; ++i) {

			// =================================================
			// X軸方向の移動
			// =================================================
			if (subMove.x != 0.0f) {

				// プレイヤーをX方向へ少し移動
				playerTransform_.translation_.x += subMove.x;

				// 移動後の座標を行列へ反映
				playerTransform_.UpdateMatrix();

				// 全てのブロックとX方向の衝突判定
				for (WorldTransform* block : blockTransforms_) {

					ResolveBlockCollisionX(playerTransform_, playerSize_, *block, blockSize_, subMove.x);
				}
			}

			// =================================================
			// Y軸方向の移動
			// =================================================
			if (subMove.y != 0.0f) {

				// プレイヤーをY方向へ少し移動
				playerTransform_.translation_.y += subMove.y;

				// 移動後の座標を行列へ反映
				playerTransform_.UpdateMatrix();

				// 全てのブロックとY方向の衝突判定
				for (WorldTransform* block : blockTransforms_) {

					ResolveBlockCollisionY(playerTransform_, playerSize_, *block, blockSize_, subMove.y);
				}
			}
		}

		// ====================================================
		// 移動中の軌跡エフェクト
		// ====================================================
		// XまたはY方向に移動している場合のみ生成する。
		if (move.x != 0.0f || move.y != 0.0f) {

			EffectBornTrail(playerTransform_.translation_);
		}

		// ====================================================
		// ゴール判定
		// ====================================================
		// ゴールアイテムが有効な場合のみ判定する。
		if (isItemActive_) {

			// プレイヤーとゴールが重なったか確認
			if (CheckCollision(playerTransform_, playerSize_, itemTransform_, itemSize_)) {

				// ゴール地点でエフェクトを発生
				EffectBorn(itemTransform_.translation_);

				// ゴールアイテムを無効化
				isItemActive_ = false;

				// クリアシーンへ移動
				scene_ = Scene::kClear;
			}
		}

		break;
	}

	// --------------------------------------------------------
	// クリア画面
	// --------------------------------------------------------
	case Scene::kClear:

		// SPACEキーを押すとタイトル画面へ戻る
		if (input->TriggerKey(DIK_SPACE)) {

			scene_ = Scene::kTitle;
		}

		break;
	}

	// ========================================================
	// カメラのプレイヤー追従
	// ========================================================
	// プレイヤーの位置を基準にカメラを移動させる。
	//
	// X方向：
	// プレイヤーのX座標に合わせる
	//
	// Y方向：
	// プレイヤーより3.0f上に配置
	//
	// Z方向：
	// プレイヤーより25.0f後ろに配置
	Vector3 cameraOffset = {0.0f, 3.0f, -25.0f};

	// プレイヤーの位置 + カメラのオフセット
	camera_.translation_.x = playerTransform_.translation_.x + cameraOffset.x;

	camera_.translation_.y = playerTransform_.translation_.y + cameraOffset.y;

	camera_.translation_.z = playerTransform_.translation_.z + cameraOffset.z;

	// カメラの行列を更新
	camera_.UpdateMatrix();

	// ========================================================
	// エフェクト更新
	// ========================================================
	// 現在存在している全てのエフェクトを更新する。
	for (Effect* effect : effects_) {
		effect->Update();
	}

	// ========================================================
	// 終了したエフェクトを削除
	// ========================================================
	// IsFinished()がtrueになったエフェクトは、
	// deleteしてlistから削除する。
	effects_.remove_if([](Effect* effect) {
		if (effect->IsFinished()) {

			delete effect;

			return true;
		}

		return false;
	});
}

// ============================================================
// 描画処理
// ============================================================
// 現在のシーンに必要な3Dモデルやエフェクトを描画する。
void GameScene::Draw() {

	// 3D描画開始
	Model::PreDraw();

	// ========================================================
	// ゲーム画面・クリア画面の描画
	// ========================================================
	if (scene_ == Scene::kGame || scene_ == Scene::kClear) {

		// ----------------------------------------------------
		// プレイヤー描画
		// ----------------------------------------------------
		if (modelPlayer_) {

			modelPlayer_->Draw(playerTransform_, camera_);
		}

		// ----------------------------------------------------
		// ゴールアイテム描画
		// ----------------------------------------------------
		// ゴールがまだ有効な場合のみ描画する。
		if (isItemActive_ && modelItem_) {

			modelItem_->Draw(itemTransform_, camera_);
		}

		// ----------------------------------------------------
		// ステージブロック描画
		// ----------------------------------------------------
		if (modelBlock_) {

			// ステージ内の全ブロックを順番に描画
			for (WorldTransform* block : blockTransforms_) {

				modelBlock_->Draw(*block, camera_);
			}
		}
	}

	// ========================================================
	// エフェクト描画
	// ========================================================
	for (Effect* effect : effects_) {

		effect->Draw(camera_);
	}

	// 3D描画終了
	Model::PostDraw();
}

// ============================================================
// ゴール到達エフェクト生成
// ============================================================
// 指定した位置にゴール用のエフェクトを生成する。
void GameScene::EffectBorn(KamataEngine::Vector3 position) {

	// エフェクトモデルが存在しなければ何もしない
	if (modelEffect_ == nullptr) {
		return;
	}

	// 花火の粒の数
	const int kParticleCount = 30;

	// 30個のエフェクトを生成
	for (int i = 0; i < kParticleCount; ++i) {

		Effect* newEffect = new Effect();

		// ランダムな色を作る
		Vector3 color = {static_cast<float>(rand() % 100) / 100.0f, static_cast<float>(rand() % 100) / 100.0f, static_cast<float>(rand() % 100) / 100.0f};

		// エフェクトを初期化
		newEffect->Initialize(modelEffect_, 0.0f, 0.5f, position, color);

		// エフェクトリストに追加
		effects_.push_back(newEffect);
	}
}

// ============================================================
// 移動軌跡エフェクト生成
// ============================================================
// プレイヤーが移動している間、プレイヤーの位置に
// 小さなエフェクトを生成する。
void GameScene::EffectBornTrail(KamataEngine::Vector3 position) {

	// エフェクトモデルが存在する場合のみ生成
	if (modelEffect_) {

		// Effectを動的生成
		Effect* newEffect = new Effect();

		// 移動軌跡用のエフェクトを初期化
		newEffect->Initialize(modelEffect_, 0.0f, 0.5f, position, {0.8f, 0.8f, 1.0f});

		// エフェクトリストに追加
		effects_.push_back(newEffect);
	}
}

// ============================================================
// AABBによる当たり判定
// ============================================================
// AABB = Axis-Aligned Bounding Box
//
// 回転していない立方体を、
// X・Y・Z方向の範囲だけで判定する簡単な当たり判定。
//
// 2つのオブジェクトの中心間距離が、
// それぞれの半分のサイズの合計より小さい場合、
// オブジェクト同士が重なっていると判断する。
bool GameScene::CheckCollision(const WorldTransform& a, const Vector3& sizeA, const WorldTransform& b, const Vector3& sizeB) {

	// それぞれのオブジェクトの中心座標
	Vector3 posA = a.translation_;
	Vector3 posB = b.translation_;

	// --------------------------------------------------------
	// X・Y・Zの3方向全てで重なっているか確認
	// --------------------------------------------------------
	//
	// X方向で重なっている
	// ＋
	// Y方向で重なっている
	// ＋
	// Z方向で重なっている
	//
	// この3つを全て満たすと衝突している。
	if (std::abs(posA.x - posB.x) < (sizeA.x + sizeB.x) * 0.5f &&

	    std::abs(posA.y - posB.y) < (sizeA.y + sizeB.y) * 0.5f &&

	    std::abs(posA.z - posB.z) < (sizeA.z + sizeB.z) * 0.5f) {
		return true;
	}

	// どこか1方向でも重なっていなければ衝突していない
	return false;
}

// ============================================================
// X軸方向のブロック衝突処理
// ============================================================
// プレイヤーが左右方向へ移動したときに、
// ブロックと衝突しているか確認する。
//
// 衝突していた場合は、プレイヤーをX方向へ押し戻して、
// ブロックの中に入り込まないようにする。
void GameScene::ResolveBlockCollisionX(WorldTransform& playerTransform, const Vector3& playerSize, const WorldTransform& blockTransform, const Vector3& blockSize, float moveX) {

	// --------------------------------------------------------
	// 衝突判定の誤差を防ぐための余白
	// --------------------------------------------------------
	// プレイヤーをブロックの境界から少しだけ離す。
	constexpr float kEpsilon = 0.1f;

	// --------------------------------------------------------
	// プレイヤーとブロックの半分のサイズを計算
	// --------------------------------------------------------
	// 中心座標から左右・上下の端までの距離を求める。
	const float playerHalfX = playerSize.x * 0.5f;
	const float playerHalfY = playerSize.y * 0.5f;

	const float blockHalfX = blockSize.x * 0.5f;
	const float blockHalfY = blockSize.y * 0.5f;

	// --------------------------------------------------------
	// プレイヤーの各端の座標を計算
	// --------------------------------------------------------
	// 中心座標 ± 半分のサイズで、
	// プレイヤーの左・右・上・下の境界を求める。
	const float playerMinX = playerTransform.translation_.x - playerHalfX;

	const float playerMaxX = playerTransform.translation_.x + playerHalfX;

	const float playerMinY = playerTransform.translation_.y - playerHalfY;

	const float playerMaxY = playerTransform.translation_.y + playerHalfY;

	// --------------------------------------------------------
	// ブロックの各端の座標を計算
	// --------------------------------------------------------
	const float blockMinX = blockTransform.translation_.x - blockHalfX;

	const float blockMaxX = blockTransform.translation_.x + blockHalfX;

	const float blockMinY = blockTransform.translation_.y - blockHalfY;

	const float blockMaxY = blockTransform.translation_.y + blockHalfY;

	// --------------------------------------------------------
	// Y方向の重なりを確認
	// --------------------------------------------------------
	// プレイヤーとブロックが上下方向に重なっていない場合、
	// 左右からの衝突は発生しない。
	if (playerMaxY <= blockMinY || playerMinY >= blockMaxY) {
		return;
	}

	// --------------------------------------------------------
	// X方向の重なりを確認
	// --------------------------------------------------------
	// プレイヤーとブロックが左右方向に重なっていない場合、
	// 衝突していないので処理を終了する。
	if (playerMaxX <= blockMinX || playerMinX >= blockMaxX) {
		return;
	}

	// ========================================================
	// X方向の押し戻し
	// ========================================================

	// --------------------------------------------------------
	// 右方向へ移動してブロックに衝突した場合
	// --------------------------------------------------------
	// プレイヤーをブロックの左側まで戻す。
	if (moveX > 0.0f) {

		playerTransform.translation_.x = blockMinX - playerHalfX - kEpsilon;
	}

	// --------------------------------------------------------
	// 左方向へ移動してブロックに衝突した場合
	// --------------------------------------------------------
	// プレイヤーをブロックの右側まで戻す。
	else if (moveX < 0.0f) {

		playerTransform.translation_.x = blockMaxX + playerHalfX + kEpsilon;
	}

	// 押し戻した位置を行列へ反映
	playerTransform.UpdateMatrix();
}

// ============================================================
// Y軸方向のブロック衝突処理
// ============================================================
// プレイヤーが上下方向へ移動したときに、
// ブロックと衝突しているか確認する。
//
// 衝突していた場合はY方向へ押し戻す。
//
// これによって、
// ・床にめり込む
// ・天井にめり込む
// といった状態を防ぐ。
void GameScene::ResolveBlockCollisionY(WorldTransform& playerTransform, const Vector3& playerSize, const WorldTransform& blockTransform, const Vector3& blockSize, float moveY) {

	// --------------------------------------------------------
	// 衝突判定の誤差を防ぐための余白
	// --------------------------------------------------------
	constexpr float kEpsilon = 0.1f;

	// --------------------------------------------------------
	// プレイヤーとブロックの半分のサイズを計算
	// --------------------------------------------------------
	const float playerHalfX = playerSize.x * 0.5f;
	const float playerHalfY = playerSize.y * 0.5f;

	const float blockHalfX = blockSize.x * 0.5f;
	const float blockHalfY = blockSize.y * 0.5f;

	// --------------------------------------------------------
	// プレイヤーの境界座標を計算
	// --------------------------------------------------------
	const float playerMinX = playerTransform.translation_.x - playerHalfX;

	const float playerMaxX = playerTransform.translation_.x + playerHalfX;

	const float playerMinY = playerTransform.translation_.y - playerHalfY;

	const float playerMaxY = playerTransform.translation_.y + playerHalfY;

	// --------------------------------------------------------
	// ブロックの境界座標を計算
	// --------------------------------------------------------
	const float blockMinX = blockTransform.translation_.x - blockHalfX;

	const float blockMaxX = blockTransform.translation_.x + blockHalfX;

	const float blockMinY = blockTransform.translation_.y - blockHalfY;

	const float blockMaxY = blockTransform.translation_.y + blockHalfY;

	// --------------------------------------------------------
	// X方向の重なりを確認
	// --------------------------------------------------------
	// プレイヤーとブロックが左右方向に重なっていない場合、
	// 上下からの衝突は発生しない。
	if (playerMaxX <= blockMinX || playerMinX >= blockMaxX) {
		return;
	}

	// --------------------------------------------------------
	// Y方向の重なりを確認
	// --------------------------------------------------------
	// プレイヤーとブロックが上下方向に重なっていない場合、
	// 衝突していないので処理を終了する。
	if (playerMaxY <= blockMinY || playerMinY >= blockMaxY) {
		return;
	}

	// ========================================================
	// Y方向の押し戻し
	// ========================================================

	// --------------------------------------------------------
	// 上方向へ移動してブロックに衝突した場合
	// --------------------------------------------------------
	// プレイヤーをブロックの下側まで戻す。
	if (moveY > 0.0f) {

		playerTransform.translation_.y = blockMinY - playerHalfY - kEpsilon;
	}

	// --------------------------------------------------------
	// 下方向へ移動してブロックに衝突した場合
	// --------------------------------------------------------
	// プレイヤーをブロックの上側まで戻す。
	else if (moveY < 0.0f) {

		playerTransform.translation_.y = blockMaxY + playerHalfY + kEpsilon;
	}

	// 押し戻した位置を行列へ反映
	playerTransform.UpdateMatrix();
}