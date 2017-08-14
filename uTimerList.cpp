#include "Arduino.h"
#include "uTimerList.h"
#include "MyTypes.h"

static bool InRange(int value, int min, int max)               // ������ true, ���� value ����� � ��������� �� min �� max (������������)
{
	return (value >= min) AND(value <= max);
}



TTimerList TimerList; 


/// ��������� �������� ��� ������� �������������
/// �� ������������ ������ 1 ������������
/// ��� Uno, Nano � ������ Micro � ATMega328, ATMega168 �������� �� ������� �1 
/// ��� Mega2560  �������� �� ������� #5
/// �� ������ �������������� ��� �����������


void TTimerList::Init()
{
	cli();

	TCCR0A = TCCR0A & 0b11111100;
	OCR0A = TIMER0_ONE_MS;
	TIMSK0 |= 0x3;
	TIFR0 = TIFR0 | 0x2;

	sei();

}

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

byte TTimerList::AvailableCount(void)
{
	return MAXTIMERSCOUNT-count;
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
	byte _sreg = SREG;								// �������� ��������� ����������
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
	SREG = _sreg;	                                  // ������ � ���������� ����� ������������ ��� ����
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

void TTimerList::TimerNewInterval(THandle hnd, long newinterval)
{
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) 
	{
		Serial << "NotInRange\n"; return;
	}
	PCallStruct item = &Items[hnd];
	if (item->CallingFunc == NULL)
	{
		Serial << "NULL\n";
		return;
	}
	cli();
	item->InitCounter = newinterval;
	item->WorkingCounter = newinterval;
	item->Active = true;
	sei();
}


ISR(TIMER0_COMPA_vect)
{

	TCNT0 = 0xFF;
	TimerList.Step();
}

