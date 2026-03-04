#pragma once
#include <memory>

class Camera;

class Skydome final {
public:

	Skydome();
	~Skydome();

	void Init(std::weak_ptr<Camera> camera);
	void Update();
	void Draw();

private:
	int _handle;
	std::weak_ptr<Camera> _camera;

};

