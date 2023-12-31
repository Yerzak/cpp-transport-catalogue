# TransportCatalogue
LocalTransportCatalogue
Транспортный справочник.
Програмная реализация локального сервера, осуществляющего хранение и систематизацию маршрутов и остановок, вычисление протяженности маршрутов, предоставление информации об остановках (какие маршруты проходят через заправшиваемую остановку) и маршрутах (количество остановок, количество уникальных остановок, протяженность маршрута, отношение фактической длины маршрута к географическому расстоянию), путем обработки, поиска и выдачи строковых данных, информации в формате JSON. Кроме того, программа визуализирует имеющиеся маршруты, отмечая остановки на них, используя для этого формат SVG.

Типы запросов к программе: 
- запрос на добавление данных;
- запрос информации;
- запрос визуализации карты маршрутов.
  Данные удобно подавать в виде JSON-файла, содержащего словарь с 3 типами информации (base_requests - добавление информации об остановках и маршрутах, render_seettings - параметры изображения (толщина линий, подложки, отступы и т.п.), stat_requests - запрос информации, в том числе запрос на отрисовку карты).
 
Рекомендуется использовать стандарт C++ не ниже 17 (лучше 20), STL.

Планы по доработке: 
- для повышения комфорта использования создать десктопное приложение;
- вывести программу из локального формата на веб-ресурс при дальнейшем желании.

Стек технологий: C++ 20 (STL), Microsoft Visual Studio 2022.

Программа читает данные из потока (iostream, fstream).
