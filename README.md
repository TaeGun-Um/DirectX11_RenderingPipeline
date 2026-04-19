[프로젝트 개요]
- 그래픽스 학습을 위한 Direct3D(11) 프레임워크

[기능]
- DirectX11 렌더링 파이프라인 구성
- Scene / Actor / Component 기반 오브젝트 구조
- 델타타임, 파일 경로, 입력 처리
- Assimp 기반 Static / Dynamic Mesh 로드
- Phong / Blinn-Phong / PBR 라이팅
- Directional / Point Light, Shadow Map
- Deferred / Forward 렌더링 경로
- 리플렉션 프로브 (Cube Map)
- 포스트 프로세스 (Blur, Distortion, Old Film)
- 스카이박스, 알파/마스킹

[아키텍처]
- `DirectX11_Base`     : 공통 유틸 (수학, 입력, 델타타임, 윈도우, 디렉토리)
- `DirectX11_Extension`: 렌더링 엔진 (디바이스, 셰이더, 메시, 머티리얼, 액터/컴포넌트)
- `DirectX11_Contents` : 씬·액터 (RenderScene, MathScene, 데모 액터)
- `Shader`             : HLSL 셰이더 (VS/PS/Include)
- 빌드 순서 : Base → Extension → Contents → (03. DirectX11_RenderingPipeline 실행파일)

[핵심 클래스]
- `Ext_Core`              : 엔진 진입점 / 메인 루프
- `Ext_DirectXDevice`     : ID3D11Device / Context / SwapChain / 메인 렌더타겟
- `Ext_Scene`             : 씬, 메인 카메라, 라이트 보관
- `Ext_Actor`             : 오브젝트 단위, Component 소유
- `Ext_Component`         : Mesh / Collision / Reflection / DynamicMesh 등
- `Ext_DirectXShader`     : 셰이더 컴파일 / 리플렉션, 바인딩 자동 세팅
- `Ext_DirectXRenderTarget`: RT/DSV 바인딩, Merge
- `Ext_ReflectionComponent`: CubeMap 캡처 및 리플렉션 텍스처 생성

[최소 요구사항]
- OS : Windows 10 이상
- 개발환경 : Visual Studio 2022(v143) 이상
- 언어 표준 : ISO C++20
- DirectX SDK : Windows SDK 포함 DirectX 11 헤더 / 라이브러리
- 외부 라이브러리
  - Assimp     : Mesh 로드
  - DirectXTex : Texture 로드
  - DirectXTK  : CubeMap 로드 / 생성
  - ImGui      : UI

[빌드]
1. 솔루션 루트에서 `Unzip.bat` 실행
   - Assimp / DirectXTex / DirectXTK lib.zip 을 풀어 `ThirdParty/*/lib/` 배치
2. Configuration : x64 Debug / Release
3. 솔루션 빌드(F7) 또는 디버깅(F5) 실행
   - 실행 파일 : `x64/{Configuration}/03. DirectX11_RenderingPipeline.exe`
   - 시작 프로젝트 : `03. DirectX11_RenderingPipeline`

[기본 단축키]
- W/A/S/D       : 이동
- 마우스 드래그  : 카메라 회전
- F1            : 프리카메라 모드 On/Off
- F4            : 프리카메라 모드에서 마우스 포커싱 탈출
- Space bar     : 점프 / 카메라 Up
- Ctrl          : 카메라 Down

[디렉토리 구조]
- `DirectX11_Base/`        : Base 라이브러리 소스
- `DirectX11_Extension/`   : Extension 엔진 소스
- `DirectX11_Contents/`    : Contents 씬/액터 소스
- `Shader/`                : HLSL 셰이더 및 include (`*.fx`)
- `Resource/`              : 메시, 텍스처, 리플렉션 캡처 등
- `ThirdParty/`            : 외부 라이브러리 (Assimp, DirectXTex, DirectXTK)
- `lib/`, `bin/`           : 빌드 산출물
