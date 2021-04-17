#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <cstring>
#include "pugi/src/pugixml.cpp" // с заголовочником линковщик плачет

const double earth_radius = 6371; // радиус Земли в километрах
const double INF = 1e5; // "бесконечность" для алгоритма Прима

typedef std::pair<double, double> cords; // координаты остановки
typedef std::vector<cords> transport; // вектор остановок для каждого маршрута
typedef std::map<std::string, transport> route_stops; // мапа "номер машрута - вектор с маршрутом"
typedef struct { unsigned bus, troll, tram; } stops_on_street; // счетчики остановок по видам транспорта

/* вычисление расстояния между точками на Земле по формуле Гаверсинуса */

double gaversinus(double lat1, double lon1, double lat2, double lon2)
{
    double lat = (lat2-lat1) * (M_PI/180);
    double lon = (lon2-lon1) * (M_PI/180);
    double res = sin(lat/2) * sin(lat/2) + cos(lat1*(M_PI/180)) * cos(lat2*(M_PI/180)) * sin(lon/2) * sin(lon/2);
    return 2 * earth_radius * atan2(sqrt(res), sqrt(1-res));
}

double primMST(size_t v_count, const transport &stops_cords)
{
    std::vector<bool> inMST(v_count);
    std::vector<double> dist(v_count, INF);

    double mst_dist = 0;

    dist[0] = 0;

    for (size_t i = 0; i < v_count; i++)
    {
        int min_key = -1;

        for (size_t j = 0; j < v_count; j++)
            if (!inMST[j] && (min_key == -1 || dist[j] < dist[min_key])) min_key = j;

        inMST[min_key] = true;

        for (size_t j = 0; j < v_count; j++)
        {
            double lat_first = stops_cords[min_key].first, lat_second = stops_cords[j].first,
                    lon_first = stops_cords[min_key].second, lon_second = stops_cords[j].second;
            double d = gaversinus(lat_first, lon_first, lat_second, lon_second);
            if (!inMST[j] && d < dist[j] && min_key != j) dist[j] = d;
        }
    }

    for (const double &u: dist)
    {
        mst_dist += u;
    }

    return mst_dist;
}

class Data
{
private:
    std::map<std::string, route_stops> routes_info; // unordered_map не вышел
    std::map<std::string, stops_on_street> streets; // - влезает не больше (2^8 - 1) эл-тов
    pugi::xml_document transport_doc; // док
    pugi::xml_node root; // корень (dataset)
public:
    void open_xml();
    void parse();
    void solve_first() const;
    void solve_second() const;
    void solve_third() const;
};

void Data::open_xml()
{
    std::cout << "Opening the XML file...\n";
    pugi::xml_parse_result result = transport_doc.load_file("data.xml");

    std::cout << "XML file open attempt: " << result.description() << "\n\n";
    root = transport_doc.child("dataset");
}

void Data::parse()
{
    for (pugi::xml_node &node: root.children())
    {
        std::string type, location, routes, coordinates;

        /* качаем нужные нам данные из файла (тип транспорта, улицу, маршруты и координаты остановки) */

        type = node.child("type_of_vehicle").text().as_string();
        location = node.child("location").text().as_string();
        routes = node.child("routes").text().as_string();
        coordinates = node.child("coordinates").text().as_string();

        /* распихиваем координаты остановок по маршрутам, а для улиц прибавляем счетчики остановок
         * по каждому виду транспорта */

        std::string cur_location;
        cords cur_cords;

        char* cords_ptr = const_cast<char *>(coordinates.data());
        char* div_cords = strtok(cords_ptr, ",");
        cur_cords.first = atof(div_cords);
        div_cords = strtok(nullptr, ",");
        cur_cords.second = atof(div_cords);

        char* routes_ptr = const_cast<char *>(routes.data());
        char* div_routes = strtok(routes_ptr, ",.");

        while (div_routes != nullptr)
        {
            routes_info[type][div_routes].push_back(cur_cords);
            div_routes = strtok(nullptr, ".,");
        }

        char* location_ptr = const_cast<char *>(location.data());
        char* div_location = strtok(location_ptr, ",");

        while (div_location != nullptr)
        {
            cur_location = div_location;
            if (cur_location[0] == ' ')
                cur_location.erase(0, 1);

            if (type == "Bus") ++streets[cur_location].bus;
            else if (type == "Tram") ++streets[cur_location].tram;
            else ++streets[cur_location].troll;

            div_location = strtok(nullptr, ",");
        }
    }

    std::cout << "Parsed successfully!\n\n";
}

void Data::solve_first() const
{
    std::cout << "Routes with the most stops (one way):" << "\n";

    std::pair<std::string, unsigned> max_route_size;

    for (auto const &veh_type: routes_info)
    {
        for (auto const &route: veh_type.second)
        {
            unsigned cur_size = route.second.size();
            if (cur_size > max_route_size.second) max_route_size = std::make_pair(route.first, cur_size);
        }
        std::cout << veh_type.first << ": route " << max_route_size.first << ", approximately " << max_route_size.second / 2
                  << " stops." << "\n";
        max_route_size = std::make_pair(0, 0);
    }

    std::cout << "\n";
}

void Data::solve_second() const
{
    std::cout << "The longest routes (by distance):\n";

    std::pair<std::string, double> longest;

    /* для подсчета длины маршрута проходим по остановкам каждого маршрута алгоритмом Прима,
     * в данном случае выбрана реализация за O(N^2), поскольку является
     * наиболее подходящей для полных графов и решения евклидовой задачи (правда, вместо декартовой системы
     * мы имеем координаты остановок на земле, а полный граф нам приходится использовать вследствие
     * невозможности установить последовательность остановок по данному датасету) */

    for (auto &veh_type: routes_info)
    {
        for (auto &route: veh_type.second)
        {
            double cur_length = primMST(route.second.size(), route.second);
            if (cur_length > longest.second) { longest = std::make_pair(route.first, cur_length); }
        }

        std::cout << veh_type.first << ": route " << longest.first << ", approximately " << longest.second
                  << " km." << "\n";
        longest.second = 0;
    }

    std::cout << "\n";
}

void Data::solve_third() const
{
    std::pair<std::string, unsigned> maximum{"", 0};

    /* тут мы тупо работаем с получившимися счетчиками для каждой из улиц */

    for (auto const &stops: streets)
    {
        unsigned sum = stops.second.tram + stops.second.bus + stops.second.troll;
        if (sum > maximum.second) maximum = std::make_pair(stops.first, sum);
    }

    std::cout << "Street with the maximum number of stops (in two directions): " << maximum.first << "\n";
    std::cout << "Total number of stops: " << maximum.second << "\n";

    std::cout << "Number of stops by type of transport: \n";
    std::cout << "Bus stops: " << streets.at(maximum.first).bus << "\n";
    std::cout << "Tram stops: " << streets.at(maximum.first).tram << "\n";
    std::cout << "Trolleybus stops: " << streets.at(maximum.first).troll << "\n\n";
}


int main(int argc, char *argv[])
{
    Data transport;

    if (argc != 2)
    {
        std::cout << "Enter the task number (1st, 2nd or 3rt) by command prompt argument!" << "\n";
        exit(2);
    }

    transport.open_xml();
    transport.parse();

    switch(argv[1][1])
    {
        case 'f':
            transport.solve_first();
            break;
        case 's':
            transport.solve_second();
            break;
        case 't':
            transport.solve_third();
            break;
        default:
            std::cout << "CMD argument error";
            return 3;
    }

    return 0;
}