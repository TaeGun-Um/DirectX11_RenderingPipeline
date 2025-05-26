#pragma once

class Ext_ObjectDestroyManager
{
public:
	// constrcuter destructer
	Ext_ObjectDestroyManager() {}
	~Ext_ObjectDestroyManager() {}

	// delete Function
	Ext_ObjectDestroyManager(const Ext_ObjectDestroyManager& _Other) = delete;
	Ext_ObjectDestroyManager(Ext_ObjectDestroyManager&& _Other) noexcept = delete;
	Ext_ObjectDestroyManager& operator=(const Ext_ObjectDestroyManager& _Other) = delete;
	Ext_ObjectDestroyManager& operator=(Ext_ObjectDestroyManager&& _Other) noexcept = delete;

	static void PushToDeathQueue(const std::shared_ptr<class Ext_Object>& Object);
	static void ProcessDeathQueue();

protected:
	
private:
	static std::queue<std::shared_ptr<class Ext_Object>> DeathQueue;
	static std::unordered_set<void*> DeathProcessed;
	
};