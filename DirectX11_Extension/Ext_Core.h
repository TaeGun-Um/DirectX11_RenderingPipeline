#pragma once

// Process의 Loop, shutdown process 담당
class Ext_Core
{
public:
	// delete Function
	Ext_Core(const Ext_Core& _Other) = delete;
	Ext_Core(Ext_Core&& _Other) noexcept = delete;
	Ext_Core& operator=(const Ext_Core& _Other) = delete;
	Ext_Core& operator=(Ext_Core&& _Other) noexcept = delete;

	template<typename SceneType>
	static std::shared_ptr<SceneType> CreateScene(std::string_view _Name)
	{
		std::shared_ptr<Ext_Scene> NewLevel = std::make_shared<SceneType>();
		std::string NewName = _Name.data();

		if (_Name == "")
		{
			const type_info& Info = typeid(SceneType);
			NewName = Info.name();
			NewName.replace(0, 6, "");
		}

		NewName = Base_String::ToUpper(NewName);

		if (Scenes.end() != Scenes.find(NewName))
		{
			MsgAssert("같은 이름의 레벨을 2개 만들수는 없습니다.");
		}

		SetSceneName(NewLevel, NewName.c_str());
		SceneInitialize(NewLevel);
		Scenes.insert(std::make_pair(NewName, NewLevel));

		return std::dynamic_pointer_cast<SceneType>(NewLevel);
	}

	static void Run(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen);
	static void RenderTest();

protected:
	
private:
	static void SetSceneName(std::shared_ptr<class Ext_Scene> Level, std::string _Name);
	static void SceneInitialize(std::shared_ptr<class Ext_Scene> _Level);

	// constrcuter destructer
	Ext_Core() {};
	~Ext_Core() {};
	
	static void Start();
	static void Update();
	static void End();

	static std::map<std::string, std::shared_ptr<class Ext_Scene>> Scenes;
	static std::shared_ptr<class Ext_Scene> CurrentScenes;
};