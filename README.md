[프로젝트 개요]
- 그래픽스 학습을 위한 Direct3D(11) 프레임워크

[기능]
- DirectX11 버전을 활용한 렌더링 파이프라인 구성
- Object 기능 지원(Scene, Actor, Component)
- 델타타임, 파일 경로 생성, 입력 기능
- assimp 라이브러리를 활용한 Static, Dynamic Mesh 로드

[최소 요구사항]
- OS : Windows 10 이상
- 개발환경 : Visual Studio 2022(v143) 이상
- 언어 표준 : ISO C++20 표준
- DirectX SDK : Wondows SDK에 포함된 DirectX 11 헤더 및 라이브러리 활용
- 외부 라이브러리
  1. assimp : Mesh 로드용
  2. DirectTex : Texture 로드용
  3. imgui : UI 라이브러리

[빌드]
1. Solution 폴더 내 Unzip.bat 실행
  - assimp 라이브러리, DirectTex 라이브러리 파일 설치용
2. Configuration : x64 Debug, Release platform 중 선택
3. 솔루션 전체 빌드(F7), 혹은 디버깅(F5)로 실행
  a. 솔루션 빌드 시 : "..\x64\{Configuration}\03. DirectX11_RenderingPipeline.exe” 파일 실행
  b. 컴파일 시 : [03. DirectX11_RenderingPipeline]을 시작 프로젝트로 선택하여 실행

[기본 단축키]
- W/A/S/D : 이동
- 마우스 드래그 : 카메라 회전 
- F1 : 프리카메라 모드 On/Off
- Space bar : 점프/카메라 Up
- Ctrl : 카메라 Down
- [임시] 마우스 포커스를 윈도우창에 둔 후 Alt+F4 : 프로젝트 종료
