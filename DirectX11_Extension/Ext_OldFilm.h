#pragma once
#include "Ext_PostProcess.h"

struct OldFilmData
{
	float4 OldFilmValue; // x 성분만 사용
};

class Ext_OldFilm : public Ext_PostProcess
{
public:
	// constrcuter destructer
	Ext_OldFilm() {}
	~Ext_OldFilm() {}

	// delete Function
	Ext_OldFilm(const Ext_OldFilm& _Other) = delete;
	Ext_OldFilm(Ext_OldFilm&& _Other) noexcept = delete;
	Ext_OldFilm& operator=(const Ext_OldFilm& _Other) = delete;
	Ext_OldFilm& operator=(Ext_OldFilm&& _Other) noexcept = delete;

protected:
	void Start() override;
	void PostProcessing(Ext_DirectXRenderTarget* _MainRenderTarget, float _DeltaTime) override;
	
private:
	std::vector<std::shared_ptr<class Ext_DirectXTexture>> Textures;
	OldFilmData OFData;
	float AccTime = 0.f;
	int CurIndex = 0;
	int MaxIndex = 0;
};