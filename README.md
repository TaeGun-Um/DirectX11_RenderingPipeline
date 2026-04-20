# DirectX11 Rendering Pipeline

그래픽스 학습용 **Direct3D 11 프레임워크**. Scene / Actor / Component 기반 오브젝트 구조 위에 Forward · Deferred · Shadow · PostProcess · Reflection 등 주요 렌더링 기법을 단계적으로 쌓아 올린 개인 프로젝트입니다.

> 상세 문서는 [`docs/`](docs/) 참조 — [Framework](docs/Framework/) · [DirectX](docs/DirectX/) · [Environment](docs/Environment/) · [WorkLog](docs/WorkLog/)

---

## 기능

**렌더링**
- Forward / **Deferred** 렌더링 경로 (G-Buffer → LightPass → Merge + Forward 알파 패스)
- Directional / Point Light, **Shadow Map**, Normal Mapping
- Phong · Blinn-Phong · **PBR** 라이팅
- **리플렉션 프로브** (정적 CubeMap 캡처)
- 스카이박스, 알파/마스킹
- 포스트 프로세스: Blur · Distortion · OldFilm

**오브젝트 시스템**
- Scene / Actor / Component 구조
- Assimp 기반 Static / Dynamic(Skinned) Mesh 로드
- FSM, Collision, Reflection, DynamicMesh 컴포넌트
- 언리얼 스타일 Character + SpringArm + Flycam

**엔진 인프라**
- 지연 삭제(Dead list) + fast-path 가드 Release 구조
- HLSL ↔ C++ cbuffer 패킹 `static_assert` 검증
- **Shader Hot Reload** (F6) — 런타임 HLSL 재컴파일
- 셰이더 리플렉션 기반 자동 바인딩

---

## 아키텍처

| # | 프로젝트 | 종류 | 역할 |
|---|---|---|---|
| 00 | [`DirectX11_Base/`](DirectX11_Base/) | Static Lib | 공통 유틸 — 수학, 입력, 델타타임, 윈도우, 디렉토리 |
| 01 | [`DirectX11_Extension/`](DirectX11_Extension/) | Static Lib | 렌더링 엔진 — 디바이스, 셰이더, 리소스, 액터/컴포넌트 |
| 02 | [`DirectX11_Contents/`](DirectX11_Contents/) | Static Lib | 씬 / 액터 구현 (RenderScene, MathScene, 데모) |
| 03 | [`DirectX11_RenderingPipeline/`](DirectX11_RenderingPipeline/) | Exe | 진입점 (`main.cpp`) |
| — | [`Shader/`](Shader/) | HLSL | 런타임 `D3DCompile` 로드용 셰이더 |

빌드 의존: **Base → Extension → Contents → Exe**

### 핵심 클래스
| 클래스 | 역할 |
|---|---|
| `Ext_Core` | 엔진 진입점 / 메인 루프 |
| `Ext_DirectXDevice` | `ID3D11Device` / Context / SwapChain / 메인 RT |
| `Ext_Scene` | 씬, 메인 카메라, 라이트 보관 |
| `Ext_Actor` / `Ext_Component` | 오브젝트 단위 / 기능 단위 |
| `Ext_DirectXShader` | 셰이더 컴파일 + 리플렉션 + 자동 바인딩 |
| `Ext_DirectXRenderTarget` | RT/DSV 바인딩, Merge |
| `Ext_ReflectionComponent` | CubeMap 캡처 → 리플렉션 텍스처 생성 |

---

## 요구 사항

- **OS**: Windows 10 이상 (개발/테스트 Windows 11)
- **IDE**: Visual Studio 2022 (Toolset `v143`)
- **언어**: ISO C++20 (`/std:c++20`)
- **DirectX**: Windows SDK 내장 DirectX 11 헤더/라이브러리 (별도 SDK 설치 불필요)
- **외부 라이브러리** (ThirdParty/ 에 lib.zip 동봉)
  - **Assimp** — Mesh / 애니메이션 로드
  - **DirectXTex** — Texture 로드 / 포맷 변환
  - **DirectXTK** — CubeMap 유틸
  - **Dear ImGui** — UI (소스 포함)

---

## 빌드 & 실행

### 1. 초기 세팅 (최초 1회)
```bat
Unzip.bat
```
`ThirdParty/{Assimp,DirectXTex,DirectXTK}/lib.zip` 압축 해제.

### 2. 빌드
- Configuration: **Debug** or **Release** × Platform: **x64**
- 솔루션 빌드 (F7) 또는 디버깅(F5)
- 시작 프로젝트: `03. DirectX11_RenderingPipeline`

**커맨드라인 (MSBuild):**
```
"<VS>/MSBuild/Current/Bin/MSBuild.exe" DirectX11_RenderingPipeline.sln ^
    -p:Configuration=Debug -p:Platform=x64 -m -nologo -verbosity:minimal
```

### 3. 실행
```
x64/{Configuration}/03. DirectX11_RenderingPipeline.exe
```

---

## 기본 조작

| 입력 | 동작 |
|---|---|
| `W` `A` `S` `D` | 이동 |
| 마우스 드래그 | 카메라 회전 |
| `Space` / `Ctrl` | 상승 / 하강 |
| `Shift` | 3배속 스프린트 (Flycam) |
| 마우스 우클릭 (홀드) | Flycam 활성 + 커서 잠금 |
| 마우스 휠 | Flycam 이동 속도 조절 |
| `F1` | Flycam 모드 sticky 토글 |
| `F4` | Flycam 커서 포커스 해제 |
| `F6` | **셰이더 Hot Reload** (전체 VS/PS 재컴파일) |

---

## 디렉토리 구조

```
DirectX11_RenderingPipeline/
├── DirectX11_Base/          # 공통 유틸 (lib)
├── DirectX11_Extension/     # 렌더링 엔진 (lib)
├── DirectX11_Contents/      # 씬 / 액터 (lib)
├── DirectX11_RenderingPipeline/   # 실행 파일 (exe)
├── Shader/                  # HLSL (*.hlsl, *.fx)
├── Resource/                # 메시 / 텍스처 / 리플렉션 캡처
├── ThirdParty/              # Assimp / DirectXTex / DirectXTK
├── docs/                    # 프로젝트 문서
└── DirectX11_RenderingPipeline.sln
```

---

## 문서

| 폴더 | 내용 |
|---|---|
| [`docs/Environment/`](docs/Environment/) | 개발 환경 — 툴, 언어, SDK, 외부 라이브러리 |
| [`docs/DirectX/`](docs/DirectX/) | DirectX11 API / 이론 정리 (01~11) |
| [`docs/Framework/`](docs/Framework/) | 프레임워크 구현 — 기초 구조부터 렌더링 기법까지 (01~16) |
| [`docs/WorkLog/`](docs/WorkLog/) | 작업 기록 — 날짜별 세션 로그 |


---

## 📝 라이선스

개인 학습용 프로젝트. 외부 라이브러리는 각자의 라이선스를 따릅니다.
