#pragma once

#define AND &&
#define OR ||
#define NOT !


using	PVoidFunc = void(*)(void);      // ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)  

 #define     MAXTIMERSCOUNT		16		// 	������������ ����� ������������������ �������� ��� Mega2560 - 16
                                        //  ����� ������������ 16 ��� ��������� �������� �������� 10000 ������
#define		TIMER0_ONE_MS		246



using THandle = int8_t;

#pragma pack(push,1)
struct TCallStruct					    // ���������� ��������� ��� �������� ������� �������
{
	PVoidFunc	CallingFunc;			// �������, ������� ����� ������� ��� ������������
	long		InitCounter;			// �������� ���-�� �����������
	long		WorkingCounter;			// ������� �������. ����� ������ ����� ���� ���������� �������� �� InitCounter
	bool        Active;	  			    // � ������ ������������ ����������� �� 1. ��� ����������� 0 ���������� ������� 
							            // CallingFunc, � ����� ������������ �������� �� InitCounter. ���� ���������� �������. :)
};
using PCallStruct = TCallStruct *;

class TTimerList
{
	private:
		TCallStruct		Items[MAXTIMERSCOUNT];
		void			Init();
		bool			active;
		byte			count;

	public:
		TTimerList();

			
    	THandle      Add(PVoidFunc AFunc, long timeMS);	        // ������� AFunc, ������� ���� ������� ����� timeMS �����������
		THandle      AddSeconds(PVoidFunc AFunc, word timeSec); // �� ��, ������ �������� �������� � ��������. 
		THandle      AddMinutes(PVoidFunc AFunc, word timeMin); // �� ��, ������ �������� �������� � �������. 
														        // ������������� ��� �������� �������� ������� ���������� 

		byte		 AvailableCount(void);

		bool         CanAdd();                          // ���� ����� �������� ������, ������ true

		bool		 IsActive();                        // true, ���� ���� ���� ������ �������

		void		 Delete(THandle hnd);               // ������� ������ hnd

		void		 Step(void);                

		void		 Start();                           // ��������� ������� ���� �������� ��������

		void		 Stop();                            // TimerList ��������, ��� ������� �����������

		byte		 Count();                           // ������� ����������� ��������, �����. ����� �� ������������, ������ ��������� �����������
		                                                // ����������, ������� �-��� CanAdd();

		void         TimerStop(THandle hnd);            // ���������� ��������� ������ �� ��� ������

		bool         TimerActive(THandle hnd);          // ������� �� ���������� ������

		void         TimerStart(THandle hnd);           // ��������� ��������� ������ (����� ���������) 
		                                                //���� �� ������������, � ���������� �������
		
		void		 TimerNewInterval(THandle hnd, long newinterval); // ��������� ������� hnd ����� ��������
};

extern TTimerList TimerList;                            // ���������� ����� ����� � �������� ������

