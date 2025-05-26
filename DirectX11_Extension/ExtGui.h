#pragma once

#include "Ext_Imgui.h"

class ExtGui : public ImguiWindow
{
public:
	// constrcuter destructer
	ExtGui();
	~ExtGui();

	// delete Function
	ExtGui(const ExtGui& _Other) = delete;
	ExtGui(ExtGui&& _Other) noexcept = delete;
	ExtGui& operator=(const ExtGui& _Other) = delete;
	ExtGui& operator=(ExtGui&& _Other) noexcept = delete;

protected:
	void OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime) override;
	
private:
	float FPSTime = 0.f;
	void FPSView(float _DeltaTime);
	
};