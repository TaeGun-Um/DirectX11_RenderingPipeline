#pragma once
#include "Ext_DirectXBufferSetter.h"

// 렌더링 컴포넌트에 필요한 정보(메시, Pipeline, 상수버퍼 정보 등)들을 가지고 있는 클래스
class Ext_MeshComponentUnit : public std::enable_shared_from_this<Ext_MeshComponentUnit>
{
	friend class Ext_Camera;
	friend class Ext_MeshComponent;
	friend class Ext_DynamicMeshComponent;
	friend class Ext_DirectXRenderTarget;

public:
	// constrcuter destructer
	Ext_MeshComponentUnit();
	~Ext_MeshComponentUnit() {}

	// delete Function
	Ext_MeshComponentUnit(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit(Ext_MeshComponentUnit&& _Other) noexcept = delete;
	Ext_MeshComponentUnit& operator=(const Ext_MeshComponentUnit& _Other) = delete;
	Ext_MeshComponentUnit& operator=(Ext_MeshComponentUnit&& _Other) noexcept = delete;

	// this 가져오기
	template<typename Type>
	std::shared_ptr<Type> GetSharedFromThis()
	{
		return std::dynamic_pointer_cast<Type>(shared_from_this());
	}

	void MeshComponentUnitInitialize(std::string_view _MeshName, MaterialType _SettingValue); // 메시 컴포넌트 유닛 생성 시 호출
	void SetTexture(std::string_view _TextureName, TextureType _SlottVlaue); // 텍스쳐 지정하기
	void SetSampler(SamplerType _SlottVlaue); // 샘플러 변경하기
	void SetSampler(std::string_view _Name, std::string_view _SlotName); // 샘플러 직접 변경

	// Getter, Setter
	std::shared_ptr<class Ext_DirectXMesh> GetMesh() { return Mesh; }
	std::shared_ptr<class Ext_DirectXMaterial> GetMaterial() { return Material; }
	std::weak_ptr<class Ext_MeshComponent> GetOwnerMeshComponent() { return OwnerMeshComponent; }
	Ext_DirectXBufferSetter& GetBufferSetter() { return BufferSetter; }

	ShadowType GetShadowType() { return ShadowT; }

	// 그림자용
	void StaticShadowOn(); // 그림자 세팅하기, 없으면 만들어줌, 스태틱은 이거
	void DynamicShadowOn(); // 그림자 세팅하기, 없으면 만들어줌, 다이나믹은 이거
	void ShadowOff(); // 그림자 끄기
	bool GetIsShadow() { return bIsShadow; }

protected:
	
private:
	void SetOwnerMeshComponent(std::weak_ptr<class Ext_MeshComponent> _OwnerMeshComponent) { OwnerMeshComponent = _OwnerMeshComponent; }
	void Rendering(float _Deltatime); // RenderUnitSetting, RenderUnitDraw 호출
	void RenderUnitSetting(); // Mesh, Material Setting
	void RenderUnitDraw(); // 정점 정보들과 셰이더를 통해 메시 Draw 실시(DrawIndexed Call)
	void RenderUnitShadowSetting(); // 셰도우를 위한 Mesh, Material Setting
	void Release();

	std::weak_ptr<class Ext_MeshComponent> OwnerMeshComponent;		// 자신을 소유한 카메라
	std::shared_ptr<class Ext_DirectXInputLayout> InputLayout = nullptr;	// Vertex Buffer와 Vertex Shader로 생성된 인풋 레이아웃 정보 저장용
	std::shared_ptr<class Ext_DirectXMesh> Mesh = nullptr;						// 정점, 인덱스 버퍼 저장용
	std::shared_ptr<class Ext_DirectXMaterial> Material = nullptr;				// 파이프라인 정보 저장용(셰이더, 레스터라이저 등)
	Ext_DirectXBufferSetter BufferSetter;													// 상수버퍼 저장용(상수버퍼, 텍스처, 블렌드 등)

	std::shared_ptr<class Ext_DirectXInputLayout> ShadowInputLayout = nullptr; // (셰도우용)Vertex Buffer와 Vertex Shader로 생성된 인풋 레이아웃 정보 저장용
	bool bIsShadow = false; // 셰도우용 OnOff
	ShadowType ShadowT = ShadowType::Unknown; // ShadowOn 하면 정해짐
};
