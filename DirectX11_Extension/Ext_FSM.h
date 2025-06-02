#pragma once

class StateParameter
{
public:
	int StateValue;
	std::function<void()> Start;
	std::function<void(float _DeltaTime)> Update;
	std::function<void()> End;
};

class Ext_FSM
{
public:
	class State
	{
	public:
		int StateValue;
		float CurStateTime;
		std::function<void()> Start;
		std::function<void(float _DeltaTime)> Update;
		std::function<void()> End;
	};

	// constrcuter destructer
	Ext_FSM() {}
	~Ext_FSM() {}

	// delete Function
	Ext_FSM(const Ext_FSM& _Other) = delete;
	Ext_FSM(Ext_FSM&& _Other) noexcept = delete;
	Ext_FSM& operator=(const Ext_FSM& _Other) = delete;
	Ext_FSM& operator=(Ext_FSM&& _Other) noexcept = delete;

	void ChangeState(int _StateValue);
	void CreateState(const StateParameter& _StateFunction);

	void Update(float _DeltaTime);
	void ChangeFunction(int _StateValue, std::function<void()> _Start, std::function<void(float _DeltaTime)> _Update, std::function<void()> _End);

	bool IsValid() { return CurState != nullptr; }

	int GetCurState() { return CurState->StateValue; }

protected:
	Ext_FSM::State* FindState(int _State);
	
private:
	std::map<int, State> AllState;
	State* CurState = nullptr;

};