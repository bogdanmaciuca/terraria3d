#pragma once
#include "int.hpp"
#include <string>

typedef i16 ItemID;

enum class ItemRarity {
    Gray, White, Blue, Green, Orange, LightRed, Pink,
    LightPurple, Lime, Yellow, Cyan, Red, Purple, Rainbow
};

struct Item {
    ItemRarity rarity;
    bool placeable;
    i64 price;
};

