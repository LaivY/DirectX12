#pragma once
#include "stdafx.h"

class Timer
{
public:
	Timer();
	~Timer() = default;

	void Start();
	void End();
	FLOAT GetDeltaTime() const { return m_deltaTime; }

private:
	LARGE_INTEGER	m_start;
	LARGE_INTEGER	m_end;
	LARGE_INTEGER	m_timer;
	FLOAT			m_deltaTime;
};