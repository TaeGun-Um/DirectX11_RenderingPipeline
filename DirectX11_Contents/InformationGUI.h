#pragma once

#include <DirectX11_Extension/Ext_Imgui.h>

class InformationGUI : public ImguiWindow
{
public:
	// constrcuter destructer
	InformationGUI();
	~InformationGUI();

	// delete Function
	InformationGUI(const InformationGUI& _Other) = delete;
	InformationGUI(InformationGUI&& _Other) noexcept = delete;
	InformationGUI& operator=(const InformationGUI& _Other) = delete;
	InformationGUI& operator=(InformationGUI&& _Other) noexcept = delete;

protected:
	void OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime) override;
	
private:
	void FPSView(float _DeltaTime);
	
};