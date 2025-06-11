#pragma once
#include <DirectX11_Extension/Ext_Scene.h>

class MathScene : public Ext_Scene
{
public:
	// constrcuter destructer
	MathScene() {}
	~MathScene() {}

	// delete Function
	MathScene(const MathScene& _Other) = delete;
	MathScene(MathScene&& _Other) noexcept = delete;
	MathScene& operator=(const MathScene& _Other) = delete;
	MathScene& operator=(MathScene&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	
private:
	
};