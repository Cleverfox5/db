#include "WorkWithDB.h"

WorkWithDB::WorkWithDB(std::string& connection_str_)
    : connect_(std::make_shared<pqxx::connection>(connection_str_))
{
    try {
        if (connect_->is_open()) {
            std::cout << "Open db successfullt: " << connect_->dbname() << std::endl;
        }
        else {
            std::cerr << "Can't open database\n";
            throw std::runtime_error("Can't open db");
        }

        connect_->prepare("get_year_and_country_by_band",
            R"(
                SELECT DATE_PART('year', founding), country
                FROM bands
                WHERE name = $1;
            )");
        connect_->prepare("get_songs_list_by_band",
            R"(
                SELECT
                    s.name,
                    m1.first_name AS composer_name,
                    m1.surname AS composer_surname,
                    m2.first_name AS author_name,
                    m2.surname AS author_surname
                FROM 
                    songs s
                JOIN 
                    musicians m1 ON s.composer_id = m1.id
                JOIN 
                    musicians m2 ON s.author_id = m2.id
                    WHERE s.band_id = 
                    (
                        SELECT id
                        FROM bands
                        WHERE name = $1
                    );
            )");
        connect_->prepare("get_author_composer_year_by_song",
            R"(
                SELECT 
	                m1.first_name AS composer_name,
	                m1.surname AS composer_surname,
	                m2.first_name AS author_name,
	                m2.surname AS author_surname,
	                created_date
                FROM songs
                JOIN 
	                musicians m1 ON composer_id = m1.id
                JOIN 
	                musicians m2 ON author_id = m2.id
                WHERE name = $1;
            )");
        connect_->prepare("get_place_duration_by_band",
            R"(
                SELECT t.name, t.finish_date - t.start_date AS duration_in_days
                FROM touring t
                JOIN cities c ON c.id = t.city_id
                WHERE t.band_id = (
	                SELECT id
	                FROM bands
	                WHERE name = $1
                );
            )");
        connect_->prepare("get_price_date_by_band",
            R"(
                SELECT price, start_date 
                FROM touring
                WHERE band_id = (
	                SELECT id
	                FROM bands
	                WHERE name = $1
                );
            )");
        connect_->prepare("get_musicians_by_band",
            R"(
                SELECT first_name, surname, (CURRENT_DATE - birth_date) / 365 AS age, role
                FROM musicians
                WHERE band_id = (
	                SELECT id
	                FROM bands
	                WHERE name = $1
                );
            )");

        connect_->prepare("add_new_band_first_part",
            R"(
            INSERT INTO bands(name, founding, country)
            VALUES($1, $2, $3);
            )");
        connect_->prepare("add_new_band_second_part",
            R"(
            INSERT INTO charts (band_id, position)
            VALUES (
                (SELECT id FROM bands WHERE name = $1),
                (SELECT COALESCE(MAX(position), 0) FROM charts) + 1
            );
            )");
        connect_->prepare("add_new_song",
            R"(
                INSERT INTO songs (name, composer_id, author_id, created_date, band_id)
                VALUES($1, 
                (
	                SELECT id FROM musicians
	                WHERE first_name = $2 AND surname = $3
                ),
                (
	                SELECT id FROM musicians
	                WHERE first_name = $4 AND surname = $5
                ), 
                $6, 
                (
	                SELECT id FROM bands
	                WHERE name = $7
                )
                );
            )");
        connect_->prepare("delete_song",
            "DELETE FROM songs "
            "WHERE name = $1; "
        );
        connect_->prepare("add_new_musician",
            "INSERT INTO musicians (first_name, surname, birth_date, role, band_id) "
            "VALUES($1, $2, $3, $4, "
            "("
            "   SELECT id "
            "   FROM bands "
            "   WHERE name = $5 "
            ") "
            ");"
        );
        connect_->prepare("delete_musician",
            "DELETE FROM musicians "
            "WHERE first_name = $1 AND surname = $2;"
        );
        connect_->prepare("go_up_in_charts",
            "UPDATE charts "
            "SET position = (SELECT posion from charts where band_id = (SELECT id FROM bands WHERE name = $1)) + 1"
            "WHERE band_id = ("
            "SELECT id FROM bands WHERE name = $1"
            ");"
        );
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

void WorkWithDB::get_year_and_country_by_band(const std::string & band_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_year_and_country_by_band", band_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::get_songs_list_by_band(const std::string& band_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_songs_list_by_band", band_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::get_author_composer_year_by_song(const std::string& song_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_author_composer_year_by_song", song_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::get_place_duration_by_band(const std::string& band_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_place_duration_by_band", band_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::get_price_date_by_band(const std::string& band_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_price_date_by_band", band_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::get_musicians_by_band(const std::string& band_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("get_musicians_by_band", band_name);

        for (pqxx::result::iterator res_it = result.begin(); res_it != result.end(); ++res_it) {
            for (pqxx::row::iterator row_it = (*res_it).begin(); row_it != (*res_it).end(); ++row_it) {
                std::cout << row_it->c_str() << " -- ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::add_new_band(const std::string& band_name, const std::string& founding, const std::string& country) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        result = work.exec_prepared("add_new_band_first_part", band_name, founding, country);
        result = work.exec_prepared("add_new_band_second_part", band_name);

        work.commit();

        std::cout << "The band was added successfully" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::add_new_song(const std::string& song_name, const std::string& created_date,
    const std::string& band_name, const std::string& name_composer, const std::string& surname_composer,
    const std::string& name_author, const std::string& surname_author) 
{
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        work.exec_prepared("add_new_song", song_name, name_composer, surname_composer,
            name_author, surname_author, created_date, band_name);

        work.commit();

        std::cout << "The song was added successfully" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::delete_song(const std::string& song_name) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        work.exec_prepared("delete_song", song_name);

        work.commit();

        std::cout << "The song was deleted successfully" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::add_new_musician(const std::string& name, const std::string& surname, const std::string& birth_date,
    const std::string& role, const std::string& band_id) {
    try {
        pqxx::work work(*connect_);
        pqxx::result result;

        work.exec_prepared("add_new_musician", name, surname, birth_date, role, band_id);

        work.commit();

        std::cout << "The musician was added successfully" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void WorkWithDB::delete_musician(const std::string& name, const std::string& surname) {
    try {
        pqxx::work work(*connect_);

        work.exec_prepared("delete_musician", name, surname);

        work.commit();

        std::cout << "The musician was deleted successfully" << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}