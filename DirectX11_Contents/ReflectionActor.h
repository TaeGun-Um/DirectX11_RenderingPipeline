#pragma once
#include <DirectX11_Extension/Ext_Actor.h>

class ReflectionActor : public Ext_Actor
{
public:
	// constrcuter destructer
	ReflectionActor() {}
	~ReflectionActor() {}

	// delete Function
	ReflectionActor(const ReflectionActor& _Other) = delete;
	ReflectionActor(ReflectionActor&& _Other) noexcept = delete;
	ReflectionActor& operator=(const ReflectionActor& _Other) = delete;
	ReflectionActor& operator=(ReflectionActor&& _Other) noexcept = delete;

	void SetRotate() { bIsRotation = true; }
	void SetReflection();

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	float AccTime = 0;
	bool bIsRotation = false;
	std::shared_ptr<class Ext_MeshComponent> MeshComp;
	std::shared_ptr<class Ext_ReflectionComponent> Reflection;

};