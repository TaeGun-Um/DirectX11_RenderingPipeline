#include "PrecompileHeader.h"
#include "Ext_Core.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include <DirectX11_Base/Base_Deltatime.h>

#include "Ext_Scene.h"
#include "Ext_Camera.h"

#include "Ext_DirectXDevice.h"
#include "Ext_DirectXResourceLoader.h"

std::map<std::string, std::shared_ptr<class Ext_Scene>> Ext_Core::Scenes;
std::shared_ptr<class Ext_Scene> Ext_Core::CurrentScenes = nullptr;
std::shared_ptr<class Ext_Scene> Ext_Core::NextScenes = nullptr;

// ���� ���� �� ������â ����
void Ext_Core::Run(HINSTANCE _hInstance, std::function<void()> _Start, std::function<void()> _End, const float4& _ScreenSize, bool _IsFullScreen)
{
	Base_Debug::LeakCheck();
	int* TrustLeak = new int;

	// ������ â ���� �� ������ ����
	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen);
	// bind�� �����ؼ� ContentsCore�� Start(), End() �Լ��� Ext_Core�� Start(), End() ȣ�� �� ����� �� �ֵ��� ��
	Base_Windows::WindowLoop(std::bind(Ext_Core::Start, _Start), Ext_Core::Update, std::bind(Ext_Core::End, _End));
}

// Scene ���� �� �ڵ� ȣ��, ���� ī�޶� ����, �̸� ����
void Ext_Core::SceneInitialize(std::shared_ptr<Ext_Scene> _Level, std::string_view _Name)
{
	CurrentScenes = _Level;
	_Level->SetName(_Name);
	_Level->SetMainCamera(_Level->CreateActor<Ext_Camera>("MainCamera"));
	_Level->Start();
}

// ��ŸŸ�� üũ, 60������ �������� ������
bool Ext_Core::TimeCheck()
{
	Base_Deltatime& Deta = Base_Deltatime::GetGlobalTime();
	bool IsPass = true;

	float DeltaTime = Deta.TimeCheck(); // 1������ ���� ��� �ð�
	Deta.AddFrameTime(DeltaTime);       // ����

	float FrameTime = Deta.GetFrameTime();  // ���� �ð� Ȯ��
	float FrameLimit = Deta.GetFrameLimit(); // 1 / 60 = 0.016666..

	if (FrameTime < FrameLimit)
	{
		IsPass = false;
	}
	else
	{
		Deta.SetDeltaTime(FrameTime);
		Deta.SetFrameRate(1.0f / FrameTime);
		Deta.SetFPS(static_cast<int>(1.0f / FrameTime + 0.5f));
		Deta.ResetFrameTime(); // ���� �ð� �ʱ�ȭ
	}

	return IsPass;
}

// ������â ���� �� ������Ʈ ����
void Ext_Core::Start(std::function<void()> _ContentsCoreStart)
{
	Ext_DirectXDevice::Initialize(); // ����̽�, ���ؽ�Ʈ, ����ü��, ����Ÿ�� ����
	Ext_DirectXResourceLoader::Initialize(); // DirectX�� Ȱ���� ���ҽ� ����

	if (nullptr == _ContentsCoreStart)
	{
		MsgAssert("Contents_Core Start is nullptr");
		return;
	}
	_ContentsCoreStart();
}

// ������Ʈ ���� �� WindowLoop
void Ext_Core::Update()
{
	//////////////////////////////  ���� ���� ��  //////////////////////////////
	if (nullptr != NextScenes)
	{
		std::shared_ptr<Ext_Scene> PrevScene = CurrentScenes;

		if (nullptr != CurrentScenes)
		{
			CurrentScenes->SceneChangeEnd();
			// MainLevel->PostProcessLevelChangeEnd();
			// MainLevel->RenderTargetTextureRelease();
		}

		CurrentScenes = NextScenes;
		NextScenes = nullptr;

		if (nullptr != CurrentScenes)
		{
			// MainLevel->RenderTargetTextureCreate();
			// MainLevel->PostProcessLevelChangeStart();
			CurrentScenes->SceneChangeInitialize();
		}
		
		Base_Deltatime::GetGlobalTime().TimeReset(); // ���� ���� �� �۷ι� Ÿ�� �ʱ�ȭ
	}

	if (nullptr == CurrentScenes)
	{
		MsgAssert("Scenes�� �����ϰ� ���� �ʽ��ϴ�.");
		return;
	}
	//////////////////////////////  ���� ���� ����  //////////////////////////////


	//////////////////////////////    ������Ʈ    //////////////////////////////
	if (!TimeCheck()) return;
	CurrentScenes->Update(Base_Deltatime::GetGlobalTime().GetDeltaTime()); // Actor ��� ������Ʈ

	Ext_DirectXDevice::RenderStart(); // ����� Ŭ���� �� ����
	CurrentScenes->Rendering(Base_Deltatime::GetGlobalTime().GetDeltaTime()); // Rendering ������Ʈ
	Ext_DirectXDevice::RenderEnd(); // Present ȣ��
	////////////////////////////// ������Ʈ ���� //////////////////////////////
}

// ������â ���� �� ȣ��, �ڿ� Release �ǽ�
void Ext_Core::End(std::function<void()> _ContentsCoreEnd)
{
	// After Window Destroy, Process ending

	if (nullptr == _ContentsCoreEnd)
	{
		MsgAssert("Contents_Core End is nullptr");
		return;
	}
	_ContentsCoreEnd();
}
