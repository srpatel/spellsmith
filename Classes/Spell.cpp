//
//  Spell.cpp
//  GemGame
//
//  Created by Sunil Patel on 06/11/2015.
//
//

#include "Spell.hpp"
#include "Gem.hpp"
#include "GameScene.hpp"
#include "Strings.hpp"

#include "json/document.h"

#define MAX_WIDTH 5
#define MAX_HEIGHT 5

static GemType *compare_temp;
Spells *Spells::instance = nullptr;

#define DEBUGGING_SPELLS 0
#define NewArray new GemType[MAX_WIDTH * MAX_HEIGHT]
#define Zero(grid) for (int i = 0; i < MAX_WIDTH; i++) for (int j = 0; j < MAX_HEIGHT; j++)\
	grid at(i, j) = NONE;
#define at(i, j) [(i) + (j) * MAX_WIDTH]

Spells *Spells::get() {
	if (instance == nullptr) {
		instance = new Spells;
	}
	return instance;
}

void Spells::init() {
	compare_temp = NewArray;
	
	std::string data = FileUtils::getInstance()->getStringFromFile("data/spells.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& spell = doc[i];
		const rapidjson::Value& shape = spell["shape"];
		const rapidjson::Value& effects = spell["effects"];
		auto s = new Spell(_(std::string("spell.") + spell["name"].GetString() + ".name"));
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
		
		for (int j = 0; j < effects.Size(); j++) {
			const rapidjson::Value& effect = effects[j];
			// Switch type and create new type depending.
			BaseEffect *e = nullptr;
			if (strcmp(effect["type"].GetString(), "PROJECTILE") == 0) {
				EffectProjectile *theEffect = new EffectProjectile;
				theEffect->type = Projectile;
				theEffect->damage = effect["damage"].GetInt();
				e = theEffect;
			} else if (strcmp(effect["type"].GetString(), "HEAL") == 0) {
				EffectHeal *theEffect = new EffectHeal;
				theEffect->type = Heal;
				theEffect->amount = effect["amount"].GetInt();
				e = theEffect;
			} else if (strcmp(effect["type"].GetString(), "SHIELD") == 0) {
				EffectShield *theEffect = new EffectShield;
				theEffect->type = Shield;
				theEffect->amount = effect["amount"].GetInt();
				e = theEffect;
			}
			if (e) {
				s->effects.push_back(e);
			}
		}
		
		s->setup();
		spells.push_back(s);
	}
}

Spell::Spell(std::string name) {
	this->name = name;
	shape = NewArray;
	// Zero it!
	Zero(shape);
}

Spell::~Spell() {
	delete [] shape;
}

std::string Spell::getName() {
	return name;
}

void Spell::setup() {
	auto layer = Layer::create();
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
			switch (type) {
				case AIR: element = "gems/mini_air.png"; break;
				case EARTH: element = "gems/mini_earth.png"; break;
				case FIRE: element = "gems/mini_fire.png"; break;
				case WATER: element = "gems/mini_water.png"; break;
				default: /*hopefully won't happen!*/ break;
			}
			auto sprite = Sprite::createWithSpriteFrameName(element);
			auto size = sprite->getContentSize();
			float x = (i - width/2.f) * size.width;
			float y = (j - height/2.f) * size.height;
			sprite->setPosition(x, y);
			layer->addChild(sprite);
		}
	}
	mininode = layer;
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
/*
 When debugging, print chart with this:
 printf("\n");
	for (int j = 0; j < Spell::max_height; j++) {
 for (int i = 0; i < Spell::max_width; i++) {
 printf("%d", compare_temp at(i,j));
 }
 printf("\n");
	}
 */
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
			bool match = memcmp(compare_temp, shape, MAX_WIDTH * MAX_HEIGHT * sizeof(GemType)) == 0;
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