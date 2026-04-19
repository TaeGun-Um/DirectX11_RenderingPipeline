#pragma once
#include "RotatingMeshActor.h"

class ReflectionActor : public RotatingMeshActor
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

	void SetReflection();

protected:
	void Start() override;

private:
	std::shared_ptr<class Ext_ReflectionComponent> Reflection;

};
