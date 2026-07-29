#pragma once

#include "KamataEngine.h"
#include "Model2.h"

using namespace KamataEngine;

class effect {
public:

	void Initialize(Model2* model, Camera* camera);

	void Update();

	void Draw();

private:

	WorldTransform worldTransform_;

	Model2* model_ = nullptr;
	
	Camera* camera_;

	effect* effect_ = nullptr;

};
