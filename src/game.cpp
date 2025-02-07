#include "game.hpp"
#include "../vendor/simdjson.h"
#include <string>

Game::Game(i16 window_width, i16 window_height) {
    LoadDataJSON();
}

void Game::Run() {

}

void Game::Cleanup() {

}

// TODO: error handling
void Game::LoadDataJSON() {
    using namespace simdjson;
    ondemand::parser parser;
    padded_string json = padded_string::load(DATA_JSON_FILENAME);
    ondemand::document data = parser.iterate(json);
    for (auto e: data["items"]) {
        std::string_view type = e["type"].get_string();
        if (type == "block") {

        }
        else if (type == "wall") {

        }
        if (type == "weapon") {

        }
    }
}

