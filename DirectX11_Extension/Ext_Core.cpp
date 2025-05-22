#include "PrecompileHeader.h"
#include "Ext_Core.h"
#include "Ext_Scene.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include <DirectX11_Base/Base_Deltatime.h>
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXResourceLoader.h"

#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"
#include "Ext_Camera.h"

std::map<std::string, std::shared_ptr<class Ext_Scene>> Ext_Core::Scenes;
std::shared_ptr<class Ext_Scene> Ext_Core::CurrentScenes = nullptr;
std::shared_ptr<class Ext_Scene> Ext_Core::NextScenes = nullptr;

// 최초 실행 후 윈도우창 생성
void Ext_Core::Run(HINSTANCE _hInstance, std::function<void()> _Start, std::function<void()> _End, const float4& _ScreenSize, bool _IsFullScreen)
{
	Base_Debug::LeakCheck();
	int* TrustLeak = new int;

	// 윈도우 창 생성 후 루프문 시작
	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen);
	// bind로 전달해서 ContentsCore의 Start(), End() 함수가 Ext_Core의 Start(), End() 호출 시 실행될 수 있도록 함
	Base_Windows::WindowLoop(std::bind(Ext_Core::Start, _Start), Ext_Core::Update, std::bind(Ext_Core::End, _End));
}

// 윈도우창 생성 후 프로젝트 세팅
void Ext_Core::Start(std::function<void()> _ContentsCoreStart)
{
	// After Create Window, EngineStart
	Ext_DirectXDevice::Initialize(); // 디바이스, 컨텍스트, 스왑체인, 렌더타겟 생성
	Ext_DirectXResourceLoader::Initialize(); // DirectX에 활용할 리소스 생성
	
	if (nullptr == _ContentsCoreStart)
	{
		MsgAssert("Contents_Core Start is nullptr");
		return;
	}
	_ContentsCoreStart();
}

// 프로젝트 세팅 후 WindowLoop
void Ext_Core::Update()
{
	//////////////////////////////  레벨 변경 시  //////////////////////////////
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
		
		Base_Deltatime::GetGlobalTime().TimeReset(); // 레벨 변경 시 글로벌 타임 초기화
	}

	if (nullptr == CurrentScenes)
	{
		MsgAssert("Scenes이 동작하고 있지 않습니다.");
		return;
	}
	//////////////////////////////  레벨 변경 종료  //////////////////////////////


	//////////////////////////////    업데이트    //////////////////////////////
	if (!TimeCheck()) return;
	CurrentScenes->Update(Base_Deltatime::GetGlobalTime().GetFrameTime()); // Actor 행렬 업데이트
	CurrentScenes->Rendering(); // Rendering 업데이트

	////////////////////////////// 업데이트 종료 //////////////////////////////
}

// 윈도우창 종료 시 호출, 자원 Release 실시
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

// Scene 생성 시 자동 호출, 메인 카메라 생성, 이름 세팅
void Ext_Core::SceneInitialize(std::shared_ptr<Ext_Scene> _Level, std::string_view _Name)
{
	CurrentScenes = _Level;
	_Level->SetName(_Name);
	_Level->SetMainCamera(_Level->CreateActor<Ext_Camera>("MainCamera"));
	_Level->Start();
}

bool Ext_Core::TimeCheck()
{
	Base_Deltatime& Deta = Base_Deltatime::GetGlobalTime();
	bool IsPass = true;

	float Detatime = Deta.TimeCheck();
	float FrameTime = Deta.GetFrameTime();
	Deta.AddFrameTime(Detatime);
	float FrameLimit = Deta.GetFrameLimit(); // 60프레임으로 제한
	if (FrameLimit > FrameTime)
	{
		IsPass = false;
	}
	else
	{
		Deta.SetFrameRate(1.0f / FrameTime);
		Deta.SetFPS(static_cast<int>(1.0f / FrameTime + 0.5f));
		Deta.ResetFrameTime();
	}

	return IsPass;
}

