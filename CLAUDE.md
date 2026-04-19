# Claude 작업 가이드

이 문서는 향후 Claude 세션이 이 레포에서 작업할 때 참고할 컨텍스트입니다.

## 프로젝트 요약
- DirectX11 기반 그래픽스 학습 프레임워크 (엄태건 개인 프로젝트)
- C++20, VS2022(v143), Windows 전용
- `DirectX11_RenderingPipeline.sln` 하나에 4개 프로젝트
  - `00. DirectX11_Base`       (Static Lib) : 공통 유틸
  - `01. DirectX11_Extension`  (Static Lib) : 엔진
  - `02. DirectX11_Contents`   (Static Lib) : 씬·액터
  - `03. DirectX11_RenderingPipeline` (Exe) : 진입점
  - `Shader` (HLSL 빌드 프로젝트)
- 빌드 의존 순서 : Base → Extension → Contents → 실행파일

## 빌드 / 실행

### 초기 세팅 (한 번만)
```
Unzip.bat          # ThirdParty/{Assimp,DirectXTex,DirectXTK}/lib.zip 압축 해제
```

### 빌드 (커맨드라인)
```
"<VS>/MSBuild/Current/Bin/MSBuild.exe" DirectX11_RenderingPipeline.sln \
    -p:Configuration=Debug -p:Platform=x64 -m -nologo -verbosity:minimal
```
VS 경로 예: `/c/Program Files/Microsoft Visual Studio/18/Community/...`

### 실행
`x64/Debug/03. DirectX11_RenderingPipeline.exe`

## 알려진 빌드 이슈 (수정 금지 / 환경 이슈)
- **FXC X3501** : `Shader.vcxproj`가 엔트리포인트 `main`을 찾지 못하는 프로젝트 설정 이슈.
  런타임 `D3DCompile`로 엔트리포인트를 커스텀 지정하므로 동작에 영향 없음.
- **assimp-vc143-mtd.dll 누락** : `Unzip.bat`을 실행하지 않았을 때 발생하는 post-build xcopy 에러.

## 코딩 스타일
- 헤더 가드는 `#pragma once`
- 멤버 함수 파라미터 prefix `_` (`_DeltaTime`, `_Value`)
- 한국어 주석은 CP949 또는 UTF-8 BOM (MSVC 기본 코드페이지 949)
  - **새 파일 생성 시 UTF-8 BOM 필수** (BOM 없으면 파싱 에러로 이어질 수 있음)
- Microsoft COM 포인터는 `COMPTR<T>` 래퍼 (in `DirectX11_Extension/Ext_BaseHeader.h`)
- 상속 시 override 사용, `__super::` 또는 `BaseClass::` 로 베이스 호출

## 자주 접근하는 위치
- 엔진 진입점 : [DirectX11_Extension/Ext_Core.cpp](DirectX11_Extension/Ext_Core.cpp) `Ext_Core::Run`
- 메인 셰이더 자동 바인딩 : [DirectX11_Extension/Ext_DirectXShader.cpp](DirectX11_Extension/Ext_DirectXShader.cpp) `ShaderResourceSetting`
- 데모 씬 : [DirectX11_Contents/RenderScene.cpp](DirectX11_Contents/RenderScene.cpp) `RenderScene::Start`
- 회전 데모 액터 공통 베이스 : [DirectX11_Contents/RotatingMeshActor.h](DirectX11_Contents/RotatingMeshActor.h)
- 라이트 상수버퍼 : [Shader/LightData.fx](Shader/LightData.fx)

## 작업 정책
- **master 브랜치 히스토리 변경 금지** : cherry-pick / reset / rebase / stash 등은 사용자가 명시적으로 요청했을 때만.
  local 변경은 unstaged 상태로 두어 사용자가 검토 · 커밋하게 한다.
- **remote push는 절대 자동으로 하지 않는다.**
- worktree 사용 지양. 메인 레포 디렉토리에서 작업.

## 권장 개선 과제 (미적용)
- Culling / Instancing 도입 (아키텍처 변경 규모 큼, 사용자 협의 필요)
- Singleton 초기화/정리 순서 명시화
- HLSL ↔ C++ 구조체 packing `static_assert`로 보강
- `ID3D11ShaderReflection` 기반 슬롯 매핑을 static_assert 또는 reflection-check 기반으로
