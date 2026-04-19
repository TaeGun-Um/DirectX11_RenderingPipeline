---
title: 셰이더 Hot Reload (F6)
original_url: ""
original_date: 2026-04-20
migrated: 2026-04-20
category: Framework
---

# 셰이더 Hot Reload

셰이더를 수정하고 효과를 보려면 원래는 프로그램을 껐다가 다시 빌드해야 한다. 수치 하나 바꿔보는데 매번 30초씩 기다리면 iteration 속도가 바닥이 난다. Hot Reload 는 **실행 중인 프로그램에서 HLSL 을 재컴파일해 즉시 반영**하는 기능.

## 단축키

**F6** — 등록된 모든 Vertex/Pixel 셰이더를 파일에서 다시 읽어 재컴파일한다.

결과는 Visual Studio **Output 창** 에 출력:
```
[HotReload] ===== 시작 =====
[HotReload] VS reloaded: Static_VS
[HotReload] VS reloaded: Dynamic_VS
...
[HotReload] PS compile failed: PBR_PS
error X3000: syntax error: unexpected token 'foo'
...
[HotReload] ===== 완료 — VS 15/15, PS 13/14 =====
```

실패한 셰이더는 **이전 버전을 그대로 유지**한다 — 화면이 검게 나오지 않음.

## 내부 동작

### 1. 셰이더가 자기 파일 경로를 기억

과거 `Ext_DirectXShader` 는 EntryPoint / Version / BinaryCode 만 들고 있었음. 재컴파일하려면 원본 `.hlsl` 경로가 필요 → `FilePath` 멤버 추가.

```cpp
class Ext_DirectXShader
{
protected:
    std::string FilePath;   // ← 새로 추가
    std::string EntryPoint;
    std::string Version;
    COMPTR<ID3DBlob> BinaryCode;
    // ...
    virtual bool Reload() { return false; } // 파생 타입이 override
};
```

`CreateVertexShader`/`CreatePixelShader` 에서 경로를 저장해 두면 Reload 때 재사용.

### 2. Reload — 두 단계 안전 교체

성공할지 확인되기 전엔 기존 상태를 건드리지 않는다:

```cpp
bool Ext_DirectXVertexShader::Reload()
{
    if (FilePath.empty()) return false;

    // [1] 임시 Blob 에 먼저 컴파일
    COMPTR<ID3DBlob> NewBlob, Error;
    if (FAILED(D3DCompileFromFile(..., NewBlob.GetAddressOf(), Error.GetAddressOf())))
    {
        OutputDebugStringA(error_msg);
        return false;  // ← 기존 BinaryCode / VertexShader 그대로
    }

    // [2] 새 D3D 셰이더 객체 생성 — 실패 시에도 기존 유지
    COMPTR<ID3D11VertexShader> NewVS;
    if (FAILED(device->CreateVertexShader(..., NewVS.GetAddressOf())))
        return false;

    // [3] 성공 → 교체
    BinaryCode   = NewBlob;
    VertexShader = NewVS;

    // [4] 리플렉션 재수집
    GetBufferSetter().Clear();
    ShaderResourceSetting();

    return true;
}
```

중요한 점:
- `COMPTR` 가 이전 객체 Release 를 자동 처리 → 메모리 누수 없음
- 실패 시 새 객체들은 스코프 종료와 함께 Release → 남는 게 없음

### 3. 모든 셰이더 순회

`Ext_DirectXResourceLoader::ReloadAllShaders()` 가 각 셰이더 타입의 이름→shared_ptr 맵을 iterate:

```cpp
for (auto& [Name, VS] : Ext_DirectXVertexShader::GetAllNameResources())
{
    if (VS) VS->Reload();
}
for (auto& [Name, PS] : Ext_DirectXPixelShader::GetAllNameResources())
{
    if (PS) PS->Reload();
}
```

`GetAllNameResources()` 는 `Ext_ResourceManager` 의 private 맵을 const 참조로 노출하는 공개 getter (외부에서 수정 불가).

### 4. F6 키 바인딩

```cpp
// Ext_Core::Start
Base_Input::CreateKey("ShaderReload", VK_F6);

// Ext_Core::Update
if (Base_Input::IsDown("ShaderReload"))
{
    Ext_DirectXResourceLoader::ReloadAllShaders();
}
```

## 왜 이 구조가 안전한가

### Material 이 가진 shared_ptr 은 여전히 유효

Material 은 셰이더를 `shared_ptr<Ext_DirectXVertexShader>` 로 참조. Reload 는 **그 객체의 내부 상태**만 바꿈 (`BinaryCode`, `VertexShader` 멤버). Material 이 들고 있는 포인터 주소는 그대로라 참조는 유효.

```
Material → shared_ptr → VertexShaderObj {
                           BinaryCode = NEW_BLOB,   ← 내용만 바뀜
                           VertexShader = NEW_VS,
                           ...
                       }
```

### 실패 시 롤백

컴파일·오브젝트 생성 둘 다 **새 임시 객체** 에 먼저 해보고, 성공 후에만 멤버를 교체. 중간에 터지면 기존이 그대로 남음.

## 알려진 한계

### 1. cbuffer 레이아웃 변경은 불완전

Material 이 `Unit->BufferSetter` 에 셰이더의 cbuffer 정보를 **한 번 복사**해 놓음. Reload 가 셰이더의 `BufferSetter` 를 재수집해도 **Unit 의 복사본은 변경 안 됨**. 따라서:
- ✅ PS 내부 수치·계산 변경 → 안전
- ✅ 주석 추가·변경 → 안전
- ⚠️ cbuffer 필드 추가/제거/순서 변경 → 재시작 권장

### 2. VS Input Signature 변경

InputLayout 은 VS 바이트코드로 만들어진다. VS 의 입력 시맨틱이 바뀌면 InputLayout 이 스테일해져 드로우콜이 깨질 수 있음. 같은 조건에선 재시작 권장.

### 3. 디버그 모드 권장

컴파일 실패 메시지가 `OutputDebugStringA` 로 나가기 때문에 Visual Studio 의 Output 창에서 확인 가능. Release 빌드에서는 그 창이 안 보이므로 Debug 에서 주로 사용.

## 향후 개선안

- **파일 감시 자동화** — `ReadDirectoryChangesW` 로 `Shader/` 변경을 감지하면 F6 안 눌러도 자동 reload
- **부분 Reload** — 모든 셰이더 대신 변경된 파일의 셰이더만
- **cbuffer 변경 완전 대응** — 모든 Unit 의 BufferSetter 를 찾아 재생성. Unit 을 셰이더 변경에 구독시키는 방식
- **단축키 분리** — PS 만 vs VS·PS 전체 등 옵션

## 얻은 결과

- **Iteration 속도 향상** — 셰이더 수정 → 저장 → 게임 창에서 F6. 1~2초면 반영.
- **실패 시 안전** — 컴파일 에러 찍히고 이전 상태 유지. 게임이 안 죽음.
- **학습 편의** — "이 값 바꾸면 어떻게 될까?" 같은 실험이 매우 편해짐.

## 관련 현재 코드
- [DirectX11_Extension/Ext_DirectXShader.h](../../DirectX11_Extension/Ext_DirectXShader.h) — 기반 클래스 + `FilePath` + 가상 `Reload()`
- [DirectX11_Extension/Ext_DirectXVertexShader.{h,cpp}](../../DirectX11_Extension/Ext_DirectXVertexShader.h) — VS Reload
- [DirectX11_Extension/Ext_DirectXPixelShader.{h,cpp}](../../DirectX11_Extension/Ext_DirectXPixelShader.h) — PS Reload
- [DirectX11_Extension/Ext_DirectXResourceLoader.{h,cpp}](../../DirectX11_Extension/Ext_DirectXResourceLoader.h) — `ReloadAllShaders()`
- [DirectX11_Extension/Ext_DirectXBufferSetter.{h,cpp}](../../DirectX11_Extension/Ext_DirectXBufferSetter.h) — `Clear()`
- [DirectX11_Extension/Ext_Core.cpp](../../DirectX11_Extension/Ext_Core.cpp) — F6 바인딩
- [2026-04-20 WorkLog](../WorkLog/2026-04-20.md)
