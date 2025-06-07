#pragma once

#include "Ext_Object.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

class ImguiWindow : public Ext_Object, public std::enable_shared_from_this<ImguiWindow>
{
	friend class Ext_Imgui;

public:

protected:
	std::string Name = "";
	virtual void Start() {}
	virtual void OnGUI(std::shared_ptr<class Ext_Scene> Level, float _DeltaTime) = 0;

private:
	bool Begin()
	{
		std::string_view View = Name;
		return ImGui::Begin(View.data());
	}

	void End() { ImGui::End(); }
};

class Ext_Imgui
{
public:
	// constrcuter destructer
	Ext_Imgui() {}
	~Ext_Imgui() {}

	// delete Function
	Ext_Imgui(const Ext_Imgui& _Other) = delete;
	Ext_Imgui(Ext_Imgui&& _Other) noexcept = delete;
	Ext_Imgui& operator=(const Ext_Imgui& _Other) = delete;
	Ext_Imgui& operator=(Ext_Imgui&& _Other) noexcept = delete;

	static void Initialize();
	static void Render(std::shared_ptr<class Ext_Scene> Level, float _DeltaTime);
	static void Release();

	template<typename WindowType>
	static std::shared_ptr<ImguiWindow> CreateImgui(std::string_view _Name)
	{
		std::string NewName = Base_String::ToUpper(_Name);

		if (AllImgui.end() != AllImgui.find(NewName))
		{
			MsgAssert("동일한 이름의 gui가 이미 존재함" + std::string(_Name));
			return nullptr;
		}

		std::shared_ptr<ImguiWindow> NewWindow = std::make_shared<WindowType>();
		NewWindow->Name = NewName;
		NewWindow->Start();
		AllImgui[NewName] = NewWindow;

		return NewWindow;
	}

	template<typename ConvertType>
	static std::shared_ptr<ConvertType> FindGUIWindowConvert(const std::string_view& _Name)
	{
		std::shared_ptr<ImguiWindow> Window = FindGUIWindow(_Name);

		return std::dynamic_pointer_cast<ConvertType>(Window);
	}

	static std::shared_ptr<ImguiWindow> FindGUIWindow(const std::string_view& _Name)
	{
		std::string UpperString = Base_String::ToUpper(_Name);

		std::map<std::string, std::shared_ptr<ImguiWindow>>::iterator FindIter = AllImgui.find(UpperString);

		if (AllImgui.end() == FindIter)
		{
			return nullptr;
		}

		return FindIter->second;
	}

	void End()
	{
		ImGui::End();
	}

protected:
	
private:
	static std::map<std::string, std::shared_ptr<ImguiWindow>> AllImgui;
	static bool bIsImguiInitialize;
	
	
};