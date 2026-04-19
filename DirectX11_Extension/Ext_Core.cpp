#include "PrecompileHeader.h"
#include "Ext_Core.h"

#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>
#include <DirectX11_Base/Base_Deltatime.h>
#include <DirectX11_Base/Base_Input.h>

#include "Ext_Scene.h"
#include "Ext_Camera.h"
#include "Ext_Light.h"
#include "Ext_DirectXDevice.h"
#include "Ext_DirectXResourceLoader.h"
#include "Ext_Imgui.h"
#include "ExtGui.h"

std::map<std::string, std::shared_ptr<class Ext_Scene>> Ext_Core::Scenes;
std::shared_ptr<class Ext_Scene> Ext_Core::CurrentScenes = nullptr;
std::shared_ptr<class Ext_Scene> Ext_Core::NextScenes = nullptr;

// =====================================================================
//  엔진 수명 주기 (Lifecycle)
// =====================================================================
//  Run()
//    ├─ Base_Debug::LeakCheck          // CRT 릭 체크 활성
//    ├─ Base_Windows::WindowCreate     // HWND 생성
//    └─ Base_Windows::WindowLoop       // 메시지 루프 진입
//          │
//          ├─ Start()  (루프 시작 시 1회)
//          │     ├─ Ext_DirectXDevice::Initialize        ← 1. Device/Context/SwapChain/MainRT
//          │     ├─ Ext_DirectXResourceLoader::Initialize ← 2. Mesh/Shader/Material/State 에셋
//          │     ├─ Ext_Imgui::Initialize                ← 3. UI 시스템
//          │     ├─ Input 키 등록
//          │     └─ Contents_Core::Start                 ← 4. 사용자 씬 생성
//          │
//          ├─ Update() (매 프레임)
//          │     ├─ Scene 전환 체크 (NextScenes)
//          │     ├─ 60fps Deltatime 게이트 (TimeCheck)
//          │     ├─ Input::Update
//          │     ├─ CurrentScenes->ActorsUpdate          ← Actor/Component Update
//          │     ├─ RenderStart → Scene->Rendering → RenderEnd(Present)
//          │     └─ CurrentScenes->Release               ← PendingDeadActors 정리 (fast path 가드 있음)
//          │
//          └─ End()    (창 종료 시 1회)
//                ├─ Contents_Core::End
//                └─ Ext_Imgui::Release
//                (Device/ResourceLoader 는 static dtor 로 프로세스 종료 시 자동 해제)
//
//  초기화 의존 순서 (반드시 Start 내부에서 유지) :
//    Device  (가장 먼저, 모든 D3D 리소스의 전제)
//     └── ResourceLoader (Mesh/Shader/Material 생성에 Device 필요)
//           └── Imgui (D3D Device/Context hook 필요)
//                 └── Contents::Start (위 에셋 활용)
//
//  순서를 섞으면: ResourceLoader가 null Device 로 CreateBuffer 호출 → crash
//                Imgui 가 ResourceLoader 이전에 오면 UI 전용 셰이더 로드 실패 가능
// =====================================================================

// 최초 실행 후 윈도우창 생성
void Ext_Core::Run(HINSTANCE _hInstance, std::function<void()> _Start, std::function<void()> _End, const float4& _ScreenSize, bool _IsFullScreen)
{
	//Base_Windows::CreateConsole(); // 콘솔창 띄우고싶으면 주석 해제
	Base_Debug::LeakCheck();

	Base_Windows::WindowCreate(_hInstance, _ScreenSize, _IsFullScreen); // 윈도우 창 생성 후 루프문 시작
	Base_Windows::WindowLoop(std::bind(Ext_Core::Start, _Start), Ext_Core::Update, std::bind(Ext_Core::End, _End)); // bind로 전달해서 ContentsCore의 Start(), End() 함수가 Ext_Core의 Start(), End() 호출 시 실행될 수 있도록 함
}

// 윈도우창 생성 후 프로젝트 세팅 — 반드시 아래 순서 준수 (위 주석 참조)
void Ext_Core::Start(std::function<void()> _ContentsCoreStart)
{
	// [1] Device/Context/SwapChain/MainRT — 모든 GPU 리소스의 전제
	Ext_DirectXDevice::Initialize();

	// [2] Mesh/Shader/Material/State 에셋 — Device 위에서 생성
	Ext_DirectXResourceLoader::Initialize();

	// [3] UI 시스템 — Device/Context 에 훅
	Ext_Imgui::Initialize();
	// Ext_Imgui::CreateImgui<ExtGui>("ExtGui"); // 테스트용 GUI 생성

	Base_Input::CreateKey("OnOff", VK_F1);
	Base_Input::CreateKey("Escape", VK_F4);
	Base_Input::CreateKey("Forword", 'W');
	Base_Input::CreateKey("Back", 'S');
	Base_Input::CreateKey("Right", 'D');
	Base_Input::CreateKey("Left", 'A');
	Base_Input::CreateKey("Up", VK_SPACE);
	Base_Input::CreateKey("DOWN", VK_CONTROL);
	Base_Input::CreateKey("FlyCam", VK_RBUTTON);  // 언리얼 에디터 스타일: 우클릭 홀드로 flycam 활성
	Base_Input::CreateKey("Sprint", VK_SHIFT);    // flycam 중 Shift로 3배속
	Base_Input::CreateKey("ShaderReload", VK_F6); // F6: 모든 HLSL 셰이더 Hot Reload

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

	float DeltaTime = Base_Deltatime::GetGlobalTime().GetDeltaTime();

	Base_Input::Update(DeltaTime);

	// F6: 셰이더 Hot Reload — 런타임에 HLSL 재컴파일 후 셰이더 객체 교체
	// 실패 시 기존 셰이더 유지. 출력창(OutputDebugString) 으로 결과 확인 가능.
	if (Base_Input::IsDown("ShaderReload"))
	{
		Ext_DirectXResourceLoader::ReloadAllShaders();
	}

	CurrentScenes->ActorsUpdate(DeltaTime); // Actor 행렬 업데이트

	Ext_DirectXDevice::RenderStart(); // 백버퍼 클리어 및 세팅
	CurrentScenes->Rendering(DeltaTime); // Rendering 업데이트
	Ext_DirectXDevice::RenderEnd(); // Present 호출
	////////////////////////////// 업데이트 종료 //////////////////////////////

	CurrentScenes->Release();
}

// 윈도우창 종료 시 호출 — Start 의 역순 정리
// - Device / ResourceLoader 의 에셋들은 static dtor 에 의해 프로세스 종료 시 자동 해제
// - Imgui 만 내부에 Windows hook 이 있어 명시적 Release 가 필요
void Ext_Core::End(std::function<void()> _ContentsCoreEnd)
{
	if (nullptr == _ContentsCoreEnd)
	{
		MsgAssert("Contents_Core End is nullptr");
		return;
	}
	_ContentsCoreEnd(); // [1] 사용자 정리 (씬 명시적 비우기 등)

	Ext_Imgui::Release(); // [2] UI 시스템 해제
}

// Scene 생성 시 자동 호출, 메인 카메라 생성, 이름 세팅
void Ext_Core::SceneInitialize(std::shared_ptr<Ext_Scene> _Level, std::string_view _Name)
{
	CurrentScenes = _Level;
	_Level->SetName(_Name);
	_Level->SetMainCamera(_Level->CreateActor<Ext_Camera>("MainCamera"));
	_Level->Start();
}

// 델타타임 체크, 60프레임 기준으로 설정함
bool Ext_Core::TimeCheck()
{
	Base_Deltatime& Deta = Base_Deltatime::GetGlobalTime();
	bool IsPass = true;

	float DeltaTime = Deta.TimeCheck(); // 1프레임 기준 경과 시간
	Deta.AddFrameTime(DeltaTime);       // 누적

	float FrameTime = Deta.GetFrameTime();  // 누적 시간 확인
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
		Deta.ResetFrameTime(); // 누적 시간 초기화
	}

	return IsPass;
}