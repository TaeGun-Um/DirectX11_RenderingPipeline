#include "PrecompileHeader.h"
#include "Base_Input.h"
#include "Base_String.h"
#include "Base_Debug.h"

std::map<std::string, Base_Input::Key> Base_Input::Keys;

void Base_Input::Key::Reset()
{
	bIsDown = false;
	bIsPress = false;
}

bool Base_Input::Key::KeyCheck()
{
	return 0 != GetAsyncKeyState(Input);
}

void Base_Input::Key::Update(float _DeltaTime)
{
	if (true == KeyCheck())
	{
		PressTime += _DeltaTime;
		if (bIsFree)
		{
			bIsDown = true;
			bIsPress = true;
			bIsFree = false;
		}
		else if (bIsDown)
		{
			bIsDown = false;
			bIsPress = true;
			bIsFree = false;
		}
	}
	else
	{
		PressTime = 0.f;
		bIsFree = true;
		bIsDown = false;
		bIsPress = false;
	}
}

bool Base_Input::IsDown(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);

	if (Keys.end() == Keys.find(UpperName))
	{
		MsgAssert("존재하지 않는 키를 사용하려고 했습니다." + UpperName);
	}

	return Keys[UpperName].bIsDown;
}

bool Base_Input::IsPress(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);

	if (Keys.end() == Keys.find(UpperName))
	{
		MsgAssert("존재하지 않는 키를 사용하려고 했습니다." + UpperName);
	}

	return Keys[UpperName].bIsPress;
}

void Base_Input::CreateKey(std::string_view _Name, int _Key)
{
	std::string UpperName = Base_String::ToUpper(_Name);

	if (Keys.end() != Keys.find(UpperName))
	{
		MsgAssert("이미 존재하는 이름의 키를 또 만들려고 했습니다.");
	}

	Keys[UpperName].Input = _Key;
}

void Base_Input::Update(float _DeltaTime)
{
	std::map<std::string, Key>::iterator StartKeyIter = Keys.begin();
	std::map<std::string, Key>::iterator EndKeyIter = Keys.end();

	for (auto& Iter : Keys)
	{
		Iter.second.Update(_DeltaTime);
	}
}


