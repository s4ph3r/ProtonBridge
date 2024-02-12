//Сервис предназначен для реализации Bluetooth­профилей дозиметров и детекторов радиоактивности, в которых измерение дозы и
//мощности дозы осуществляется путём подсчёта числа частиц, попавших в детектор. Датчики могут быть как на основе счётчиков 
//Гейгера, так и на основе сцинтилляционных детекторов с ФЭУ или лавинными фотодиодами. Динамические диапазоны для величин 
//сервиса выбраны в расчёте на охват всех возможных случаев. Измеряемые величины и флаги состояния передаются непрерывно в
//единственной характеристике, не требующей запроса (тип notification, наиболее энергоэффективный способ). При этом, согласно 
//спецификации Bluetooth Low Energy, нотификация должна быть разрешена записью в сопутствующий атрибут конфигурации. 
//Все остальные нижеописанные величины и соответствующие им характеристики имеют тип Read или Read/Write. Они предназначены 
//не для регулярного обмена, но для сравнительно редких действий по запросу пользователя или при автоматической реакции
//программного обеспечения на события превышений. По типу данных BT­профиль структурирован несколькими группами характеристик:
//1. Измерительные​характеристики.
//2. Характеристики с параметрами ​ настройки прибора​
//3. Сервисные характеристики или ​ характеристики калибровки​ прибора аппаратных компонент прибора.
//
//Измерительные характеристики
//Устройство поддерживающее Service BLE предоставляет две характеристики для получения измеренных величин и обновляемых флагов
//состояния устройства:
//1. Основная нотификационная измерительная характеристика (Notification characteristic) для обновления вычисляемых величин по подписке.
//Характеристика обновляется устройством каждые 2 секунды.
//2. Дополнительная измерительная​​ характеристика​для чтения (Read characteristic) исходных данных измерений таких как количество частиц с
//начала измерений и количество частиц за определенный промежуток времени.
//
//Настроечные характеристики
//Профиль устройства позволяет центральному устройству читать и изменять(Read/Write characteristic) некоторые параметры реакции на
//измеряемые величины. К таким параметрам относятся:
//1. Порог срабатывания сигнализации о ​ превышении мощности дозы
//2. Порог срабатывания сигнализации о​превышении накопленной дозы​ .
//3. Минимальное ​ время измерения (детектирования)​превышения порога мощности дозы.
//4. Команда управления виброзвуковой сигнализацией
//Для устройства AtomTag предусмотрено хранение 3­х наборов порогов. Индикация превышения порогов может быть звуковой, виброзвуковой или
//только вибро. Устройство может быть также переведено в бесшумный режим полностью для всех порогов или по выбору.
//
//Калибровочные характеристики
//Пользовательским ПО предусмотрено только чтение параметров калибровочных характеристик. Управление калибровочными или сервисными
//характеристиками не поддерживается в пользовательской версии центрального устройства работающего с Service BLE. Этот набор
//характеристик необходим для первоначальной настройки устройства или выполнении сервисных функций в процессе эксплуатации устройства и
//производится только с помощью специализированного ПО квалифицированными пользователями, а так же для вычислений некоторых
//измерительных параметров при разрыве соединения с устройством. 
//В профиле учитываются следующие основные ​ калибровочные параметры​ :
//1. Чувствительность счётчика, задаваемая в имп/мкР (float). Выбрана величина именно такой размерности по той причине, что в ней даётся
//паспортная чувствительность большинства известных датчиков. Кроме того, в этих единицах чувствительность датчика представляет собой легко
//запоминающуюся величину. Например, для СБМ­20/СТС­5 это 78имп/мкР, для СБМ­21 – 9.5 имп/мкР, для Бета­1 – 120имп/мкР.
//2. Собственный фон счётчика, [имп/мин] (float).
//3. Время расчёта текущего значения мощности дозы в секундах (unsigned short). Чувствительность счётчика по умолчанию – как у СБМ­20.
//Собственный фон нулевой, т.е. не учитывается. Время расчёта текущей мощности дозы – 34сек. В такой конфигурации основанный на СБМ­20 прибор
//идентичен большинству бытовых дозиметров. Данные доступны на чтение в обычном режиме, запись калибровочных данных требует пароля для
//исключения случайной перезаписи калибровки.
//Дополнительные калибровочные данные профиля:
//1. Калибровочный коэффициент для АЦП измерителя ЭДС батареи. Представляет собой вещественную величину (float), на которую следует
//умножить целое число АЦП, чтобы получить напряжение батареи в милливольтах.
//2. Целое 16­битное ​ число АЦП термодатчика​ , соответствующее комнатной температуре 23˚С.
//3. Калибровочный коэффициент АЦП термодатчика, определяющий наклон шкалы. Представляет собой вещественное (float) обратное число квантов
//АЦП, соответствующее изменению температуры на один градус Цельсия. Значение по умолчанию – 1/4.5 – соответствует документированной
//характеристике чипсета сс254х. Настройки работы дозиметра включают в себя установки трёх порогов мощности дозы и дозы, при превышении которых
//задаётся поведение дозиметра, а именно длительность звука, тон и длительность вибрации. Также настройки дозиметра включают в себя
//характеристику настройки отклика дозиметра на зарегистрированную частицу. Задаётся длительность звукового сигнала в интервале 0..1000мс, а также
//длительность вибрации 0..1000мс.
//Для быстрого выбора типовых конфигураций и управления дозиметром отведена отдельная характеристика, имеющая тип read/write. В эту
//характеристику записываются 8­ битные команды­макросы с возможными 16­битными аргументами (до трёх аргументов), целью которых является
//сокращение числа циклов обмена, например, переход устройства в «тихий» режим, в котором он не издаёт звуков и вибраций.

//Измерительные характеристики
//Измерительные характеристики ­ основной набор данных с которым работает клиентское приложение. Характеристики разбиты на две группы 
//по типу доставки потока данных: активная Notification основная, и пассивная Read дополнительная.

//Если заработает снести нахрен
#define PROTON_SERVICE_UUID 0x1D, 0xF7, 0x54, 0xAF, 0xB9, 0x14, 0xED, 0x81,0x04, 0x43, 0x1A, 0x7A, 0x7E, 0x76, 0xBC, 0x70

//////////////////////////////////////////////////////////////////////////////////////////////////
//Основная измерительная характеристика 70BC767E­7A1A­4304­81ED­14B9AF54F7BD//////////////////////
//Attribute UUID 2092
//Значение 001
//Предназначена для постоянной передачи устройством измеряемых величин и флагов состояния.
#define PROTON_MM_UUID 0xBD, 0xF7, 0x54, 0xAF, 0xB9, 0x14, 0xED, 0x81,0x04, 0x43, 0x1A, 0x7A, 0x7E, 0x76, 0xBC, 0x70
//Структура (13 байт, Little Endian)
#define PMM_STATE_ATTR 0    //Флаги состояния \\uint8 1байт
#define PMM_CUM_DOSE_ATTR 1 //Накопленная доза в ​ mSv \\float 4байта
#define PMM_POW_DOSE_ATTR 5 //Мощность дозы в ​ µSv/h​, рассчитывается в течении N секунд. \\float 4байта
						    //Количество N задается в основной калибровочной характеристике, по ум. N=34
#define PMM_IMP_COUNT_ATTR 9//Число импульсов за последние 2 секунды \\uint16 2байта
#define PMM_BATTERY_ATTR 11 //Процент заряда батареи (0..100) в формате signed char. Внутри AtomTag \\uint8 1байт
							//рассчитывается из ЭДС литий­ионного аккумулятора обратной 
							//параболической интерполяцией в предположении: 0% ­ 3.5В, 10% ­ 3.6В, 100% ­ 4.0
#define PMM_TEMP_ATTR 12 //Температура в формате signed char, градусы Цельсия (­128..127), ºC \\uint8 1байт
//
//Флаги (Индекс бита)
//0 - Произошло превышение одного из установленных ​ порогов дозы или мощности дозы. 
//1 - Сбрасываемый флаг превышения ​ порога мощности дозы
//2 - За последние 2 сек произошло изменение скорости счёта более чем на Х стандартных отклонений
//    (Х=5 по умолчанию). Значение мощности дозы сброшено и рассчитывается заново.
//    Взводится и удерживается до конца нового замера мощности дозы (по умолчанию 34 сек). 
//3 - Зарезервирован
//4 - Индикатор превышения тока через детектор. если ток через счётчик Гейгера аномально большой.
//    Может сигнализировать о выходе счётчика из строя, а также о значительной перегрузке ионизирующими
//    частицами, при которой работа в режиме счётчика невозможна. Счёт при этом может падать или вовсе
//    отсутствовать. Следует информировать пользователя, что в детекторе непрерывная ионизация или непогасающий разряд.
//5 - Индикатор перегрузки детектора счётными импульсами если суммарная длительность мёртвого времени превышает
//    50%, достигнут технический предел счётчика Гейгера, при дальнейшем повышении мощности дозы он перестанет
//    работать в режиме счётчика. Поправка на мёртвое время (поправочный коэффициент) превышает число 2.
//6 - Индикатор подключения зарядного устройства. если подключено зарядное устройство
//7 - Индикатор аварийного выключения. Был выключен аппаратно в связи с глубоким разрядом
//    батареи, либо в результате зависания

//////////////////////////////////////////////////////////////////////////////////////////////////
//Дополнительная измерительная характеристика 8E26EDC8­A1E9­4C06­9BD0­97B97E7B3FB9////////////////
//Предназначена для получения исходных данных о числе попавших в детектор частиц, без пересчёта 
//с использованием калибровки. Необходима, например, для перерасчёта дозы, накопленной в промежутке
//обрыва связи, когда точности ​ float ​ значения дозы недостаточно.
#define PROTON_AM_UUID 0xB9, 0x3F, 0x7B, 0x7E, 0xB9, 0x97, 0xD0, 0x9B,0x06, 0x4C, 0xE9, 0xA1, 0xC8, 0xED, 0x26, 0x8E
//Структура (20 байт, Little Endian)
#define PAM_TOTAL_IMP_COUNT_ATTR 0      //Общее число импульсов \\uint64 8байт
#define PAM_IMP_COUNT_SPEED_ATTR 8      //Число импульсов коррекции на мёртвое время датчика. \\uint32 4байта
									    //Для расчёта дозовых величин на него нужно увеличить
									    //общее число импульсов
#define PAM_TOTAL_IMP_COUNT_SEC_ATTR 12 //Общее число импульсов за N секунд. Количество N \\uint32 4байта
										//задается в основной калибровочной характеристике
										//По умолчанию:N=34
#define PAM_DOSE_SEC_ATTR 16 			//Время измерения дозы в секундах. Не более 136 лет \\uint32 4байта

//////////////////////////////////////////////////////////////////////////////////////////////////
//Настроечные характеристики////////////////////////////////////////////////////////////////////// 
//Настроечные характеристики делятся на две группы: три характеристики установки 
//виброзвуковой индикации превышения порогов и характеристики управления устройством.
//
//Характеристики настроек индикации превышения порогов////////////////////////////////////////////
//Характеристики предназначена для установки порогов сработки на значения дозы и мощности 
//дозы, для установки времени расчёта мощности дозы (меньшее время увеличивает скорость 
//сработки за счёт точности), для установки типа и длительности звукового сигнала, а
//также длительности вибросигнала при превышениях.
//Установка порогов сохраняется в ОЗУ при записи характеристики и действует как при работе 
//прибора в автономном режиме, в несвязанном с другим устройством состоянии, так и при 
//подключенном устройстве. При записи команды сохранения настроек с открытым пользовательским 
//паролем сохраняется во флеш.
//
//Порог срабатывания 0 3F71E820­1D98­46D4­8ED6­324C8428868C
//Низкоприоритетный порог срабатывания
#define PROTON_P0_UUID 0x8C, 0x86, 0x28, 0x84, 0x4C, 0x32, 0xD6, 0x8E,0xD4, 0x46, 0x98, 0x1D, 0x20, 0xE8, 0x71, 0x3F
//
//Порог срабатывания 1 2E95D467­4DB7­4D7F­9D82­4CD5C102FA05
//Виброзвуковая сигнализация имеет приоритет ​ над первым порогом​ , в случае превышения 
//будет срабатывать в первую очередь
#define PROTON_P1_UUID 0x05, 0xFA, 0x02, 0xC1, 0xD5, 0x4C, 0x82, 0x9D,0x7F, 0x4D, 0xB7, 0x4D, 0x67, 0xD4, 0x95, 0x2E
//
//Порог срабатывания 2 F8DE242F­8D84­4C12­9A2F­9C64A31CA7CA
//Виброзвуковая сигнализация имеет приоритет ​ над первым и вторым порогом​ , в случае 
//превышения будет срабатывать в первую очередь.
#define PROTON_P2_UUID 0xCA, 0xA7, 0x1C, 0xA3, 0x64, 0x9C, 0x2F, 0x9A,0x12, 0x4C, 0x84, 0x8D, 0x2F, 0x24, 0xDE, 0xF8
//
//Структура характеристик порогов (10 байт, Little Endian)
#define PP_CUM_DOSE_ATTR 0      //Порог включения сигнала о превышении накопленной дозы, mSv \\float 4байта
								//0:10mSv
							    //1:40mSv
								//2:200mSv
#define PP_POW_DOSE_ATTR 4      //Порог срабатывания сигнала о превышении мощности дозы, µSv/h \\float 4байта
								//0:0.5µSv/h
								//1:2.0µSv/h
								//2:10.µSv/h
#define PP_TIME_DOSE_ATTR 8     //Время детектирования превышения мощности дозы, seconds (34 seconds) \\uint8 1байт
#define PP_VIBRO_SOUND_ATTR 9   //Байт управления виброзвуковой (см. ниже) сигнализацией. \\uint8 1байт
								//Байт состоит из двух полубайтов, старший из которых отвечает за
								//поведение на превышение мощности дозы, младший ­ на превышение дозы.
								//для порогов 1 и 2 ­ 0x11, для порога 3 ­ 0x22
//Байт управления сигнализацией, формат 4­битной настройки
//0x1 - звуковая сигнализация по умолчанию ­ двойной короткий тоновый сигнал при превышении 
//		мощности дозы каждые 2 секунды, сигнал кодом Морзе “D1”,“D2” или “D3” (для порогов дозы 
//		1,2 и 3 соответственно) раз в минуту при превышении дозы, вибросигнал отключен.
//0x2 - то же, что и 0х01, но включен вибросигнал длительностью 1 секунда, который 
//		сопровождает звуковой сигнал.
//0x3...0xf - зарезервированы
//Старшие 4 бита байта управления сигнализацией отвечают за управление реакцией на превышение 
//мощности дозы, младшие 4 бита ­ на превышение дозы. Например, 0х12 будет означать отключенную 
//вибросигнализацию на превышение порога мощности дозы, но включенную на превышение дозы.

/////////////////////////////////////////////////////////////////////////////////////////////////
//Характеристика управления устройством EA50CFCD­AC4A­4A48­BF0E­879E548AE157
//Характеристика управления устройством предоставляет интерфейс для установки настроек работы устройства 
//с текущими величинами измерений и порогов, отклика на детектирование частиц и пр. Состоит из набора
//параметризованных команд. На чтение возвращает текущие настройки виброзвуковой сигнализации.
#define PROTON_CD_UUID 0x57, 0xE1, 0x8A, 0x54, 0x9E, 0x87, 0x0E, 0xBF,0x48, 0x4A, 0x4A, 0xAC, 0xCD, 0xCF, 0x50, 0xEA
//Структура (8 байт, Little Endian)
#define PCD_COM_ATTR 0      	//Команда, на чтение всегда 0x00 \\uint8 1байт
#define PCD_SOUND_DUR_ATTR 1  	//Параметр #1 \\uint16 2байта
								//На чтение: длительность звука в msec
								//На запись: первый параметр команды: длительность звука в msec, пароль, либо не имеет значения,
								//если команда не предполагает этого параметра.
#define PCD_SOUND_TON_ATTR 3  	//Параметр #2 \\uint16 2байта
								//На чтение: тон звука в Hz
								//На запись: тон звука в Hz, либо 0x0 если команда не предполагает этого параметра.
#define PCD_VIBRO_DUR_ATTR 5  	//Параметр #3 \\uint16 2байта
								//На чтение: длительность вибрации в msec
								//На запись: длительность виброотклика на импульс в msec, либо не имеет значения, 
								//если команда не предполагает этого параметра.
#define PCD_VIBRO_CON_ATTR 7  	//На чтение: флаги установок виброзвуковой сигнализации \\uint8 1байт
								//бит 0: 1 если отключена только виброзвуковая реакция на порог дозы 1
								//бит 1: 1 если отключена только виброзвуковая реакция на порог дозы 2
								//бит 2: 1 если отключена только виброзвуковая реакция на порог дозы 3
								//бит 3: 1 если отключена только виброзвуковая реакция на порог мощности дозы 1
								//бит 4: 1 если отключена только виброзвуковая реакция на порог мощности дозы 2
								//бит 5: 1 если отключена только виброзвуковая реакция на порог мощности дозы 3
								//бит 6: 1 если отключена виброзвуковая реакция на импульсы
								//бит 7: 1 если вся виброзвуковая сигнализация отключена, виброзвуковой отклик 
								//на импульсы при этом может быть разрешён и действует отдельно.
//Код команды
//0x01 - Установка параметров виброзвукового отклика на импульс. Параметр #1 ­ длительность звука, ms (первый ­ 0..1000 ms)
//		 Параметр #2 ­ тон звука в Hz (второй ­ 490..10000 Hz)
//		 Параметр #3 ­ длительность вибро в ms (третий ­ 0..1000 ms)
//0x02 - Установка ​ типовой конфигурации (preset)виброзвукового отклика на импульс​с длительностью звука ​ 2мс, 1кГц без вибрации (0x0)
//0x03 - Установка типовой конфигурации виброзвукового отклика на импульс с длительностью звука ​ 20мс, 1кГц без вибрации (0x0)
//0x04 - Установка ​ типовой конфигурации (preset)виброзвукового отклика на импульс​с длительностью звука ​ 1мс,4кГц без вибрации​
//		 (установка по умолчанию, действует в свежепрошитом приборе) (0x0)
//0x05 - Установка типовой конфигурации виброзвукового отклика на импульс с длительностью звука ​ 20мс, 4кГц без вибрации (0x0)
//0x06 - Установка типовой конфигурации виброзвукового отклика на импульс с длительностью звука ​ 1мс, 4кГц и вибрацией 10мс. (0x0)
//0x07 - Установка типовой конфигурации виброзвукового отклика на импульс с длительностью звука ​20мс, 4кГц и вибрацией 100мс. (0x0)
//0x08 - Отключение виброзвуковой реакции на импульсы (запись нулей в величины длительностей звукового и вибрационного откликов).
//		 Виброзвуковая реакция на пороги сохраняется. Команда оптимизирует количество сеансов связи с одним устройством для типовых установок.
//0x0A - Активация «тихого»​режима. Отключает любую виброзвуковую сигнализацию, включая реакцию на пороги. Во флеш не сохраняется.
//		 Прибор не издаёт никаких заметных человеку сигналов.
//0x10 - Де­активация «тихого» режима. Возвращает виброзвуковую сигнализацию. Возвращаются в состояние настроек комманд управления
//		 виброзвуковой сигнализации.
//0x11 - Снимает сбрасываемые флаги о превышениях дозы, интенсивности и некорректного выключения. Подтверждение приёма флагов состояния.
//0x12 - Возвращает настройки порогов к значениям по умолчанию. 0xACCE “Открытый” пароль. Должен быть строкой вида {0xAC, 0xCE },
//		 т.е. если пишем как параметр uint16, то он “своппится”, 0xCEAC.
//0x13 - Отключает виброзвуковую реакцию на пороги, импульсы и прочие события.
//		 0x0000 ­ импульсы
//		 0x0001 ­ 1­й порог дозы
//		 0x0002 ­ 2­й порог дозы
//		 0x0003 ­ 3­й порог дозы
//		 0x0004 ­ 1­й порог мощности дозы
//		 0x0005 ­ 2­й порог мощности дозы
//		 0x0006 ­ 3­й порог мощности дозы
//		 0x0007 ­ все пороги
//		 0x0008 ­ все пороги, кроме первого (разрешает если выключен)
//		 0x0009 ­ все пороги, кроме второго (разрешает если выключен)
//		 0x000A ­ все пороги, кроме третьего(разрешает если выключен)
//		 0x0010 ­ все звуки (пороги и служебные сигналы), включая реакцию на импульсы. Реакция на импульсы идёт отдельным флагом и
//				  её можно включить при установленном режиме отключения всех звуков. Другие звуки включить нельзя.
//		 0x0011 ­ все звуки, кроме звуков реакции на импульсы.
//		 0x0012 ­ звук при переподключении прибора
//		 0x0013 ­ звук­сигнализатор разряда батареи ниже 10%
//		 Отключение виброзвуковой сигнализации всех порогов делает невозможным индивидуальное управление порогами.
//0x14 - Возвращает виброзвуковую виброзвуковую реакцию.
//		 0x0000 ­ импульсы
//		 0x0001 ­ 1­й порог дозы
//		 0x0002 ­ 2­й порог дозы
//		 0x0003 ­ 3­й порог дозы
//		 0x0004 ­ 1­й порог мощности дозы
//		 0x0005 ­ 2­й порог мощности дозы
//		 0x0006 ­ 3­й порог мощности дозы
//		 0x0007 ­ все пороги
//		 0x0010 ­ все звуки, а также включает звук на импульсы, если он был выключен
//		 0x0011 ­ все звуки, но не затрагивает звук на импульсы и соответствующий флаг
//		 0x0012 ­ звук при переподключении прибора
//		 0x0013 ­ звук­сигнализатор разряда батареи ниже 10%
//0xCA - Сохранение действующих настроек во флеш­память.​Без сохранения настройки действуют до первой перезагрузки устройства.
//		 При сохранении настроек: Параметр #1: 0хACCE, второй и третий не имеют значения
//		 Пароль должен быть строкой вида: {0xAC, 0xCE}, т.е. если пишем как параметры uint16, то байты попарно “своппятся”, 0xCEAC.
//0хAA - Проигрывает мелодию. Предназначено для поиска прибора, если он потерян.
//		 Номер мелодии или звуковой последовательности. Рекомендуется использовать 0x00. Эта последовательность покрывает широкую
//		 полосу частот и эффективно использует резонанс встроенного звукоизлучателя. 
//		 Также доступны значения 0х01 и 0х02. Мелодии 0х80..0х8С ­ служебные.
//0xDE - Сбрасывает накопленное значение общего числа импульсов (дозы). Пароль 0xACCE
//0xFF - Выключает устройство, аппаратно отсоединяя элемент питания. Эквивалентно действию скрытой кнопки. 
//		 0xACCE Включить можно будет только нажатием скрытой кнопки либо подсоединением зарядногоустройства.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Калибровочные (сервисные) характеристики
//Основная калибровочная характеристика 57F7031F­03C1­4016­8749­BAABAA58612D
//Характеристика позволяет прочитать и управляет калибровочными значениями счетчика частиц.
#define PROTON_SC_UUID 0x2D, 0x61, 0x58, 0xAA, 0xAB, 0xBA, 0x49, 0x87,0x16, 0x40, 0xC1, 0x03, 0x1F, 0x03, 0xF7, 0x57
//Структура (18 байт, Little Endian)
#define PSC_SENS_IMP_ATTR 0  		//Чувствительность датчика, в импульсах на микроРентген, impulses/µR \\float 4байта
									//78.0(чувствительн ость счетчика СБМ­20)
#define PSC_BACK_COUNT_ATTR 4  		//Фоновый счёт датчика, в импульсах в секунду, impulses/sec - 1/15(СБМ­20) \\float 4байта
#define PSC_DEAD_TIME_ATTR 8  		//Мертвое время датчика, ms - 0.2​ (СБМ­20) \\float 4байта
#define PSC_DOSE_COUN_TIME_ATTR 12  //Время измерения мощности дозы, seconds. При записи нечётного будет округлено к меньшему \\uint16 2байта
									//чётному. 34, должно быть четным!
#define PSC_PSWD_WRITE_RAM_ATTR 14  //Пароль для записи калибровки в RAM. Write: ​0xACCE57FE Read: 0x0 \\uint32 4байта
//
//Характеристика служебных данных E2423A67­7541­4080­8B5A­59449454A873
//Характеристика предназначена для настройки прибора при его производстве. Позволяет читать некоторые калибровочные 
//данные и управлять ими. Также управляет сохранением калибровочных данных во флеш­память.
#define PROTON_SD_UUID 0x73, 0xA8, 0x54, 0x94, 0x44, 0x59, 0x5A, 0x8B,0x80, 0x40, 0x41, 0x75, 0x67, 0x3A, 0x42, 0xE2
//Общая структура (17 байт, Little Endian)
#define PSD_COM_R_ATTR 0  			//Команда, на чтение, всегда 0x00 \\uint8 1байт
#define PSD_COM_W_ATTR 1  			//На запись: параметры #1­#8 На чтение: массив значений длинной 8 байт. \\uint16[8] 2байта
//Характеристика предоставляет несколько режимов управления и получения данных в зависимости от параметра 
//команды переданной центральным устройством в режиме Write при последнем сеансе связи.
//В режиме записи​назначение параметров при записи зависит от команды.
//0xCA - Сохранение действующей калибровки
//		 Параметр #1: 0х57FE
//		 Парaметр #2: 0xACCE
//		 Парaметр #3: 0xD0CA
//		 Пароль записи калибровки ­ “закрытый”, не предназначен для пользовательской документации. 
//		 Должен быть строкой вида {0x57, 0xFE, 0xAC, ...}, т.е. если пишем как параметры uint16, то байты попарно “своппятся”, 0xFE57, ...
//0xCB - Калибровка сдвига и наклона шкалы термодатчика
//		 Калибровка сдвига: 
//		 Парaметр #1:0хACCE
//		 Парaметр #2: 0x0000
//		 Парaметр #3: текущая температура в формате signed short 
//		 Калибровка наклона шкалы:
//		 Парaметр #1: 0хACCE
//		 Парaметр #2: 0x0001
//		 Парaметр #3: текущая температура в формате signed short
//		 При калибровке наклона шкалы температура должна быть отлична от температуры калибровки сдвига шкалы как минимум на 5
//		 градусов Цельсия. В противном случае команда будет проигнорирована.
//0хСС - Устанавливает режим чтения сервисных данных
//		 Парaметр #1: 0хACCE,
//		 Парaметр #2: режим чтения сервисных данных (uint16). По умолчанию парaметр #2: 0x00
//В режиме чтения​с помощью характеристики можно получить:
//1. Калибровочные параметры чипсета прибора.
//2. Массив измерений последних 2­х секундных измерений счетчика, для восстановления значений при разрыве связи между центральным
//   устройством и прибором.
//Тип данных, который можно получить при чтении характеристики зависит от предварительно выставленного, в режиме работы 
//с характеристикой ​ Write 0xCC (см. таблицу выше)​ , соответствующей командой ​ режима чтения сервисных данных​­ по умолчанию 
//этот режим задан байтом ​ 0х00​
//
//Структура для выбранного режима чтения 0х01 (17 байт, Little Endian)
#define PSD_NULL_ATTR 0  			//Всегда ноль \\uint8 1байт
#define PSD_CAL_BAT_ATTR 1  		//Калибровочный коэффициент для перевода числа АЦП батареи в милливольты, mV. 1/38 \\float 4байта
#define PSD_CAL_TEMP_ATTR 5  		//Калибровочный коэффициент для перевода числа АЦП температуры в ºC. 2/9 \\float 4байта
#define PSD_CAL_TEMP_23_ATTR 9  	//Калибровочное значение кода АЦП температуры, соответствующее комнатной температуре. 23 \\uint16 2байта
#define PSD_TEMP_ATTR 11  			//Текущее значение АЦП температуры \\uint16 2байта
#define PSD_TEMP_CAL_ATTR 13  			//температура, при которой выполнена калибровка сдвига шкалы, градусы Цельсия 23 \\signed char 1байт
//Режим чтения сервисных данных:
//1. 0x00, 0x02​и любое другое значение выше ­ последние 8двухсекундных замеров числа импульсов. 
//	 Значение по умолчанию ­ 0x00​ . ​ Массив значение начинается от последнего измерения к более раннему.
//2. 0x01​­ сервисные данные температурной калибровки.

//Характеристика текстового описания BB6C9C06­C37D­49B0­94CA­83623622573B
//Характеристика позволяет установить краткое (не более 20 символов) текстовое имя прибора или его описание. 
//Никак не влияет на измерительную часть. Запись с верным паролем инициирует сохранение принятой строки во флеш.
#define PROTON_TD_UUID 0x3B, 0x57, 0x22, 0x36, 0x62, 0x83, 0xCA, 0x94,0xB0, 0x49, 0x7D, 0xC3, 0x06, 0x9C, 0x6C, 0xBB
//Структура (20 байт)
#define PTD_NAME_ATTR 0  			//Произвольная строка, которой пользователь может промаркировать прибор. \\char[20] 20байт
									//Допустимый набор символов ASCII, Latin. Последний символ обязательно должен быть ‘\0’. “AtomTag\0”


#define PROTON_SERVICE 0x00000001

typedef void (*protonControlCB_t)( uint8 paramID ) ;

typedef struct
{
	protonControlCB_t pfnProtonCB; // Вызывается когда какие-то аттрибуты изменились
} ProtonCBs_t;

extern bStatus_t Proton_AddService();
extern bStatus_t Proton_RegisterAppCBs( ProtonCBs_t *appCallbacks );
extern bStatus_t Proton_SetParameter( uint8 param, uint8 len, void *value );
extern bStatus_t Proton_GetParameter( uint8 param, void *value );
//Функции AddService и RegisterAppCBs служат для регистрации профиля в стеке и привязки функций обратного вызова программы к профилю.
//Функции SetParameter и GetParameter нужны для управления значениями характеристик профиля.