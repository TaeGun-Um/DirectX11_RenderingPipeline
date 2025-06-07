#pragma once
#include "Ext_Imgui.h"
#include "Ext_DirectXRenderTarget.h"

class ImageShotWindow : public ImguiWindow
{
public:
	ImTextureID RenderTexture;
	ImVec2 Size;

public:
	void RenderTextureSetting(ImTextureID RenderTexture, ImVec2 Size);

public:
	void OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime) override;
};

struct NameRenderTarget
{
public:
	std::string Name;
	std::shared_ptr<Ext_DirectXRenderTarget> RenderTarget;
};

class RendertargetGUI : public ImguiWindow
{
public:
	// constrcuter destructer
	RendertargetGUI() {}
	~RendertargetGUI() {}

	// delete Function
	RendertargetGUI(const RendertargetGUI& _Other) = delete;
	RendertargetGUI(RendertargetGUI&& _Other) noexcept = delete;
	RendertargetGUI& operator=(const RendertargetGUI& _Other) = delete;
	RendertargetGUI& operator=(RendertargetGUI&& _Other) noexcept = delete;

	void OnGUI(std::shared_ptr<class Ext_Scene> _Scene, float _DeltaTime) override;
	static void AddDebugRenderTarget(int _Order, std::string_view _Name, std::shared_ptr<Ext_DirectXRenderTarget> _Target);
	static void Clear();

protected:
	
private:
	static std::map<int, NameRenderTarget> DebugRenderTarget;

};