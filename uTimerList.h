#pragma once


using	PVoidFunc = void(*)(void);      // ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)  

#ifdef ARDUINO_AVR_MEGA2560
 #define     MAXTIMERSCOUNT		16		// 	������������ ����� ������������������ �������� ��� Mega2560 - 16
#else
 #define     MAXTIMERSCOUNT		8		// 	������������ ����� ������������������ �������� ��� ������ Uno - 8
#endif

#define	    _1MSCONST			1990	//	�������� ��� 1 ������������  12��� - 1500, 16��� - 2000 ����� ��������� ������� 
                                        //  ����� ������������ 16 ��� ��������� �������� �������� 10000 ������



extern bool InRange(int,int,int);

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

		bool         CanAdd();                          // ���� ����� �������� ������, ������ true

		bool		 IsActive();                        // true, ���� ���� ���� ������ �������

		void		 Delete(THandle hnd);               // ������� ������ hnd

		void		 Step(void);                

		void		 Start();                           // ��������� ������� ���� �������� ��������

		void		 Stop();                            // TimerList ��������, ��� ������� �����������

		byte		 Count();                           // ������� ����������� ��������, �����. ����� �� ������������, ������ ��������� �����������
		                                                // ����������, ������� �-��� CanAdd();

		void         TimerStop(THandle hnd);            // ���������� ��������� ������ �� ��� ������

		void         TimerStart(THandle hnd);           // ��������� ��������� ������ (����� ���������) 
		                                                //���� �� ������������, � ���������� �������
};

extern TTimerList TimerList;                            // ���������� ����� ����� � �������� ������

