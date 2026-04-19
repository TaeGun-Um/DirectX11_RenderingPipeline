# 개발 환경

## 운영체제
- Windows 10 이상 (개발 / 테스트 Windows 11 Pro)

## IDE / 빌드
- Visual Studio 2022 (Platform Toolset `v143`)
- C++ 언어 표준: **ISO C++20** (`/std:c++20`)
- 빌드 구성: `Debug | Release` × `Win32 | x64` (x64 Debug/Release 권장)
- 솔루션: `DirectX11_RenderingPipeline.sln`

### 솔루션 내 프로젝트 (빌드 순서)

| 번호 | 프로젝트 | 종류 | 역할 |
|---|---|---|---|
| 00 | `DirectX11_Base` | Static Lib | 공통 유틸 (수학, 입력, 델타타임, 윈도우, 디렉토리) |
| 01 | `DirectX11_Extension` | Static Lib | 렌더링 엔진 (디바이스, 셰이더, 리소스, 액터/컴포넌트) |
| 02 | `DirectX11_Contents` | Static Lib | 씬/액터 구현 |
| 03 | `DirectX11_RenderingPipeline` | Exe | 진입점 (main.cpp) |
| -  | `Shader` | HLSL 빌드 | 런타임 로드용 HLSL |

의존: `Base → Extension → Contents → exe`

## 외부 라이브러리
- **Assimp** — Mesh 로드 (정적/애니메이션 포함)
- **DirectXTex** — Texture 로드 / 포맷 변환
- **DirectXTK** — CubeMap 로드 / 생성 도우미
- **Dear ImGui** — UI (소스 직접 포함)

`ThirdParty/{Assimp,DirectXTex,DirectXTK}/` 에 헤더 + `lib.zip` 동봉.

## Windows SDK / DirectX
- Windows SDK 내장 DirectX 11 헤더·라이브러리 사용
- 별도 DirectX SDK 설치 불필요
- 주요 링크 대상: `d3d11.lib`, `dxgi.lib`, `d3dcompiler.lib` 등

## 초기 세팅
```
Unzip.bat          # ThirdParty/*/lib.zip 압축 해제 (assimp, DirectXTex, DirectXTK 라이브러리 추출)
```
솔루션 빌드 전 1회 실행 필요.

## 빌드 & 실행
1. Configuration: `Debug` 또는 `Release`, Platform: `x64`
2. 솔루션 빌드 (F7)
3. 실행 파일: `x64/{Configuration}/03. DirectX11_RenderingPipeline.exe`
4. 디버깅(F5) 실행 시 시작 프로젝트: `03. DirectX11_RenderingPipeline`

### MSBuild (커맨드라인)
```
"<VS>/MSBuild/Current/Bin/MSBuild.exe" DirectX11_RenderingPipeline.sln \
    -p:Configuration=Debug -p:Platform=x64 -m -nologo -verbosity:minimal
```

## 파일 인코딩 주의사항
프로젝트 일부 `.cpp`/`.h`/`.hlsl` 파일은 **CP949(ISO-8859) 인코딩**으로 저장되어 있음.
- 신규 파일 생성 시 **UTF-8 with BOM** 사용 권장 (MSVC 기본 코드 페이지 호환)
- CP949 파일을 UTF-8 도구로 무심코 편집하면 한글 주석이 `�`(U+FFFD)로 손상됨 → 바이트 레벨 편집 필요
