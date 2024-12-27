#include "WorkWithDB.h"
#include <Windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    std::string connection_string = "host = localhost port = 5432 dbname = music_bands user = postgres password = Postgre123password";
    std::string band_name = "Король и шут";
    std::string song_name = "Песня 3";
    std::string anoter_band_name = "Хе-хе, ха-ха";
    std::string founding = "2003-12-23";
    std::string country = "Россия";

    try {
        WorkWithDB db(connection_string);

        std::cout << "Просто тесты\n";

        //тест части менеджера
        db.get_year_and_country_by_band(band_name);
        db.get_songs_list_by_band(band_name);
        db.get_author_composer_year_by_song(song_name);
        db.get_place_duration_by_band(band_name);
        db.get_price_date_by_band(band_name);
        db.get_musicians_by_band(band_name);

        //тест части администратора
        //db.add_new_band(anoter_band_name, founding, country);
        db.add_new_song("jsdlf", "2015-12-23", "Король и шут", "Имя", "Фамилия", "Третье имя", "Фамилия");
        db.delete_song("jsdlf");
        db.add_new_musician("ыафва", "sdfsdf", "2003-12-23", "элктронная гитара", "Король и шут");
        db.delete_musician("ыафва", "sdfsdf");
    }
    catch (const std::runtime_error & e) {
        return -1;
    }

    return 0;
}