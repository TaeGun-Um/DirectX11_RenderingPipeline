#pragma once
#include "Ext_DirectXRenderTarget.h"

class Ext_PostProcess : std::enable_shared_from_this<Ext_PostProcess>
{
	friend class Ext_DirectXRenderTarget;

public:
	// constrcuter destructer
	Ext_PostProcess();
	~Ext_PostProcess();

	// delete Function
	Ext_PostProcess(const Ext_PostProcess& _Other) = delete;
	Ext_PostProcess(Ext_PostProcess&& _Other) noexcept = delete;
	Ext_PostProcess& operator=(const Ext_PostProcess& _Other) = delete;
	Ext_PostProcess& operator=(Ext_PostProcess&& _Other) noexcept = delete;


protected:
	struct FrameData
	{
		float4 ScreenSize; // ȭ��(�Ǵ� �ؽ�ó) ũ��
		float AccTime = 0.0f; // ����� �ð�(��)
	};

	virtual void Start() = 0;
	virtual void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _Deltatime) = 0;

	std::shared_ptr<Ext_MeshComponentUnit> PostUnit;
	std::shared_ptr<Ext_DirectXRenderTarget> PostTarget;

	FrameData FData;

private:
	
};