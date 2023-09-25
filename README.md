# TransportCatalogue
LocalTransportCatalogue
Транспортный справочник.
Програмная реализация локального сервера, осуществляющего хранение и систематизацию маршрутов и остановок, вычисление протяженности маршрутов, предоставление информации об остановках (какие маршруты проходят через заправшиваемую остановку) и маршрутах (количество остановок, количество уникальных остановок, протяженность маршрута, отношение фактической длины маршрута к географическому расстоянию), путем обработки, поиска и выдачи строковых данных.

Типы запросов к программе: - запрос на добавление данных; - запрос информации (функция AddInfo добавляет данные в справочник, функция SearchInfo обрабатывает запросы; каждая из них перед началом ввода информации запрашивает количество команд на ввод).
 
Рекомендуется использовать стандарт C++ не ниже 17 (лучше 20), STL.

Планы по доработке: - для повышения комфорта использования создать десктопное приложение; - внедрить возможность различных форматов ввода данных (read_input_fuctions); - внедрить возможность выдачи результатов поиска в различных форматах; - вывести программу из локального формата на веб-ресурс при дальнейшем желании.

Стек технологий: C++ 20 (STL), Microsoft Visual Studio 2022.

Программа читает данные из потока. Есть возможность перенаправить вход на чтение из файлов.

Формат входных данных:
13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye

Ожидаемый вывод: 
Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828 
