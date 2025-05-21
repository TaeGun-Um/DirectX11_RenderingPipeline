#pragma once
#include "Ext_Actor.h"

// �������� ����ϴ� Actor
class Ext_Camera : public Ext_Actor
{
public:
	// constrcuter destructer
	Ext_Camera();
	~Ext_Camera();

	// delete Function
	Ext_Camera(const Ext_Camera& _Other) = delete;
	Ext_Camera(Ext_Camera&& _Other) noexcept = delete;
	Ext_Camera& operator=(const Ext_Camera& _Other) = delete;
	Ext_Camera& operator=(Ext_Camera&& _Other) noexcept = delete;

protected:
	void Start() override {}
	void Update(float _DeltaTime) override {}
	void Destroy() override {}
	
private:
	
};