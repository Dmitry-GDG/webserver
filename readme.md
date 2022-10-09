```
struct pollfd {
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};
```

https://ru.manpages.org/poll/2

https://habr.com/ru/company/infopulse/blog/415259/

Правила оформления:
1. Каждый день ДО начала правки кода делаем
```
git pull
```
2. Каждый раз ПЕРЕД уходом на перерыв или домой
- заполняем описанием нововведений в программе этот файл,
- делаем:
```
git add *
git commit -m "bla-bla-bla"
git push
```
3. При правке чужого кода 
- сначала комментим фрагмент, который собираемся исправить,
- затем пишем свой фрагмент.

4. При вводе классов срр:
- название парных файлов cpp и hpp начинаем с заглавной буквы
- после объявления переменной через двойной слэш (//) пишем пояснение, для чего она нужна
- имена всех приватных членов класса и приватных функций класса начинаем с нижнего подчеркивания ("_name")

5. Все необходимые системные хэдеры включаем в main.hpp

6. Все необходимые самописные хэдеры включаем в заголовочный файл (хэдер) класса, в него же включаем main.hpp

7. В cpp файл класса включаем хэдер класса

8. Все самописные cpp и hpp файлы кадаем в папку src (без подпапок).

9. Для конфигов, ресурсов и тд делаем подпапки

10. Все дефайны делаем в одном месте: main.hpp

11. Все новые файлы срр необходимо включать в Makefile в раздел "MAIN =" без расширений

12. Требование к файлу конфига серверов: 
	- Локации и сервер заканчиваются строкой с закрывающейся фигурной скобкой (без других символов)
	- методы перечисляются через пробел и/или новой записью (нельзя через /)
	- cgi не нумеруются (нельзя cgi2 kjk kjk)
	- каждая строчка оканчивается точкой с запятой и/или переводом строки
	- новый параметр - с новой строки (нельзя в одной строке несколько разных параметров, они будут обработаны, как аргументы первого параметра или проигнорированы (если у первого параметра ожидается только один аргумент))
	- Не понял, для чего у Аси в конфиге параметр client_max_body_size
	- Не понял, для чего у Аси в конфиге параметр cgi_dir

### Надо сделать:
- Я пока не нашёл образец файла конфига NGINX со всеми возможными полями. Необходимо разобраться с файлом конфига: он должен быть, как у NGINX. Необходимо представить наш файл конфига (поля и имена переменных) в соответствии с файлом NGINX
- Не понял, для чего у Аси в конфиге параметр client_max_body_size
- Не понял, для чего у Аси в конфиге параметр cgi_dir
- что такое cgi http://www.codenet.ru/webmast/cgi/cgidev.php
- возможно, надо перевести поля в uppercase перед сравнением Content-Type и Content-Length (https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case)

### Пометки
- парсится uri - cu-29
- добавление файла (папки с файлами) в бинарном виде в ответ (GT 45)

### Хронология изменений:
9/10 #Д#
- сделал splitStringStr;
- _parseInputBodyStr(connection);
- printVectorPair;
- необх доделать GET 404 работа с картинками (SR_GET-39). Клиент постоянно ссылается на первоначальный адрес, поэтому не подгружаются картинки 404. Возможно, редирект первоначального адреса на адрес, найденный при поиске 404?


8/10 #Д#
- испраил парсинг вх данных: теперь в inputStrBody записывается весь остаток, после извлечения оттуда inputStrHead
- в соответствие с этим изменил печать в терминал и лог-файл
- временно пофикшен DDELIMETER in connectionAnswer SR-345
- переделаны клиенты client_multipart и client_urlencoded для тестов
- новое поле std::vector<std::pair<std::string, std::string> >	bodyFieldsVec; 
- необх: _parseInputBodyStr(connection) SR_POST-93;
- необх: splitStringStr  u-94


7/10 #Д#
- сделал клиента для посылки запроса на сервер и получения ответа от него (в папке testClient)
- поменял структуру connection.inputData.headerFields на вектор пар (могли быть повторения, map не годилась)
- необх разобраться с multipart/form-data и multipart/mixed
- не правильно парсятся вх данные при нескольких DDELIMETERs SR-441

6/10 #Д#
- изменил RequestProcessingSteps
- _findConnectionLenBody;
- полностью изменил алгоритм парсинга входного буфера (_readSd), сделал заполнение отдельно header и body, проверку размера body на соответствие Content-length
- добавил закрытие sd по условию Connection:  Close (SR-_mainLoop()-270)
- добавил декодирование адреса в запросе;
- _extractLocalAddress (извлечение локального адреса из адреса в запросе) - done
- addressParamsStr (параметры адресной строки запроса POST)
- переделал печать data from sd (с учетом возможных параметров addressParamsStr)
- необх доделать SR_POST-29 (подумать, что делать с параметрами addressParamsStr. Возможно, придется сохр новый файл или передать данные в имеющийся файл)


5/10 #Д#
- переделал печать запросов и ответов в терминал и в логфайл: теперь это выводится в обычном режиме (не в дебаге) и через табы
- научился слать запрос POST через nc (head="POST localhost:4242 HTTP/1.1\r\nHost: localhost\r\nAuthorization: Basic myname:passwordinbs64\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: 10\r\nConnection: Close\r\n\r\nstatus=mymessage\r\n\r\n" ; echo $head | nc localhost 4242)
- разобрался, почему POST печатает только первую строку, и пофиксил
- необх изменить логику поиска конца строки запроса (сейчас ищется DDELIMETER, надо длину сверять???) SR-401
- необх выделить из запроса inputStrBody

3/10 #Д#
- доделал GET (вроде не сегается)
- сделал на Get, POST и DELETE отдельные файлы
- непонятно, почему создаётся куча новых sd при запросах браузеров. А старые sd не удаляются
- начал работать над POST. Необх понять, в каком виде получаем адрес и проверить, нет ли такого файла уже

2/10 #Д#
- ввёл команду exec для выполнения команд терминала (в частности, для поиска расширения файла)
- сделал логи, сколько байтов файла отправлены успешно
- сделал отправку файлов на сервер 
- сделал полный список MIME Types (_contentTypesInit())
- Почему-то, иногда сегается и вылетает (замечено на Safari)
- надо начать разбираться с Post

1/10 #Д#
- получилось отправить на запрос get html файл и открыть его в браузере
- изменил парсинг конфигов серверов, убрав слэши в начале и в конце путей root, upload, path, cgi, index
- GET работает, но необходимо доделать:
- необх разбить строку, отправляемую к браузеру, на маленькие кусочки и отправлять постепенно маленькими кусочками (SR 504 и 592)
- необх сделать логи, что файлы отправлены успешно
- необх подумать о if (!_addFileToAnswer) sr 363

30/9 #Д#
- Поделил запрос на заголовок и тело ( через DDELIMETER ) - parseInputData
- переделал parseInputData в часть класса ServerRouter
- ввёл connection->responseData.connectionAnswer
- необходимо продолжить добавление файла (папки с файлами) в бинарном виде в ответ (SR line 473 = GT 45)

29/9 #Д#
- добавил в структуру t_response поле метод
- переименовал и упорядочил структуры данных во всех файлах
- переименовал в соответствии с сабжектом название исполняемого файла webserv
- изменил функцию connectionClear
- работаю над анализом вх данных: необх расставить RequestProcessingStep и следить за ними на всех этапах (ас-56)

28/9 #Д#
- перенёс в connection inputstr в inputdata
- ввёл в connection структуру t_response и перенёс в неё responseStatus
- ввёл в структуре connection: lastActivityTime и реализовал закрытие соединения при бездействии по таймауту (у Сафари - 3 мин)
- работаю над ServerRouter line 244

- необх откорректировать парсинг Инпут Дата на предмет окончания его делиметром (если не оканчивается -> сохранить часть, сделать метку, что не окончен, и принять остаток) - (sr 216, 414) - DONE?
- необх доделать ответ (ServerRouter line 255 & 291; ws-117; h-288)
- небх подправить конфиги под имеющиеся файлы в папке resourses
- необх разобраться с wsm1

27/9 #Д#
- ввёл в connection responseStatusCode;
- необх откорректировать парсинг Инпут Дата на предмет окончания его делиметром (если не оканчивается -> сохранить часть, сделать метку, что не окончен, и принять остаток) - (sr 216, 414) - DONE?
- необх доделать ответ (ServerRouter line 255 & 291; ws-117; h-288)
- небх подправить конфиги под имеющиеся файлы в папке resourses
- необх разобраться с wsm1

26/9 #Д#
- изменил инициализацию методов (теперь они инициализируются в ServerRouter)
- переименовал в ServerRouter и structs: methods в allowedMethods
- ввел структуры Response status codes и Content types
- с вебстраницами поработал
- необх доделать ответ (ServerRouter line 255 & 291; ws-117; h-288)

25/9 #Д#
- исправил парсинг запросов от браузеров
- подправил принты дебагов
- response Status Codes - done
- необх готовить ответ (ServerRouter line 216 & 250; ws-117; h-14; rp-109)

24/9 #Д#
- подкорректировал сообщения в логфайле
- начал переделывать обработку вх данных
- необходимо продолжить разбираться с GET запросами от Safari

23/9 #Д#
- новые принты в терминале и логфайле
- реализовал неблокирующиеся сокеты fcntl(sd, F_SETFL, O_NONBLOCK);
- изменил парсинг InputData

22/9 #Д#
- ввёл логфайл
- ctrl_c_handler

21/9 #Д#
- переделаны структуры данных
- парсер iputData в прпоцессе;

20/9 #Д#
- обработка запросов недоделана
- продолжил mainLoop
- начал наполнять полями структуру Connection
- начал парсер iputData;

19/9 #Д#
- начал обрабатывать запросы (недоделано)
- начал mainLoop

18/9 #Д#
- ввёл DEBUGMODE
- в соответсвии с этим переделал все тестовые принты
- подкорректировал структуры входных данных (и принты к ним)
- начал менять алгоритм запуска сервера
- сервер запускается, надо обработать запросы

17/9 #Д#
- серверы запускаются
- входящие запросы принимаются (например, nc localhost 8081. Или в браузере localhost:8081)
- начал работу над обработкой входящих запросов

16/9 #Д#
- исправил парсер

15/9 #Руслан#

15/9 #Д#
- добавил несколько функций для печати векторов
- сделал корректировку парсера конфига, чтоб учесть фигурные скобки (после закрытия скобки - фрагмент завершен)

14/9 #Руслан#

14/9 #Д#
- Вынес парсер входной структуры из класса ServerRouter в отдельный, независящий от класса файл parserServerConfigFile.cpp
- Начал реализовывать функцию start
- ввёл в структуру ServerRouter переменную std::vector<Server> и создал класс Server
- полностью переделал парсер конфига
- взял у Аси её конфиг и пролверил парсер на нём.

13/9 #Руслан#

13/9 #Д#
- Сделал функцию печать конфигов всех серверов printAllServersConfig;
- в классе ServerRouter заменил переменную "std::vector<WebServer> _servers" на "std::vector<t_config> _configs". Соответственно, убрал полностью класс WebServer;
- доделал парсер конфига.

12/9 #Руслан#

12/9 #Д#
- разбиение конфига по слову server;
- в конфиге: проверка и удаление строк или частей строк с комментами;
- в конфиге: проверка и удаление лишних White spaces (появился файл utilites.cpp);
- в конфиге: проверка и удаление пробелов в начале каждой строки;
- в конфиге: проверка и удаление пустых строк;
- в конфиге: проверка и пробелов и ";" в конце каждой строки;
- в итоге получил структуру входного файла без лишних знаков, печать стр.269-271 ServerRouter.cpp;
- начал набивать конфиг сервера;
- начал делать функцию печать конфига сервера utilites.cpp -> printServerConfig
- можно посмотреть печать конфига, раскомментив стр.188 ServerRouter.cpp

11/9 #Руслан#
- функция проверки скобок
- функция разбиения на серверы
- функция, которая сервер разбивает на структуру не доделан, так как много времени возился с функциями для обрезки по пробелам и табам и точкой с запятой. Можно просплитить по ключевым словам строки для структуры и получить необходимые данные.

11/9 #Д#:
- включил в readme.md правила
- изменил main.cpp
- Makefile
- перенёс структуру сервера в файл WebServer.cpp
- перенёс парсер из отдельного класса в WebServer.cpp как функцию класса (файлы Parser.hpp и Parser.cpp не нужны)
- сделал две структуры: t_location и t_config;
- создал класс ServerRouter;

10/9 #Руслан#: 
- составил небольшой конфиг, но скорее всего он неполный, так как не до коца разобрался с ним
- начал писать парсер
- в каком виде писать структуру с данными из парсинга завтра обговорим, потом буду дописывать
- добавил свой html файл для пробных будущих запусков
- добавил библиотеки в хэдер main
