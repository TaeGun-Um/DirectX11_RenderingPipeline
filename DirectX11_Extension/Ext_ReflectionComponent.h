#pragma once
#include "Ext_Component.h"

class Ext_ReflectionComponent : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_ReflectionComponent() {}
	~Ext_ReflectionComponent() {}

	// delete Function
	Ext_ReflectionComponent(const Ext_ReflectionComponent& _Other) = delete;
	Ext_ReflectionComponent(Ext_ReflectionComponent&& _Other) noexcept = delete;
	Ext_ReflectionComponent& operator=(const Ext_ReflectionComponent& _Other) = delete;
	Ext_ReflectionComponent& operator=(Ext_ReflectionComponent&& _Other) noexcept = delete;

	void ReflectionInitialize(std::shared_ptr<class Ext_Actor> _Owner, std::string_view _CaptureTextureName, const float4& _Scale = float4(128, 128));
	std::shared_ptr<class Ext_DirectXTexture> GetReflectionCubeTexture() { return CubeTexture; }

	// 리플렉션 컴포넌트 생성·초기화·메쉬 바인딩을 한 번에 수행하는 헬퍼
	// 이름 충돌을 막기 위해 전역 카운터로 유니크 이름을 생성
	static std::shared_ptr<Ext_ReflectionComponent> AttachTo(std::shared_ptr<class Ext_Actor> _Owner, std::shared_ptr<class Ext_MeshComponent> _Mesh, const float4& _CaptureSize = float4(512, 512));

protected:

private:
	std::shared_ptr<class Ext_DirectXTexture> CubeTexture = nullptr;
	static int AttachCounter;

};
