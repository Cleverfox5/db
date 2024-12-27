#pragma once
#include <pqxx/pqxx>
#include <iostream>

class WorkWithDB {
public:
    WorkWithDB(std::string& connection_str);

    void get_year_and_country_by_band(const std::string& band_name);
    void get_songs_list_by_band(const std::string& band_name);
    void get_author_composer_year_by_song(const std::string& song_name);
    void get_place_duration_by_band(const std::string& band_name);
    void get_price_date_by_band(const std::string& band_name);
    void get_musicians_by_band(const std::string& band_name);
    
    void add_new_band(const std::string& band_name, const std::string & founding, const std::string & country);
    void add_new_song(const std::string& song_name, const std::string & created_date, 
        const std::string& band_name, const std::string& name_composer, const std::string& surname_composer, 
        const std::string& name_author, const std::string& surname_author);
    void delete_song(const std::string& song_name);
    void add_new_musician(const std::string& name, const std::string& surname, const std::string& birth_date, 
        const std::string& role, const std::string& band_id);
    void delete_musician(const std::string& name, const std::string& surname);


private:
    std::shared_ptr<pqxx::connection> connect_;
};