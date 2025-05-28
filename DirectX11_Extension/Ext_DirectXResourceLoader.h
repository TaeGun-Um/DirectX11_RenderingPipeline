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

};