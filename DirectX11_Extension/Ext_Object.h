#pragma once

class Ext_Object
{
public:
	// constrcuter destructer
	Ext_Object();
	~Ext_Object();

	// delete Function
	Ext_Object(const Ext_Object& _Other) = delete;
	Ext_Object(Ext_Object&& _Other) noexcept = delete;
	Ext_Object& operator=(const Ext_Object& _Other) = delete;
	Ext_Object& operator=(Ext_Object&& _Other) noexcept = delete;

	// Getter, Setter
	void UpdateOn() { IsUpdate = true; }
	void UpdateOff() { IsUpdate = false; }
	void SetName(std::string_view _Name) { Name = _Name; }
	bool GetIsDeath() { return IsDeath; }
	
	void SetOwnerScene(std::shared_ptr<class Ext_Scene> _OwnerScene) { OwnerScene = _OwnerScene; }
	std::shared_ptr<class Ext_Scene> GetOwnerScene() { return OwnerScene; }

protected:
	virtual void Start() {}
	virtual void Update(float _DeltaTime) {}
	virtual void Destroy() {}
	
private:
	std::shared_ptr<class Ext_Scene> OwnerScene = nullptr;
	std::string Name = "";
	
	bool IsUpdate = true;
	bool IsDeath = false;
};