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

	void SetFunction1(std::function<void()> _NewFunction)
	{
		Function1 = _NewFunction;
	}

	void SetTransform1(std::shared_ptr<class Ext_Transform> _NewTransform)
	{
		Transform1 = _NewTransform;
	}

	void SetTransform2(std::shared_ptr<class Ext_Transform> _NewTransform)
	{
		Transform2 = _NewTransform;
	}

protected:
	void OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime) override;
	
private:
	void FPSView(float _DeltaTime);
	std::function<void()> Function1;
	std::shared_ptr<class Ext_Transform> Transform1 = nullptr;
	std::shared_ptr<class Ext_Transform> Transform2 = nullptr;
};