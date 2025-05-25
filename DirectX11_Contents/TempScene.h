#pragma once
#include <DirectX11_Extension/Ext_Scene.h>

class TempScene : public Ext_Scene
{
public:
	// constrcuter destructer
	TempScene();
	~TempScene();

	// delete Function
	TempScene(const TempScene& _Other) = delete;
	TempScene(TempScene&& _Other) noexcept = delete;
	TempScene& operator=(const TempScene& _Other) = delete;
	TempScene& operator=(TempScene&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	void Destroy() override;
	
private:
	
};