# Webserv
Этот проект посвящен написанию собственного HTTP-сервера.

Вы сможете протестировать его в реальном браузере.

HTTP — один из наиболее часто используемых протоколов в Интернете. Знание его секретов будет полезно, даже если вы не будете работать над веб-сайтом.

### Как тестировать
```
make && ./webserv
```

#### в браузере localhost:8080
#### метод GET
- набрать в адресной строке localhost:8080
	- потестировать все ссылки
- можнно в адресной строке набрать путь к файлу
	- если файл существует, то будет показан
	- если такого файла нет - будет показана страница 404
	- если такой страницы на сервере нет (например, 8081) - страница 404 будет нарисована сервером

#### метод POST
- перейти на сайте на соответствующую вкладку
- выбрать файл или два файла
- кликнуть отправить
- файл сохранится по адресу: 8080/downloads с соответствующим именем (если имя существует, то добавится индекс)

#### метод DELETE
- Чекать из ReqBin Google Chrome Extension https://chrome.google.com/webstore/detail/reqbin-http-client/gmmkjpcadciiokjpikmkkmapphbmdjok 
Поместить файл 1.txt в директорию 8080 и в директорию 8080/downloads/ нашего сервера, выбрать метод DELETE и в адресной строке набрать:
```
127.0.0.1:8080/1.txt // Status: 200 (Ok)
127.0.0.1:8080/1.txt // Status: 404 (Not Found)
127.0.0.1:8080/downloads/1.txt // Status: 405 (Method Not Allowed)
```
В поле RAW можно посмотреть текст запроса.
- Можно чекать из client_delete.cpp

#### CGI
- зайти на сайт 127.0.0.1:8080 и выбрать Python или Ruby CGI. Страница откроется скриптом на соответствующем языке

#### Cookies
- Лучше тестировать в Safari. Нажать кнопку "Go to page" и посмотреть на результат. После каждого нового посещения счетчик увеличивается. Можно стиреть куки, нажав на соответствующую кнопку
- также куки видны в терминале в хэдере запроса

#### Autoindex
- autoindex — то это штуковина, которая включает листинг директорий по http, средствами веб-сервера (конечно, если в директории нет настоящего index-файла).

#### Потестировать
- при отсутствии в папке ресурсов сайта страницы ошибки, такая страница нарисуется самим сервером
- знаки кирилицы (и не только) в адресной строке кодируются/декодируются

#### Что такое имя сайта
- Доменное имя — удобный способ для нас, людей, но Интернет организован таким образом, что компьютеры могут искать точное местоположение сервера по его IP-адресу, который представляет собой набор чисел, например 222.324.3.1 (IPv4). Браузер использует распознаватель DNS (Domain Name System), используя системный вызов gethostbyname POSIX для получения информации о хосте. На школьных маках этот файл находится в /etc/hosts и выглядит:
```
127.0.0.1       localhost
255.255.255.255 broadcasthost
::1             localhost
```
Изменить его можно только с правами администратора. Если это не дает никакой информации о домене, система отправляет запрос на DNS-сервер. Большинство людей используют DNS-сервер, предоставленный их интернет-провайдером. DNS-сервер может иметь IP-адрес домена в кэше. Если нет, он спросит корневой DNS-сервер.

### Правила оформления:
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
	- cgi состоят из двух параметров: тип и путь_к_директории_с_установленным;
	- каждая строчка оканчивается точкой с запятой и/или переводом строки
	- в параметре конфига limitClientBodySize и в дефайне # define LIMIT_CLIENT_BODY_SIZE определяется макс размер принимаемых картинок
	- новый параметр - с новой строки (нельзя в одной строке несколько разных параметров, они будут обработаны, как аргументы первого параметра или проигнорированы (если у первого параметра ожидается только один аргумент))

### Хронология изменений:
31/10 #Д#
- сделал CGI
- сделал COOKIES

29/10 #Д#
- закомментил ненужную переменную char * bodyBinar и все связанные с ней строки кода
- дополнил раздел "Как тестировать" этого файла

28/10 #Д#
- Пофиксил длину limitClientBodySize по умолчанию (даже если LIMIT_CLIENT_BODY_SIZE не задан) parserSCF-236
- очень много мелких доработок
- DONE!!!! работаю над приёмом бинарного файла (пока одного) - SR_POST
- МОГУ передать несколько картинок!!!!!!

27/10 #Д#
- изменил в структуре данных: ввёл unsigned long limitClientBodySize (для создания массива чаров буфера body)
- надо предусмотреть:
	- DONE проверка размера Content-Length (не больше он, чем limitClientBodySize, иначе - еррор?) SR _readSd
	- DONE выделение памяти под массив чаров: bodyBinar = (char *)malloc (connection.сontentLength + 1) SR _readSd
	- ? очистка памяти после сохранения файлов: free(bodyBinar) (SR_POST _postSaveFile last line) - должно ли зависить от количесва частей/файлов? Проверить
- DONE надо: переделать парсинг Body в чары SR _readSd
- надо: при парсинге учесть boundary
- надо: сделать, чтоб файлы открывались и в них писалось/дописывалось в бинарном виде отдельно по файлам

26/10 #Д#
- заменил connection.lenBody на connection.сontentLength (что по смыслу правильно)
- _findConnectionСontentLength теперь bool
- полностью переписал SR _readSd (недоделал)
- выяснилось, что нельзя буфер добавлять к строке (проглатываются символы) => надо переписать конкатенацию строк, начиная с connection->inputStr += buf; SR-430. Возможно, писать не в строку, а в файл бинарный??

25/10 #Д#
- подправил DELETE страницу 8080
- сделал ответ на запрос DELETE _prepareDeleteAnswer страницей как https://developer.mozilla.org/ru/docs/Web/HTTP/Methods/DELETE
- пофиксил ожидание ответа сервера после запроса DELETE (требовалось content-type и Content-Length)
- разделил запрос по амперсанту на адрес и параметры (_parseParamsStr)
- ввёл переменные _serverIp (SR) & _port (S)
- сделал переадресацию со страниц Deleted (DELETE) & Success (POST) на главную страницу того сайта
- надо сделать: SR-526 проверить логику: должен сохраняться ВСЁ тело, потом распарсиваться. Сейчас какие-то куски. Надо разобраться. Возможно, поправить всю логику, начиная с SR-419

24/10 #Д#
- установил ReqBin Google Chrome Extension https://chrome.google.com/webstore/detail/reqbin-http-client/gmmkjpcadciiokjpikmkkmapphbmdjok . Запрос DELETE на нём отрабатывается (см раздел "Как тестировать")
- Пофиксил: надо сделать стр 405 в методе DELETE когда статус 405. Сделал универсальную страницу на ошибки (смотрит по коду ошибки) ServerRouter_addStatusCodePage.cpp
- сделал _createAutoindex
- Пофиксил: надо проверить и доделать логику запроса _createAutoindex
- сделал печать даты в ответах
- FIXED надо сделать: разделить запрос по амперсанту на адрес и параметры
- надо пофиксить: ответ POST не должен всегда быть Continue (100)

23/10 #Д#
- подправил раздел этого файла "как тестировать"
- подправил шрифт в стр new404
- убрал _addressDecode (была задвоена с urlDecode)
- сделал метод DELETE. Чекать из client_delete.cpp
- FIXED надо: сделать возврат из метода DELETE страницей как https://developer.mozilla.org/ru/docs/Web/HTTP/Methods/DELETE
```
<html>
  <body>
    <h1>File deleted.</h1>
  </body>
</html>
```
- ? надо: сделать список файлов на странице DELETE в браузере
- FIXED надо: сделать выбор и посыл на удаление файла со странице в браузере - сделал перенаправление на страницу, с которой можно сделать метод DELETE

22/10 #Д#
- добавил кодер urlEncode(std::string str)
- пофиксил SR_POST _postFormData (сегался в конце сообщения, когда уже не было имени файла)
- пофиксил: надо сделать: запрос GET в кирилице
- слегка поправил new404 (картинки не стал, так как их будут искать в директории конкретного сервера)
- надо обработать POST картинок
- надо обработать неск файлов, надо учесть длинный файл (больше одного запроса)
- надо обработать отправку формы как: https://developer.mozilla.org/ru/docs/Learn/Forms/Sending_and_retrieving_form_data

21/10 #Д#
- пофиксил длинный путь к картинкам 404.
- убрал папку upload из locations, оставил только на сервере -> parsing, print...
- добавил папку downloads по умолчанию в upload;
- эта папка будет создана в директории сервера, если её не было
- пофикшено: надо пофиксить: создаются мусорные файла при сохранении нового файла на сервер
- пофиксил состояние данных для записи (убрал ДЕЛИМЕТЕР)
- сделал функцию печати чар=инт
- убрал 	_splitStrDelimeter (некорректно работала), заменил на splitStringStr
- пофиксил реферер 404 в кирилице (поставил декодер urlDecode)
- надо обработать отправку формы как: https://developer.mozilla.org/ru/docs/Learn/Forms/Sending_and_retrieving_form_data

20/10 #Д#
- SR_POST выделил имя файла
- сделал кнопки на сайте 8080
- работаю над _getFile в SR_POST-210
- работаю над lenAnswer в POST (SR-364, SR_GET-396)
- надо: _postGetFilePath(t_connection * connection); _postCheckIsFileExist(t_connection * connection); _postSaveFile(t_connection * connection) (SR_POST-325)
- надо обработать неск файлов, надо учесть длинный файл (больше одного запроса)
- надо поправить: SR_GET-315, 323: при переадресации не учитывается случай, когда путь к несуществующему файлу является папкой (не отображаются картинки 404)

19/10 #Д#
- решил в один из сайтов (8080) вставить форму отправки файла на сервер
- autoindex теперь bool
- делаю SR_POST _postFormData (выделил boundary)
- boundary теперь вектор в структуре
- необх сделать рекурсивный(?) поиск разделителей и обработку данных

18/10 #Д#
- перенёс клиент из ft_irc для тестирование метода POST (отправки файлов на сервер)
- занимаюсь переделкой этого клиента под наши нужды
- необх в клиенте разобраться с типами ПОСТ запросов и сформировать правильный (client-commands _preparePostRequest 113)


17/10 #Д#
- подправил readme
- пофиксил pics 404
- DONE (SR-512): надо ласт тайм поправить (вышибает соединение)
- надо client_multi чтоб он мог открывать файлы и перекидывать их содержимое. То есть в argv - имена файлов

11/10 #Д#
Работаю над картинками в 404:
- findLastSlashInAddress
- _findReferer;
- надо: использовать connection->referer в случае 404 (SR_GET-265);
Сделал картинки 404
- подправил принты во всех файлах

10/10 #Д#
- браузхеры с keep-alive засоряют sd: я решил их отключать по таймауту, чтоб чистить sd (# define TIMEOUT в main.hpp)
- пофиксил readFileToStrInBinary в utility
- пока не справился с картинками в 404: я ввёл новые поля в connections.responseData: pathExcludeInErr и pathIncludeBeginInErr, могу получитиь адрес. Также ввёл _ifErrPages. Но браузер постоянно ломится за доп ресурсами (картинки и пр) по старому ошибочному адресу => там нет этих ресурсов. Пока закомментил

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
