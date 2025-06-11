#pragma once
#include <DirectX11_Extension/Ext_Scene.h>

class RenderScene : public Ext_Scene
{
public:
	// constrcuter destructer
	RenderScene();
	~RenderScene();

	// delete Function
	RenderScene(const RenderScene& _Other) = delete;
	RenderScene(RenderScene&& _Other) noexcept = delete;
	RenderScene& operator=(const RenderScene& _Other) = delete;
	RenderScene& operator=(RenderScene&& _Other) noexcept = delete;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;
	
private:
	
};