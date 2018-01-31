#pragma once
#include "Arduino.h"



#define AND &&
#define OR ||
#define NOT !


using	PVoidFunc = void(*)(void);      // ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)  


#ifdef __AVR_ATmega2560__
 #define     	MAXTIMERSCOUNT		16		// 	������������ ����� ������������������ �������� ��� Mega2560 - 16
 #define	TIMER0_ONE_MS	247
#endif

#ifdef __AVR_ATMEGA8__
 #define	 MAXTIMERSCOUNT		8
 #define	 _1MSCONST		F_CPU/8/1000 //	�������� ��� 1 ������������  12��� - 1500, 16��� - 2000 ����� ��������� ������� 
											//  ����� ������������ 16 ��� ��������� �������� �������� 10000 ������
#endif

#ifdef __AVR_ATmega328P__
#define  MAXTIMERSCOUNT		12		// 	������������ ����� ������������������ �������� ��� ������ Uno - 12
#define	 TIMER0_ONE_MS	247
//#define	 ARDUINO_AVR_UNO
#endif


using THandle = byte;

#pragma pack(push,1)
struct TCallStruct					    // ���������� ��������� ��� �������� ������� �������
{
	PVoidFunc	CallingFunc;			// �������, ������� ����� ������� ��� ������������
	long		InitCounter;			// �������� ���-�� �����������
	long		WorkingCounter;			// ������� �������. ����� ������ ����� ���� ���������� �������� �� InitCounter
	bool        	Active;	  			    // � ������ ������������ ����������� �� 1. ��� ����������� 0 ���������� ������� 
							            // CallingFunc, � ����� ������������ �������� �� InitCounter. ���� ���������� �������. :)
};
#pragma pack(pop)
using PCallStruct = TCallStruct *;

class TTimerList
{
private:
	TCallStruct		Items[MAXTIMERSCOUNT];
	void			Init();
	bool			active;
	byte			count;

// ��������, ��� ������ ����������, ��� ����� ����������
// � callback ������� ���������
	bool			isValid(THandle hnd);				

public:
	TTimerList();


	THandle      Add(PVoidFunc AFunc, long timeMS);	        // ������� AFunc, ������� ���� ������� ����� timeMS �����������
	THandle      AddSeconds(PVoidFunc AFunc, word timeSec); // �� ��, ������ �������� �������� � ��������. 
	THandle      AddMinutes(PVoidFunc AFunc, word timeMin); // �� ��, ������ �������� �������� � �������. 
															// ������������� ��� �������� �������� ������� ���������� 
// ������� ��� ������ � �����

		inline bool  	CanAdd() const;                    // ���� ����� �������� ������, ������ true

		inline bool	IsActive() const;                  // true, ���� ���� ���� ������ �������

		void		Delete(THandle hnd);               // ������� ������ hnd

		void		Step(void);                

		void		AllStart();                        // ��������� ������� ���� �������� ��������

		void		AllStop();                         // ��������� ��� �������, TimerList ����������

		inline byte	Count() const {return count; };    // ������� ����������� ��������, �����. ����� �� ������������, ������ ��������� �����������
		 	                                            // ����������, ������� �-��� CanAdd();

		inline byte MaxTimersCount() const { return MAXTIMERSCOUNT; }; // ������������ ����� ��������


//  ������� ��� ������ ����������� �������, ��������� ��� ������� (THandle)

		void	TimerPause(THandle hnd);	//������������� ������ hnd

		void	TimerResume(THandle hnd);	// ���������� ���� ��� ������� hnd � �������������� �����

		void	TimerStop(THandle hnd);		// ���������� ��������� ������ 

		bool	TimerActive(THandle hnd);       // ������� �� ���������� ������

		void    TimerStart(THandle hnd);        // ��������� ��������� ������ (����� ���������) 
		
		void	TimerNewInterval(THandle hnd, long newinterval); // ��������� ������� hnd ����� ��������
};



extern TTimerList TimerList;                            // ���������� ����� ����� � �������� ������

