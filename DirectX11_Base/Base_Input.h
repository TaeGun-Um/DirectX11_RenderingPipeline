#pragma once

class Base_Input
{
	struct Key
	{
		bool bIsFree = false;
		bool bIsDown = false;
		bool bIsPress = false;

		float PressTime = 0.f;
		int Input = -1;

		void Reset();
		bool KeyCheck();
		void Update(float _DeltaTime);
	};

public:
	// constrcuter destructer
	Base_Input() {}
	~Base_Input() {}

	// delete Function
	Base_Input(const Base_Input& _Other) = delete;
	Base_Input(Base_Input&& _Other) noexcept = delete;
	Base_Input& operator=(const Base_Input& _Other) = delete;
	Base_Input& operator=(Base_Input&& _Other) noexcept = delete;

	static void CreateKey(std::string_view _Name, int _Key);
	static bool IsDown(std::string_view _Name);
	static void Update(float _DeltaTime);
	static bool IsPress(std::string_view _Name);

protected:
	
private:
	static std::map<std::string, Key> Keys;
	
};