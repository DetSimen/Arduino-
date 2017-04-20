#include "Arduino.h"
#include "uTimerList.h"

#define AND &&
#define OR  ||
#define NOT !

TTimerList TimerList;    

/// ��������� �������� ��� ������� �������������
/// �� ������������ ������ 1 ������������
/// ��� Uno, Nano � ������ Micro � ATMega328, ATMega168 �������� �� ������� �1 
/// ��� Mega2560  �������� �� ������� #5
/// �� ������ �������������� ��� �����������


#ifdef ARDUINO_AVR_MEGA2560
void TTimerList::Init()
{
	cli();
	TCCR5A = 0; TCCR5B = 0;
	TCNT5 = 0;

	OCR5A = _1MSCONST;
	TCCR5B |= (1 << WGM52);
	TCCR5B |= (1 << CS51);// | (1 << CS10);
	TIMSK5 |= (1 << OCIE5A);
	sei();
}
#else  // Uno, Nano, Micro
void TTimerList::Init()
{
	cli();
	TCCR1A = 0; TCCR1B = 0;
	TCNT1 = 0;

	OCR1A = _1MSCONST;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS11);// | (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);
	sei();
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
			cli();
			Items[i].CallingFunc	= AFunc;
			Items[i].InitCounter	= timeMS;
			Items[i].WorkingCounter = timeMS;
			Items[i].Active          = true;
			count++;
			if (NOT active)  Start();
			sei();
			return i;
		}
	}
	return -1;                                   // ���� ��� - ������ ��� ������ (-1) 
}

THandle TTimerList::AddSeconds(PVoidFunc AFunc, word timeSec)
{
	return Add(AFunc, 1000L*timeSec);          // ���������� ��� ������ 
}

THandle TTimerList::AddMinutes(PVoidFunc AFunc, word timeMin)
{
	return Add(AFunc, timeMin*60L*1000L);     // ���������� ��� �����
}

bool TTimerList::CanAdd()
{
	for (byte i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc != NULL) continue;  // ���� � ������ ���� ������ �����, ���� ����� �������� ������
		return true;                                 // ������ true
	}
	return false;                                    // ���� ��� - �� false
}

bool TTimerList::IsActive()
{
	return active;                                   // ���� ���� ���� ������ �������, ������ true, ���� ��� ����������� - false
}

void TTimerList::Delete(THandle hnd)                 // ������� ������ � ������� hnd.     
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT - 1))
	{
		cli();
		Items[hnd].CallingFunc		= NULL;
		Items[hnd].InitCounter		= 0;
		Items[hnd].WorkingCounter	= 0;
		Items[hnd].Active           = false;
		if (count > 0)  count--;
		if (count==0) Stop();                        // ���� ��� ������� �������, ���������� � ���� �������� ��������
		sei();
	}
}

/// ��� ������� ���������� ��� ���������� ������������ ������� ������ ������������
/// �������� ���� ���������� ���������� ������������

void TTimerList::Step(void)
{
	if (NOT active) return;                         // ���� ��� ������� �����������, �� � ������ ���
	cli();                                          // ���� ����� �� ������� �������� ���������, ��������� ����������
// ������ ������� ����                      
	for (THandle i = 0; i < MAXTIMERSCOUNT; i++)   // ��������� �� ����� ������ ��������
	{
		if (Items[i].CallingFunc == NULL)  continue;  // ���� �������-���������� �� ���������, ������ �� ��������� ���� 
		if (NOT Items[i].Active)           continue; // ���� ������ ���������� - ����
		if (--Items[i].WorkingCounter > 0) continue;  // ��������� �� 1 ������� �������
		Items[i].CallingFunc();                       // ���� ������ 0, �������� �������-����������
		Items[i].WorkingCounter = Items[i].InitCounter; // � ���������� � ������� ������� ��������� �������� ��� ����� �������
	}
	sei();                                            // ������ � ���������� ����� ���������
}

void TTimerList::Start()                              
{
	if (NOT active) Init();                              // ��� ���������� ������� �������, �������������� ���������� ������
	active = true;
}

void TTimerList::Stop()
{
	active = false;                                   // ���������� ��� �������
}

byte TTimerList::Count()
{
	return count;                                     // ������� ����������� ��������
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
	if (NOT InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (Items[hnd].CallingFunc == NULL)      return;
	if (Items[hnd].Active)                   return;
	cli();
	Items[hnd].WorkingCounter = Items[hnd].InitCounter;  // � ������ ������ ��������� �������
	Items[hnd].Active = true;
	sei();
}

#ifdef ARDUINO_AVR_MEGA2560                              // ���������� ���������� ����������� �������
ISR(TIMER5_COMPA_vect)
{
	OCR5A = _1MSCONST;
	TimerList.Step();
}

#else
ISR(TIMER1_COMPA_vect)
{
	OCR1A = _1MSCONST;
	TimerList.Step();
}
#endif


bool InRange(int value, int min, int max)               // ������ true, ���� value ����� � ��������� �� min �� max (������������)
{
	return (value >= min) AND (value <= max);
}
