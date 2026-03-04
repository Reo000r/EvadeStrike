#pragma once
#include "Library/Geometry/Vector3.h"
#include <memory>

class GameObject;

class ShadowGenerator
{
public:
	ShadowGenerator();
	~ShadowGenerator();

	void Init(std::weak_ptr<GameObject> baseObject);
	void Update();
	void ShadowDrawStart();
	void ShadowDrawEnd();
	void NormalDrawStart();
	void NormalDrawEnd();

private:
	int _shadowMapHandle;
	std::weak_ptr<GameObject> _baseObject;
	;

};

