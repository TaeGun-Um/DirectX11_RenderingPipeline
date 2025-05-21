#include "PrecompileHeader.h"
#include "Ext_Core.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include "Ext_Scene.h"
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXRenderTarget.h"
#include "Ext_DirectXResourceLoader.h"

#include "Ext_DirectXInputLayout.h"
#include "Ext_DirectXVertexBuffer.h"
#include "Ext_DirectXIndexBuffer.h"

std::map<std::string, std::shared_ptr<class Ext_Scene>> Ext_Core::Scenes;
std::shared_ptr<class Ext_Scene> Ext_Core::CurrentScenes = nullptr;
std::shared_ptr<class Ext_Scene> Ext_Core::NextScenes = nullptr;

void Ext_Core::Run(HINSTANCE _hInstance, std::function<void()> _Start, std::function<void()> _End, const float4& _ScreenSize, bool _IsFullScreen)
{
	Base_Debug::LeakCheck();
	int* TrustLeak = new int;

	// 윈도우 창 생성 후 루프문 시작
	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen);
	// bind로 전달해서 ContentsCore의 Start(), End() 함수가 Ext_Core의 Start(), End() 호출 시 실행될 수 있도록 함
	Base_Windows::WindowLoop(std::bind(Ext_Core::Start, _Start), Ext_Core::Update, std::bind(Ext_Core::End, _End));
}

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

void Ext_Core::Update()
{
	// After EngineStart, EngineLoop
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
		// GameEngineTime::GlobalTime.Reset();
	}

	if (nullptr == CurrentScenes)
	{
		MsgAssert("Scenes이 동작하고 있지 않습니다.");
		return;
	}

	CurrentScenes->Update(1.f); // Actor 행렬 업데이트
	CurrentScenes->Rendering(1.f); // Rendering 업데이트
}

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

void Ext_Core::SetSceneName(std::shared_ptr<Ext_Scene> Level, std::string _Name)
{
	Level->SetName(_Name);
}

void Ext_Core::SceneInitialize(std::shared_ptr<Ext_Scene> _Level)
{
	CurrentScenes = _Level;
	_Level->Start();
}

