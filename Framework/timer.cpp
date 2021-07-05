#include "timer.h"

Timer::Timer() : m_start{}, m_end{}, m_deltaTime{ 0.0f }
{
	QueryPerformanceFrequency(&m_timer);
}

void Timer::Start()
{
	QueryPerformanceCounter(&m_start);
	m_end = {};
}

void Timer::End()
{
	QueryPerformanceCounter(&m_end);
	m_deltaTime = (m_end.QuadPart - m_start.QuadPart) / static_cast<FLOAT>(m_timer.QuadPart);
}