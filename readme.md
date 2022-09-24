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
- что такое cgi

### Хронология изменений:
24/9 #Д#
- подкорректировал сообщения в логфайле
- начал переделывать обработку вх данных
- не работает выход из цикла, когда клиент ctrl+c (раньше работал)

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
