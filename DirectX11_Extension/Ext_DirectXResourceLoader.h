#pragma once

// DirectX에 필요한 Resource들을 로드하는 클래스
class Ext_DirectXResourceLoader
{
	friend class Ext_Core;

public:
	// delete Function
	Ext_DirectXResourceLoader(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader(Ext_DirectXResourceLoader&& _Other) noexcept = delete;
	Ext_DirectXResourceLoader& operator=(const Ext_DirectXResourceLoader& _Other) = delete;
	Ext_DirectXResourceLoader& operator=(Ext_DirectXResourceLoader&& _Other) noexcept = delete;

	// Hot Reload — 현재 로드된 모든 VS/PS 를 파일에서 다시 읽어 재컴파일
	// - 실패한 셰이더는 이전 바이트코드 유지 → 한두 개가 틀려도 런타임이 살아있음
	// - Material 의 내부 cbuffer 복사본은 갱신되지 않으므로, cbuffer 레이아웃까지 바꾼 경우엔 재시작 필요
	// - 사용처: Ext_Core::Update 에서 "ShaderReload" 키(F6) 입력 시 호출
	static void ReloadAllShaders();

protected:
	
private:
	static void Initialize();					// 최초 1회 실행하여 리소스 로딩
	static void LoadTexture();			// 이미지 로드
	static void MakeVertex();			// 인풋레이아웃, 버텍스 버퍼, 인덱스 버퍼, 메시 생성
	static void MakeSampler();			// 샘플러 생성
	static void MakeBlend();				// 블렌드 생성
	static void MakeDepth();				// 뎁스 생성
	static void ShaderCompile();		// 셰이더 오토 컴파일 실시
	static void MakeRasterizer();		// 레스터라이저 세팅
	static void MakeMaterial();			// 머티리얼 생성

	///////////////////////////////////////////////////////////////
	static void CreateTriangle();
	static void CreateRect();
	static void CreateCube();
	static void CreateSphere();
	static void CreateCylinder();
};