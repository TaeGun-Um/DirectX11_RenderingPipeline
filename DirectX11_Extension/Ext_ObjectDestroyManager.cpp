#include "PrecompileHeader.h"
#include "Ext_ObjectDestroyManager.h"

#include "Ext_Object.h"

std::queue<std::shared_ptr<Ext_Object>> Ext_ObjectDestroyManager::DeathQueue;
std::unordered_set<void*> Ext_ObjectDestroyManager::DeathProcessed;

void Ext_ObjectDestroyManager::PushToDeathQueue(const std::shared_ptr<Ext_Object>& Object)
{
    if (!Object || DeathProcessed.contains(Object.get()))
    {
        return;
    }

    DeathQueue.push(Object);
    DeathProcessed.insert(Object.get());
}

void Ext_ObjectDestroyManager::ProcessDeathQueue()
{
    while (!DeathQueue.empty())
    {
        std::shared_ptr<Ext_Object> Object = DeathQueue.front();
        DeathQueue.pop();

        if (!Object)
        {
            continue;
        }

        std::string Nae = Object->GetName(); // 누구세요
        Object->Destroy(); // ? 가상 함수 호출됨
    }

    DeathProcessed.clear(); // 다음 프레임을 위해 클리어
}