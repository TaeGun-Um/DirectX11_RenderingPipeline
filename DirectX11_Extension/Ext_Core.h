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

	// Scene 생성 후 저장
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
			MsgAssert("같은 이름의 Scene을 2개 만들수는 없습니다.");
			return nullptr;
		}

		SceneInitialize(NewLevel, NewName.c_str());
		Scenes.insert(std::make_pair(NewName, NewLevel));

		return std::dynamic_pointer_cast<SceneType>(NewLevel);
	}

	// Scene 변경 실시
	static void ChangeScene(std::string_view _Name)
	{
		std::string Name = _Name.data();
		std::string NextName = Base_String::ToUpper(Name);

		if (Scenes.end() == Scenes.find(NextName))
		{
			MsgAssert("없는 Scene은 찾을 수 없습니다.");
			return;
		}

		NextScenes = Scenes[NextName];
	}

	// Scene 찾기
	template<typename SceneType>
	static std::shared_ptr<SceneType> FindScene(std::string_view _Name)
	{
		std::string Name = _Name.data();
		std::string FindName = Base_String::ToUpper(Name);

		if (Scenes.end() == Scenes.find(FindName))
		{
			MsgAssert("없는 Scene은 찾을 수 없습니다.");
			return;
		}

		return std::dynamic_pointer_cast<SceneType>(Scenes[FindName]);
	}

	static void Run(HINSTANCE _hInstance, std::function<void()> _Run, std::function<void()> _End, const float4& _ScreenSize, bool _IsFullScreen);

protected:
	
private:
	static void SceneInitialize(std::shared_ptr<class Ext_Scene> _Level, std::string_view _Name); // 씬 생성 후 기본 값 입력
	static bool TimeCheck(); // 델타타임 체크

	// constrcuter destructer
	Ext_Core() {};
	~Ext_Core() {};
	
	static void Start(std::function<void()> _ContentsCoreStart);
	static void Update();
	static void End(std::function<void()> _ContentsCoreEnd);

	static std::map<std::string, std::shared_ptr<class Ext_Scene>> Scenes; // 저장된 Scene들
	static std::shared_ptr<class Ext_Scene> CurrentScenes; // 현재 지정된 Scene
	static std::shared_ptr<class Ext_Scene> NextScenes; // 다음에 변경될 Scene
};