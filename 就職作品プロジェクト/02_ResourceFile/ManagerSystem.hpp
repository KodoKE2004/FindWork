#pragma once
#include <memory>

class ManagerSystem
{
protected:
    std::unique_ptr<ManagerSystem> m_Instance;
public:
    ManagerSystem* GetInstance(){
        return m_Instance.get();
    }
};