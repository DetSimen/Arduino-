<H2>Много таймеров из одного</H2>


Основная задача, стоящая перед горе-программистом AVR это не строгий математический анализ каких-нить бестолковых данных, не быстрое преобразование Фурье и даже не унылое мигание светлодиодиком. Основная задача, занимающая 80% программы - измерение временных интервалов. Чаще всего требуется узнать, скока времени прошло перед/после какого-нить события, чтобы не пропустить и вовремя откликнуться на него. Кто-то (90% новичков) жизни своей не мыслят без функции delay(), кто-то постоянно теребонькает несчастную millis(), зачастую, делая это неправильно и со священным ужасом ожидая страшного переполнения.  Сложность в том, что  временных интервалов по ходу программы требуется много, часто и, как правило, разных.  Например, время конверсии даччика DS18B20 при максимальном 12-битовом разрешении составляет 750 миллисекунд, и раньше, чем вычислится температура опрашивать снова его бессмысленно.  Для других даччиков время опроса будет совершенно другое, и временной интервал для него, соотвецтвенно - тоже.  На экран информацию чаще 1 раза в 50 миллисекунд выводить тоже неприлично, только лишняя трата ресурсов, нервов и самого экранчика, значит опять надо где-то хранить время, когда вывод осуществлялся последний раз, чтоб не обращаться к нему чаще положенного. Везде, куда не посмотришь - время, время и время... 

Кто-то, не мудрствуя лукаво, использует для своих целей delay() и не парится, даже не понимая, что процессор во время выполнения этой функции просто висит в холостом цикле, не делая ничего полезного, и не способен исполнять что-то иное (на самом деле способен, канеш, но для новичков это тайна).  В некоторых задачах, будь то простейший Blink или управление светлодиодной лентой это не критично, но если delay() используется в программе управления газовой горелкой, тут впору эвакуировать из дома родных и близких (и кота) на время эксплуатации такой горелки.  

Более продвинутые открыли для себя функцию millis() и поглядывают/поплевывают на делейщиков с высоты своего воображаемого Олимпа как исполненные благодати Свидетели Переполнения. И пестрят их программы чуть более чем полностью конструкциями вида 

	if (millis()-lastmillis > myinterval) dosomething();

а то и 

	if (lastmillis+myinterval>millis()) dosomething();

но последние, обычно, к исходу 49-х суток взрываются вместе с переполнившимся millis-om() и угрозы больше не представляют.  

delay() похож на дубину, которой нерадивый пользователь со всего маху шарашит бедный процессор по голове, после чего ему становится очень трудно стоять без сознания и он валяется в отключке некоторое заданное количество времени. Естественно, ничего другого он делать в этом состоянии не способен.  

millis() в этом отношении гораздо гуманнее. Он похож на хозяина раба, который вручил ему наручные часы, и строго настрого наказал следить за временем, если не хочется получить дубиной по башке, как в прошлый раз.  И вот человечек, выполняя свою нехитрую работу, всё время пялится на часы, чтоб узнать не прошло ли достаточно времени, чтоб включить/выключить что то важное.  Иначе, не уследишь/забудешь и улетишь вабнимку с газовым котлом Марс заселять вместе с родными и близкими (и котом), а жалка. 
Такое можно, канеш, потерпеть, если временной интервал один, но как только их становится несколько, да еще и друг от друга зависящих, выполнение задания превращается в атнюдь не тривиальную задачу (мошт, лучше дубиной, а?).

То что предлагаю я, можно назвать будильником. Человек заводит себе несколько будильников на разное время и спокойно занимается своими делами.  По мере срабатывания каждого, можно ненадолго отвлечься, включить/выключить то что нужно/ненужно и спокойно продолжить заниматься своими делами дальше.  Если действие было однократное, будильник можно отключить, а если совсем-совсем однократное - то и выбросить.  Если действие периодическое, а так чаще всего и бывает, ничего делать не нужно, через ранее заданный интервал будильник сам по себе сработает снова.

В основе всего этого священнодействия лежит самодостаточный класс TCounterDown, который, как ясно из названия является счетчиком вниз, до нуля. Класс самодостаточный, его можно применять самостоятельно для подсчета внешних событий. Каких? Да каких угодно, например, сколько раз человек нажал на кнопку, допустим, звонка. Если больше 10, то выйти и спросить, чо звонишь-то? видишь дома нет никто?  или активировать механическую руку в боксерской перчатке, которая автоматически колотит слишком настойчивого посетителя по темечку (да, это я Том и Джерри насмотрелса за время сынова децтва). 

У класса есть конструктор, который принимает начальное значение счетчика событий и адрес функции, которая вызовется когда это количество событий произойдёт. 

	TCounterDown(uint32_t Counter, pvfCallback CallbackFunc) 

тип pvfCallback - это указатель на функцию ничего не принимающую и ничего не отдающую

	using pvfCallback = void(*)(void); // тип pointer to void function Callback

ее задача только в том, чтобы просигнализировать основной программе, что требуемое количество событий случилось (прозвонить будильником).

еще, из главных, у класса есть функция Tick() которая уменьшает заданный в конструкторе счетчик событий, и если он достиг 0, т.е требуемое кол-во событий случилось - то она и вызывает пользовательскую функцию CallbackFunc. После этого, внутренний счетчик вновь заполняется первоначальным значением автоматически и подсчёт событий начинается сначала. Для удобства программирования Tick() завернут в перегруженный оператор --, чтоб при наступлении события вызывать его както так 

	Count--; 

у класса есть еще несколько вспомогательных функций, которые могут пригодиться.  Если нужно изменить счетчик событий, например, считать не 10 а 20 нажатий, есть функция 

	// установить новый интервал для счета
	void setInterval(uint32_t anewinterval);

счетчик можно остановить, запустить снова с того же места или сбросить счетчик на первоначальное значение

	// остановить счетчик (поставить на паузу)
	inline void Stop(void);

	// продолжить считать с того же места, где остановили	
	inline void Start(void);

	// сбросить счетчик, считать сначала
	inline void Reset(void);

кроме того, функцией isActive() можно проверить, что счетчик не остановлен, а функцией isEmpty() проверить, что назначена функция обратного вызова.  
Полностью публичный интерфейс класса выглядит так 

	class TCounterDown {
	public:
	
	// конструктор. Принимает первоначальное значение, которое потом будет уменьшаться при каждом вызове 
	// Tick() и указатель на void функцию обратного вызова.

	TCounterDown(uint32_t aTimeMS, pvfCallback aCallback);


	// остановить счетчик (поставить на паузу)

	inline void Stop(void);


	// продолжить считать с того же места, где остановили	

	inline void Start(void);

	// сбросить счетчик, считать сначала

	inline void Reset(void);


	// отдает true - если счетчик запущен и считает, и false, если остановлен

	inline bool isActive(void) const ;


	// отдает true - если функция Callback не назначена

	inline bool isEmpty(void) const ;


	// установить новый интервал для счета

	void setInterval(uint32_t anewinterval);


	// Оператор постдекремента. Уменьшает рабочий счетчик на 1 за вызов, если он не остановлен. Если 	   счетчик дошел до 0, 
	// и назначена функция Callback, то она вызывается

	TCounterDown &operator --(int);


	// то же самое в виде функции, а не оператора

	void Tick(void) { (*this)--; };  // просто вызываем предыдущий оператор постдекремента
	}

Повторюсь, класс самодостаточен, и может использоваться для подсчёта любых событий, не обязательно временнЫх. Но далее он понадобится, чтобы считать именно время. 

Традиционно, чтобы подсчитать кол-во времени, которое прошло с какого либо события, принято использовать таймеры, встроенные в AVR процессор (аппаратные таймеры). Даже millis() в глубине своей души - ни что иное, как аппаратный 8-битный таймер #0, настроенный на переполнение 1 раз в 255 тиков. Изначально, после сброса и инициализации он настроен так, чтобы между двумя переполнениями проходило ровно 1024 миллисекунды.  Путем сложных и не очень вычислений, каждое переполнение на 1 увеличивает значение переменной, хранящей то, что и отдает потом функция millis() при вызове. Каждые N-периодов значение корректируется, чтобы быть кратным миллисекунде, а не 1.024 мс, но это нам пока не важно. Плохая новость в том, что аппаратных таймеров у процессора не так много, от 2 до 5 у Меги2560, а временных интервалов надо мерять зачастую гораздо больше. На всё таймеров не напасесся. Выход в том, чтобы используя один аппаратный таймер, создать сколько угодно программных таймеров (ограничение только в размере ОЗУ, ведь их надо где-то хранить) и тут то нам как раз и пригодятся рассмотренные выше счётчики, нужно просто собрать их в массив или список. А при срабатывании аппаратного таймера, нужно просто перебрать все созданные программные счётчики, уменьшить на единицу значение каждого и если какой-нибудь из них достиг 0, то он сам вызовет функцию, которая известит основную программу, что заданный временной интервал прошел, необходимо выполнить какое-то действие. 

Минимальный интервал срабатывания счетчиков я выбрал 1 миллисекунду, и вот почему.  Во-первых, можно использовать тот же таймер, на котором висит millis() и не занимать еще один ценный ресурс, каковым является аппаратный таймер. Во-вторых, на 16 МГц процессоре за 1 миллисекунду может выполниться примерно 10000 инструкций процессора, чего вполне достаточно, для полноценного отклика на сработавший будильник и выполнение нужного действия. Для этого нужно просто определить прерывание по совпадению от таймера #0, а чтоб и millis() работал, то когда придёт совпадение, счетчику таймера присваивается 0xFF, тогда следующим шагом вызовется прерывание по переполнению. Millis(), правда будет врать процентов на 3, но кто это заметит? И это, в Атмега8 таймер #0 обрезан, прерывания по совпадению у него нет, поэтому там пришлось полноценно использовать таймер #1. Жалка.
В моих программах, мне ни разу не приходилось измерять время микросекундами, правда осциллографы реального времени и высокочастотные генераторы я не делаю, а всевозможные сенсоры и исполнительные устройства живут в масштабах секунд и более. Самый минимальный интервал, который я использую - 20 миллисекунд, для сканирования клавиатуры, остальные интервалы - минимум в 10 раз больше. 

Поэтому следующий класс для рассмотрения - TTimerList, которыйхранит в себе список программных счетчиков TCounterDown, которые считают число миллисекунд. Каждое срабатывание аппаратного таймера, TTimerList пробегает по списку сохраненных счетчиков и выполняет декремент каждого.  При достижении 0 счетчик сам вызовет функцию обратного вызова (уведомление, будильник), и отсчет пойдет сначала. 
Интерфейс TTimerList прост как 2 копейки, у него есть функции как для всего списка (остановить/запустить), так и для каждого отдельного таймера. 

	class TTimerList   {
	public:
	// конструктор, создает пустой список размера asize, для хранения счетчиков
	// при создании забивает список NULL-ами

	TTimerList(uint8_t asize) {


	// добавить счетчик в список
	// ainterval   задается в миллисекундах
	// acallback - адрес функции, которая вызовется, когда счетчик досчитает до 0
	// 
	// возвращает Handle щёччика (номер под которым он был добавлен), 
	// или специальное значение INVALID_HANDLE, если 
	// добавить не удалось, нет места в списке

	THandle Add(uint32_t ainterval, pvfCallback acallback);


	// то же, но счетчик создается в остановленном состоянии
	// для запуска нужно потом вызвать TimerList.Start(hnd)
	// 
	THandle AddStopped(uint32_t ainterval, pvfCallback acallback);


	// запустить цикл перебора счетчиков

	inline void Start(void) { 


	// остановить цикл перебора счётчиков

	inline void Stop(void) { 


	// отдает true - если цикл перебора запущен

	inline bool isActive(void) const { 



	// функции для работы с конкретным счётчиком в списке, по его хэндлу


	// запустить счётчик с номером hnd

	void Start(THandle hnd);


	// остановить счётчик hnd

	void Stop(THandle hnd);


	// Перезапустить счётчик hnd сначала

	void Reset(THandle hnd);


	// Установить счетчику hnd новый интервал счёта

	void setNewInterval(THandle hnd, uint32_t anewinterval);

	
	// проверить, активен ли счётчик hnd

	bool isActive(THandle hnd);


	// остановить и удалить счётчик hnd из массива

	void Delete(THandle hnd);


	// отдает количество добавленных в список щёччиков

	bsize_t getCount() const;


	// сюда приходят тики от аппаратного таймера. Если список активен, запускается цикл
	// перебора счетчиков в списке.  На каждом тике каждый добавленный счётчик уменьшается на 1
	void Tick(void);
	};

основную идею я корявенько донес, а комментарии в тексте подскажут что там к чему (надеюсь)

обьект TTimerList - глобальный, define-ми в .срр файле определяется, сколько счетчиков для разных процессоров можно создать, для 328-й меги это 10, для 2560 - 16, а для Атмега8 и 168 - максимум 8 штук. 

Использование:  скопировать все файлы в свою директорию. В своем .ino файле написать 

	#include "TTimerList.h" 

	extern TTimerList TimerList;

и всё. Можно добавлять, удалять, останавливать и вновь запускать нужные счётчики.

рабочий Blink без delay(), millis() и loop() выглядит так


	#include "TTimerList.h"

	extern TTimerList TimerList;

	THandle hGreenLed;   // Handle добавляемого счетчика для его последующей идентификации

	uint8_t greenLedPin = 13;

	void tmrGreenLed(void){
	  static bool ledState=true;          	// состояние светодиода
	  digitalWrite(greenLedPin, ledState);	// вывести состояние в светодиод
	  ledState = !ledState;			// инвертировать состояние
	}

	void setup(){
	  pinMode(greenLedPin, OUTPUT);
	  hGreenLed = TimerList.Add(1000,tmrGreenLed); // функция tmrGreenLed будет вызываться раз в 1000 мс.
	}

	void loop(){}


а если вдруг нам надо, чтобы светодиод мигал неравномерно, например 200 мс горел, а 4800 мс не горел, то просто на лету меняем интервалы отсчета. 


	void tmrGreenLed(void){
	   static bool ledState=true;          	// состояние светодиода
	   digitalWrite(greenLedPin, ledState);	// вывести состояние в светодиод

	   TimerList.setNewInterval(hgreenLed, ledState ? 200 : 4800); // 

	   ledState = !ledState;			// инвертировать состояние
	}

пробуем, спрашиваем, не надоедаем.

Я НЕ ЗАНИМАЮСЬ встраиванием таймеров в ваш код, не надо присылать мне свои простыни для "анализа" как туда прикрутить это.  Алгоритм работы ВАШЕЙ программы известен только ВАМ, я в нем разбираца не буду.  Сможете прикрутить туда таймеры, буду рад, не сможете - значит еще не пришло время, пользуйтесь другими решениями.  И материть меня за это по электропочте не надо, мы с котом расстраиваемся, плачем, сильнапьём. 

Cвязаться со мной по вопросам можно по электропочте dap68@mail.ru
