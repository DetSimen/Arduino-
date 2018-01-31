#include "Arduino.h"
#include "TTimerList.h"
#include "MyTypes.h"


/*
static bool InRange(int value, int min, int max)
{
	return (value >= min) AND(value <= max);
}
*/

TTimerList TimerList; 


/// ��������� �������� ��� ������� �������������
/// �� ������������ ������ 1 ������������
/// ��� Uno, Nano � ������ Micro � ATMega328, � �� Mega2560  �������� �� ������� #0
/// �� Atmega8 �������� �� ������� 1, �� ������� 0 ��� ���������� �� ����������
/// �� ������ �������������� ��� �����������

#if defined(__AVR_ATmega2560__) 
void TTimerList::Init()
{
	byte oldSREG = SREG;
	cli();

	TCCR0A = TCCR0A & 0b11111100;
	OCR0A = TIMER0_ONE_MS;
	TIMSK0 |= 0x3;
	TIFR0 = TIFR0 | 0x2;

	SREG = oldSREG;

}
#elif defined(__AVR_ATmega328P__)
void TTimerList::Init()
{
	byte oldSREG = SREG;
	cli();

	TCCR0A = TCCR0A & 0b11111100;
	OCR0A = TIMER0_ONE_MS;
	TIMSK0 |= 0x3;
	TIFR0 = TIFR0 | 0x2;

	SREG = oldSREG;

}
#elif defined(__AVR_ATMEGA8__)
void TTimerList::Init()
{
	byte oldSREG = SREG;
	cli();

	TCCR1A = 0; TCCR1B = 2;
	TCNT1 = 0;

	OCR1A = _1MSCONST;
	TCCR1B |= (1 << WGM12);
//	TCCR1B |= (1 << CS11) | (1 << CS10);
	
	TIMSK1 |= (1 << OCIE1A); 


	SREG = oldSREG;

}
#endif

TTimerList::TTimerList()
{
	count = 0;
	for (byte i = 0; i < MAXTIMERSCOUNT; i++)  // �������������� �������������, ���������� ������ ������
	{
		Items[i].CallingFunc    = NULL;
		Items[i].InitCounter    = 0;
		Items[i].WorkingCounter = 0;
		Items[i].Active          = false;
	}
	active = false;
}

THandle TTimerList::Add(PVoidFunc AFunc, long timeMS)
{
	for (THandle i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc == NULL)         // ����������� �� ������ ��������
		{                                         // ���� ����� ������ �����, ��������� ������
			byte sreg = SREG; cli();
			Items[i].CallingFunc	= AFunc;
			Items[i].InitCounter	= timeMS;
			Items[i].WorkingCounter = timeMS;
			Items[i].Active          = true;
			count++;
			if (NOT active)  AllStart();
			SREG = sreg;
			return i;
		}
	}
	return 0xFF;                                   // ���� ��� - ������ ��� ������ (-1) 
}

THandle TTimerList::AddSeconds(PVoidFunc AFunc, word timeSec)
{
	return Add(AFunc, 1000L*timeSec);          // ���������� ��� ������ 
}

THandle TTimerList::AddMinutes(PVoidFunc AFunc, word timeMin)
{
	return Add(AFunc, timeMin*60L*1000L);     // ���������� ��� �����
}



bool TTimerList::CanAdd() const
{
	for (byte i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc != NULL) continue;  // ���� � ������ ���� ������ �����, ���� ����� �������� ������
		return true;                                 // ������ true
	}
	return false;                                    // ���� ��� - �� false
}

bool TTimerList::IsActive() const
{
	return active;                                   // ���� ���� ���� ������ �������, ������ true, ���� ��� ����������� - false
}

void TTimerList::Delete(THandle hnd)                 // ������� ������ � ������� hnd.     
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT - 1))
	{
		byte sreg = SREG; cli();
		Items[hnd].CallingFunc		= NULL;
		Items[hnd].InitCounter		= 0;
		Items[hnd].WorkingCounter	= 0;
		Items[hnd].Active           = false;
		if (count > 0)  count--;
		if (count==0) AllStop();                        // ���� ��� ������� �������, ���������� � ���� �������� ��������
		SREG = sreg;
	}
}

/// ��� ������� ���������� ��� ���������� ������������ ������� ������ ������������
/// �������� ���� ���������� ���������� ������������

void TTimerList::Step(void)
{
	if (NOT active) return;                         // ���� ��� ������� �����������, �� � ������ ���
	byte _sreg = SREG;								// �������� ��������� ����������
			// ������ ������� ����                      
	for (THandle i = 0; i < MAXTIMERSCOUNT; i++)   // ��������� �� ����� ������ ��������
	{
		cli();                                          // ���� ����� �� ������� �������� ���������, ��������� ����������
		if (Items[i].CallingFunc == NULL)  continue;  // ���� �������-���������� �� ���������, ������ �� ��������� ���� 
		if (NOT Items[i].Active)           continue; // ���� ������ ���������� - ����
		if (--Items[i].WorkingCounter > 0) continue;  // ��������� �� 1 ������� �������
		Items[i].WorkingCounter = Items[i].InitCounter; // � ���������� � ������� ������� ��������� �������� ��� ����� �������
		sei();
		Items[i].CallingFunc();                       // ���� ������ 0, �������� �������-����������
	}
	SREG = _sreg;	                                  // ������ � ���������� ����� ������������ ��� ����
}

void TTimerList::AllStart()                              
{
	if (NOT active) Init();                              // ��� ���������� ������� �������, �������������� ���������� ������
	active = true;
}

void TTimerList::AllStop()
{
	active = false;                                   // ���������� ��� �������
}

void TTimerList::TimerStop(THandle hnd)
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT-1))
	{
		Items[hnd].Active = false;                    // ���������� ������ ����� hnd
	}
}

bool TTimerList::TimerActive(THandle hnd)
{
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return false;
	else return Items[hnd].Active;
}

void TTimerList::TimerStart(THandle hnd)              // ��������� ������������� ������ ����� hnd
{
	if (isValid(hnd))
	{
		if (Items[hnd].Active)                   return;
		byte old = SREG; 
		cli();
		Items[hnd].WorkingCounter = Items[hnd].InitCounter;  // � ������ ������ ��������� �������
		Items[hnd].Active = true;
		SREG = old;
	}
}

void TTimerList::TimerResume(THandle hnd)                 // ���������� ���� ������� ����� �����
{
	if (isValid(hnd)) Items[hnd].Active = true;
}

void TTimerList::TimerPause(THandle hnd)				// ��������� ������ �� �����
{
	if (isValid(hnd)) Items[hnd].Active = false;
}

bool TTimerList::isValid(THandle hnd)					// private �������, c������ �� �����
{
	return (InRange(hnd, 0, MAXTIMERSCOUNT) AND (Items[hnd].CallingFunc != NULL));
}

void TTimerList::TimerNewInterval(THandle hnd, long newinterval)	// ��������� ������� ����� ��������
{
	if (isValid(hnd))
	{
		TimerStop(hnd);
		Items[hnd].InitCounter = newinterval;
		TimerStart(hnd);
	}
}

#if defined(__AVR_ATMEGA8__)

ISR(TIMER1_COMPA_vect)
{
	OCR1A = _1MSCONST;
	TimerList.Step();
}
#else
ISR(TIMER0_COMPA_vect)
{

	TCNT0 = 0xFF;
	TimerList.Step();
}
#endif

ISR(PCINT0_vect)
{

}

