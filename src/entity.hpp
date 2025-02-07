#pragma once
#include "int.hpp"
#include <vector>

typedef i16 ItemID;

enum class ItemRarity {
    Gray, White, Blue, Green, Orange, LightRed, Pink,
    LightPurple, Lime, Yellow, Cyan, Red, Purple, Rainbow
};

struct Item {
    i16 use_time;
    ItemRarity rarity;
    bool placeable;
};

struct Entity {
    i16 max_life;
    i16 defense;
    i16 kb_resist;
};

struct Mob : public Entity {
    ItemID banner;
};

static std::vector<Item> items;

