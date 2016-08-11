//
//  Spell.cpp
//  GemGame
//
//  Created by Sunil Patel on 06/11/2015.
//
//

#include "Spell.hpp"
#include "Gem.hpp"
#include "Strings.hpp"

#include "json/document.h"

#define MAX_WIDTH 5
#define MAX_HEIGHT 5

static GemType *compare_temp;
SpellManager *SpellManager::instance = nullptr;

#define DEBUGGING_SPELLS 0
#define NewArray new GemType[MAX_WIDTH * MAX_HEIGHT]
#define Zero(grid) for (int i = 0; i < MAX_WIDTH; i++) for (int j = 0; j < MAX_HEIGHT; j++)\
	grid at(i, j) = NONE;
#define at(i, j) [(i) + (j) * MAX_WIDTH]
/*
static int MakeOptionalNumber(const rapidjson::Value& e, const char* key) {
	if (e.HasMember(key)) {
		return e[key].GetInt();
	} else {
		return -1;
	}
}

static std::function<int(Game *)> MakeAmountGenerator(const rapidjson::Value& e) {
	// If it has "amount" use that.
	if (e.HasMember("amount")) {
		int i = e["amount"].GetInt();
		return [i](Game *){ return i; };
	} else if (e.HasMember("min") && e.HasMember("max")) {
		int min = e["min"].GetInt();
		int max = e["max"].GetInt();
		return [min, max](Game *){ return (std::rand()%(max-min)) + min; };
	} else {
		return [](Game *){ return 0; };
	}
}
*/
SpellManager *SpellManager::get() {
	if (instance == nullptr) {
		instance = new SpellManager;
	}
	return instance;
}

void SpellManager::init() {
	compare_temp = NewArray;
	
	std::string data = FileUtils::getInstance()->getStringFromFile("data/spells.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& spell = doc[i];
		const rapidjson::Value& shape = spell["shape"];
		const rapidjson::Value& tier = spell["tier"];
		auto s = new Spell(spell["name"].GetString());
		s->tier = tier.GetInt();
#if DEBUG
		if (shape.Size() != MAX_HEIGHT) {
			LOG("height != MAX_HEIGHT\n");
		}
#endif
		for (int j = shape.Size() - 1; j >= 0; j--) {
			const char *row = shape[j].GetString();
			// TODO : should it be strlen?
			int width = strlen(row);
#if DEBUG
			if (width != MAX_WIDTH) {
				LOG("width != MAX_WIDTH\n");
			}
#endif
			for (int k = 0; k < MAX_WIDTH; k++) {
				GemType g = NONE;
				switch (row[k]) {
					case ' ': g = NONE; break;
					case 'F': g = FIRE; break;
					case 'W': g = WATER; break;
					case 'E': g = EARTH; break;
					case 'A': g = AIR; break;
				}
				if (g != NONE) {
					s->shape at(k, MAX_HEIGHT - j - 1) = g;
				}
			}
		}
		
		s->setup();
		spells.push_back(s);
	}
}

Spell::Spell(const char *name) {
	this->name = name;
	shape = NewArray;
	// Zero it!
	Zero(shape);
}

Spell::~Spell() {
	delete [] shape;
}

std::string Spell::getDescription() {
	std::string translated = _(std::string("spell.") + name + ".description");
	return translated;
}

std::string Spell::getName() {
	std::string translated = _(std::string("spell.") + name + ".name");
	return translated;
}

Layer *Spell::makeNode(bool mini) {
	auto layer = Layer::create();
	layer->setContentSize(Size(40, 40));
	
	int width = 0, height = 0;
	// Find height and width of shape (so we can draw the spell centred)
	// TODO : Could make this neater... not that it matters!
	For2(MAX_WIDTH, MAX_HEIGHT) {
		if (shape at(i, j) != NONE) {
			if (i > width) {
				width = i;
			}
			if (j > height) {
				height = j;
			}
		}
	}
	
	// Populate with gems.
	For2(MAX_WIDTH, MAX_HEIGHT) {
		auto type = shape at(i, j);
		if (type != NONE) {
			const char *element;
			if (mini) {
				switch (type) {
					case AIR: element = "gems/mini_air.png"; break;
					case EARTH: element = "gems/mini_earth.png"; break;
					case FIRE: element = "gems/mini_fire.png"; break;
					case WATER: element = "gems/mini_water.png"; break;
					case CRYSTAL: element = "gems/mini_gold.png"; break;
					default: /*hopefully won't happen!*/ break;
				}
			} else {
				switch (type) {
					case AIR: element = "gems/air.png"; break;
					case EARTH: element = "gems/earth.png"; break;
					case FIRE: element = "gems/fire.png"; break;
					case WATER: element = "gems/water.png"; break;
					case CRYSTAL: element = "gems/gold.png"; break;
					default: /*hopefully won't happen!*/ break;
				}
			}
			auto sprite = LoadSprite(element);
			auto size = sprite->getContentSize();
			float x = (i - width/2.f) * size.width;
			float y = (j - height/2.f) * size.height;
			sprite->setPosition(x, y);
			layer->addChild(sprite);
		}
	}
	
	return layer;
}

void Spell::setup() {
	mininode = makeNode(true);
	mininode->setContentSize(Size(40, 40));
	mininode->retain();
	mininode->setAnchorPoint(Vec2::ZERO);
}

// Helper functions
// Copy Chain into a grid
void Spell::copy(Chain *chain, GemType *grid) {
	Chain *sentinel = chain;
	while (sentinel != nullptr) {
		int i = sentinel->i;
		int j = sentinel->j;
		grid at(i, j) = sentinel->type;
		sentinel = sentinel->next;
	}
}
// Align a shape inside the grid to the bottom-left
void Spell::align(GemType *grid) {
	int min_i = INT_MAX;
	int min_j = INT_MAX;
	
	For2(MAX_WIDTH, MAX_HEIGHT) {
		if (grid at(i, j) != NONE) {
			if (i < min_i)
				min_i = i;
			
			if (j < min_j)
				min_j = j;
		}
	}
	
	// Offset each gem - if there is offsetting to do
	if (min_i > 0 || min_j > 0) {
		for (int i = min_i; i < MAX_WIDTH; i++) {
			for (int j = min_j; j < MAX_HEIGHT; j++) {
				grid at(i - min_i, j - min_j) = grid at(i, j);
				grid at(i, j) = NONE;
			}
		}
	}
}
// Rotate by 90 degs...
void Spell::rotate(GemType *grid) {
	GemType temp[MAX_WIDTH * MAX_HEIGHT];
	// Copy into temp grid
	For2(MAX_WIDTH, MAX_HEIGHT) {
		temp at(i, j) = grid at(i, j);
	}
	
	Zero(grid);
	
	// Copy each column into a row
	// TODO : This rotatation assumes height = width!
	for (int i = MAX_WIDTH - 1; i >= 0; i--) {
		for (int j = 0; j < MAX_HEIGHT; j++) {
			grid at(j, MAX_HEIGHT - i - 1) = temp at(i, j);
		}
	}
}
// Flip by any axis
void Spell::flip(GemType *grid) {
	for (int i = 0; i < MAX_WIDTH; i++) {
		for (int j = 0; j < MAX_HEIGHT / 2; j++) {
			int mirror_j = MAX_HEIGHT - j - 1;
			auto temp = grid at(i, j);
			grid at(i, j) = grid at(i, mirror_j);
			grid at(i, mirror_j) = temp;
		}
	}
}

bool Spell::operator==(Chain *chain) {
	Zero(compare_temp);
	copy(chain, compare_temp);
	
#if DEBUGGING_SPELLS
	printf("\nSpell: \n");
	for (int j = 0; j < MAX_HEIGHT; j++) {
		for (int i = 0; i < MAX_WIDTH; i++) {
			printf("%d", shape at(i,j));
		}
		printf("\n");
	}
#endif
	bool success = false;
	for (int j = 0; j < 2; j++) {
		For (4) {
			rotate(compare_temp);
			align(compare_temp);
			// TODO : memcmp seems a bit crude... opt for going gem by gem. That way we can have wild-cards.
#if DEBUGGING_SPELLS
			printf("\nAttempt: \n");
			for (int j = 0; j < MAX_HEIGHT; j++) {
				for (int i = 0; i < MAX_WIDTH; i++) {
					printf("%d", compare_temp at(i,j));
				}
				printf("\n");
			}
#endif
			bool match = true;
			
			for (int n = 0; n < MAX_WIDTH; n++) {
				for (int m = 0; m < MAX_HEIGHT; m++) {
					if (shape at(n, m) != compare_temp at(n, m)) {
						// There is a difference!
						// If the user has drawn a crystal, that's a match!
						// Crystal doesn't match "none", though.
						if (shape at(n, m) == GemType::NONE ||
							compare_temp at(n, m) != GemType::CRYSTAL) {
							match = false;
							goto next;
						}
					}
				}
			}
			next:;
			//bool match = memcmp(compare_temp, shape, MAX_WIDTH * MAX_HEIGHT * sizeof(GemType)) == 0;
			if (match) {
				success = true;
				goto done;
			}
		}
		flip(compare_temp);
	}
	done:;
	
	return success;
}