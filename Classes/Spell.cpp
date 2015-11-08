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

GemType *Spell::compare_temp;
std::vector<Spell *> Spell::spells;
int Spell::max_width;
int Spell::max_height;

#define DEBUGGING_SPELLS 0
#define NewArray new GemType[Spell::max_width * Spell::max_height]
#define Zero(grid) for (int i = 0; i < Spell::max_width; i++) for (int j = 0; j < Spell::max_height; j++)\
	grid at(i, j) = NONE;
#define at(i, j) [(i) + (j) * Spell::max_width]

void Spell::init(int width, int height) {
	Spell::max_width = width;
	Spell::max_height = height;
	
	compare_temp = NewArray;
	
	std::string data = FileUtils::getInstance()->getStringFromFile("data/spells.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& spell = doc[i];
		const rapidjson::Value& shape = spell["shape"];
		auto s = new Spell(_(std::string("spell.") + spell["name"].GetString() + ".name"));
#if DEBUG
		if (shape.Size() != Spell::max_height) {
			LOG("height != Spell::max_height\n");
		}
#endif
		for (int j = shape.Size() - 1; j >= 0; j--) {
			const char *row = shape[j].GetString();
			// TODO : should it be strlen?
			int width = strlen(row);
#if DEBUG
			if (width != Spell::max_width) {
				LOG("width != Spell::max_width\n");
			}
#endif
			for (int k = 0; k < Spell::max_width; k++) {
				GemType g = NONE;
				switch (row[k]) {
					case ' ': g = NONE; break;
					case 'F': g = FIRE; break;
					case 'W': g = WATER; break;
					case 'E': g = EARTH; break;
					case 'A': g = AIR; break;
				}
				if (g != NONE) {
					s->shape at(k, Spell::max_height - j - 1) = g;
				}
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
	For2(Spell::max_width, Spell::max_height) {
		if (shape at(i, j) != NONE) {
			if (i > width) {
				width = i;
			}
			if (j > width) {
				height = j;
			}
		}
	}
	// Populate with gems.
	For2(Spell::max_width, Spell::max_height) {
		auto type = shape at(i, j);
		if (type != NONE) {
			const char *element;
			switch (type) {
				case AIR: element = "gems/mini_air.png"; break;
				case EARTH: element = "gems/mini_earth.png"; break;
				case FIRE: element = "gems/mini_fire.png"; break;
				case WATER: element = "gems/mini_water.png"; break;
				case NONE: /*hopefully won't happen!*/ break;
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
	
	For2(Spell::max_width, Spell::max_height) {
		if (grid at(i, j) != NONE) {
			if (i < min_i)
				min_i = i;
			
			if (j < min_j)
				min_j = j;
		}
	}
	
	// Offset each gem - if there is offsetting to do
	if (min_i > 0 || min_j > 0) {
		for (int i = min_i; i < Spell::max_width; i++) {
			for (int j = min_j; j < Spell::max_height; j++) {
				grid at(i - min_i, j - min_j) = grid at(i, j);
				grid at(i, j) = NONE;
			}
		}
	}
}
// Rotate by 90 degs...
void Spell::rotate(GemType *grid) {
	GemType temp[Spell::max_width * Spell::max_height];
	// Copy into temp grid
	For2(Spell::max_width, Spell::max_height) {
		temp at(i, j) = grid at(i, j);
	}
	
	Zero(grid);
	
	// Copy each column into a row
	// TODO : This rotatation assumes height = width!
	for (int i = Spell::max_width - 1; i >= 0; i--) {
		for (int j = 0; j < Spell::max_height; j++) {
			grid at(j, Spell::max_height - i - 1) = temp at(i, j);
		}
	}
}
// Flip by any axis
void Spell::flip(GemType *grid) {
	for (int i = 0; i < Spell::max_width; i++) {
		for (int j = 0; j < Spell::max_height / 2; j++) {
			int mirror_j = Spell::max_height - j - 1;
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
	for (int j = 0; j < Spell::max_height; j++) {
		for (int i = 0; i < Spell::max_width; i++) {
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
			for (int j = 0; j < Spell::max_height; j++) {
				for (int i = 0; i < Spell::max_width; i++) {
					printf("%d", compare_temp at(i,j));
				}
				printf("\n");
			}
#endif
			bool match = memcmp(compare_temp, shape, Spell::max_width * Spell::max_height * sizeof(GemType)) == 0;
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