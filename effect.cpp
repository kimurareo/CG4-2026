#include "effect.h"
#include "math.h"

#include <cmath>

void effect::Initialize(Model2* model, Camera* camera) {

	worldTransform_.Initialize();

	worldTransform_.rotation_ = {0, 3.14f, 0};

	worldTransform_.scale_ = {1, 1, 1};

	model_ = model;
	camera_ = camera;
}

void effect::Update() {

    WorldTransformUpdate(worldTransform_);

}

void effect::Draw() {

   model_->Draw(worldTransform_, *camera_);

}
