#include "PrecompileHeader.h"
#include "Ext_FSM.h"

void Ext_FSM::CreateState(const StateParameter& _StateFunction)
{
	if (nullptr != FindState(_StateFunction.StateValue))
	{
		MsgAssert("�̹� �����ϴ� �̸��� ������Ʈ�� �� ������� �߽��ϴ�." + std::to_string(_StateFunction.StateValue));
	}

	State& NewState = AllState[_StateFunction.StateValue];

	NewState.StateValue = _StateFunction.StateValue;
	NewState.End = _StateFunction.End;
	NewState.Start = _StateFunction.Start;
	NewState.Update = _StateFunction.Update;
}

void Ext_FSM::ChangeFunction(int _State, std::function<void()> _Start,
	std::function<void(float _Delta)> _Update,
	std::function<void()> _End
)
{
	if (nullptr == FindState(_State))
	{
		MsgAssert("�̹� �����ϴ� �̸��� ������Ʈ�� �� ������� �߽��ϴ�." + std::to_string(_State));
	}

	Ext_FSM::State* Fsm = FindState(_State);

	Fsm->Start = _Start;
	Fsm->End = _End;
	Fsm->Update = _Update;

}

void Ext_FSM::Update(float _DeltaTime)
{
	if (nullptr == CurState)
	{
		MsgAssert("���� ������Ʈ�� �������� �ʽ��ϴ�");
	}

	CurState->Update(_DeltaTime);
}

void Ext_FSM::ChangeState(int _State)
{
	if (nullptr != CurState)
	{
		if (nullptr != CurState->End)
		{
			CurState->End();
		}
	}

	CurState = FindState(_State);

	if (nullptr == CurState)
	{
		MsgAssert("�������� �ʴ� ������Ʈ�� �̵��Ϸ��� �߽��ϴ�" + std::to_string(_State));
	}

	CurState->Start();
}

Ext_FSM::State* Ext_FSM::FindState(int _State)
{

	if (AllState.end() == AllState.find(_State))
	{
		return nullptr;
	}

	return &AllState[_State];
}