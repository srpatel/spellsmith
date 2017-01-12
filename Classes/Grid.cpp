//
//  Grid.cpp
//  GemGame
//
//  Created by Sunil Patel on 21/10/2015.
//
//

#include "Grid.hpp"
#include "GameScene.hpp"
#include "Shaders.hpp"
#include "Constants.h"
#include "SoundManager.hpp"

#include "ui/CocosGUI.h"

#define GEM_WIGGLE              0
#define GEM_STAGGERED_ARRIVAL   1
#define GEM_ANIMATION           1
#define GEM_ANIMATION_TIME      0.1f
#define COLUMN_STAGGER_AMOUNT   0.2f
#define ROW_STAGGER_AMOUNT      0.1f
#define SINGLE_BACKWARDS        0
#define DRAW_CHAIN_PARALLEL_TO_GEM_EDGE 0

#define PI 3.1415926535

#define CENTRE_CIRCLE           1
#define OCTAGON                 2
#define MAX_OVAL                3
#define DIAMOND                 4
#define GEM_TOUCH_TYPE          DIAMOND
#define GEM_CENTRAL_SENSITIVITY 0.4f

#define DRAW_TOUCH_AREA         0

//#define GEM_CENTRAL_SENSITIVITY 2
//#define ORTHOGONAL_ONLY         0

static Vec2 startpos(-1000,-1000);
static Vec2 endpos(-1000,-1000);

static bool ChainContainsCoords(Chain *chain, int i, int j) {
	Chain *sentinel = chain;
	while (sentinel) {
		if (sentinel->i == i && sentinel->j == j) {
			// We're already here...don't add
			return true;
		}
		sentinel = sentinel->next;
	}
	return false;
}

Grid::Grid(int w, int h, float maxWidth, float maxHeight) {
    this->width = w;
    this->height = h;
	this->diagonals_allowed = true;
    
    // Fill with default constructed gems
    grid = new Gem*[w*h];
    
    init(maxWidth, maxHeight);
}
Grid::~Grid() {
    for (int i = 0; i < width * height; i++) {
        delete grid[i];
    }
    delete [] grid;
}
Gem *Grid::get(int column, int row) {
    int index = column + row * width;
    return grid[index];
}
void Grid::set(int column, int row, Gem *gem, bool init = false, GemType type = NONE) {
    int index = column + row * width;
    if (!gem)
        delete grid[index];
    grid[index] = gem;
    
    if (init) {
		if (type == NONE) {
			gem->init();
		} else {
			gem->init(type);
		}
		
        gem->sprite->setOpacity(0);
        auto fadeIn = cocos2d::FadeIn::create(.1f);
        gem->sprite->runAction(fadeIn);
        
        // Center the grid
        float ii = column - (width - 1)/2.f;
        float jj = row - (height - 1)/2.f;
		gem->sprite->setPosition(ii * Gem::getSize().width, jj * Gem::getSize().height);
        addChild(gem->sprite);
    }
}

cocos2d::Vec2 Grid::getSize() {
	Gem *gem = get(0, 0);
	if (!gem) return cocos2d::Vec2::ZERO;
	return cocos2d::Vec2(width * Gem::getSize().width, height * Gem::getSize().height);
}

bool Grid::init(float maxWidth, float maxHeight)
{
    if ( !Layer::init() )
    {
        return false;
    }
	
	selectedLayer = Layer::create();
	addChild(selectedLayer);
	
	overlay = LayerColor::create(Colours::SEMIBLACK, maxWidth, maxHeight);
	overlay->setPosition(-maxWidth/2, -maxHeight/2);
	overlay->setVisible(false);
	addChild(overlay);
	
	line = cocos2d::DrawNode::create();
	line->setPosition(cocos2d::Vec2::ZERO);
	this->addChild(line);
	
	Gem::scale = 1;
	/*auto tempSprite = LoadSprite("gems/air.png");
	auto size = tempSprite->getBoundingBox().size / Director::getInstance()->getContentScaleFactor();*/
	float ratioX = maxWidth/(Gem::getSize().width * width);
	float ratioY = maxHeight/(Gem::getSize().height * height);
	float ratio = MIN(ratioY, ratioX);
	
	Gem::scale = MIN(ratio, 1.6f);
#if DRAW_TOUCH_AREA
	auto line2 = cocos2d::DrawNode::create();
	line2->setPosition(cocos2d::Vec2::ZERO);
	this->addChild(line2);
#endif

    // Setup the gems, and them to us
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Gem *gem = new Gem;
            set(i, j, gem, true);
			
#if DRAW_TOUCH_AREA && GEM_TOUCH_TYPE == DIAMOND
			auto h = Gem::getSize().height;
			auto w = Gem::getSize().width;
			auto p = gem->sprite->getPosition();
			auto from = Vec2(0, -h/2);
			auto to = Vec2(w/2, 0);
			line2->drawSegment(from + p, to + p,
						1.0f * CC_CONTENT_SCALE_FACTOR(),
						Color4F(Colours::ORANGE));
			
			from = Vec2(w/2, 0);
			to = Vec2(0, h/2);
			line2->drawSegment(from + p, to + p,
						1.0f * CC_CONTENT_SCALE_FACTOR(),
						Color4F(Colours::ORANGE));
			
			from = Vec2(0, h/2);
			to = Vec2(-w/2, 0);
			line2->drawSegment(from + p, to + p,
						1.0f * CC_CONTENT_SCALE_FACTOR(),
						Color4F(Colours::ORANGE));
			
			from = Vec2(-w/2, 0);
			to = Vec2(0, -h/2);
			line2->drawSegment(from + p, to + p,
						1.0f * CC_CONTENT_SCALE_FACTOR(),
						Color4F(Colours::ORANGE));
#endif
        }
    }
    
    auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
    
    touchListener->onTouchBegan = CC_CALLBACK_2(Grid::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Grid::onTouchEnded, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Grid::onTouchMoved, this);
//    touchListener->onTouchCancelled = CC_CALLBACK_2(Grid::onTouchCancelled, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	
    return true;
}

void Grid::drawChain() {
	line->clear();
	
	if (chain) {
		Chain *sentinel = chain;
		while (sentinel) {
			if (sentinel->next) {
				// Draw from us to next
				float fi = sentinel->i - (width - 1)/2.f;
				float fj = sentinel->j - (height - 1)/2.f;
				float ti = sentinel->next->i - (width - 1)/2.f;
				float tj = sentinel->next->j - (height - 1)/2.f;
				auto from = Vec2(fi * Gem::getSize().width, fj * Gem::getSize().height);
				auto to = Vec2(ti * Gem::getSize().width, tj * Gem::getSize().height);
				// if the line is going diagonally, we need to make an adjustment
#if DRAW_CHAIN_PARALLEL_TO_GEM_EDGE
				if (sentinel->i != sentinel->next->i && sentinel->j != sentinel->next->j) {
					auto offset = Vec2(10, 20) * get(sentinel->i, sentinel->j)->sprite->getScale();
					if (sentinel->i > sentinel->next->i) {
						from.x -= offset.x;
						to.x += offset.x;
					} else {
						from.x += offset.x;
						to.x -= offset.x;
					}
					
					if (sentinel->j < sentinel->next->j) {
						from.y += offset.y;
						to.y -= offset.y;
					} else {
						from.y -= offset.y;
						to.y += offset.y;
					}
				}
#endif
				/*float m = (to.y - from.y) / (to.x - from.x);
				float perp = -1.0/m;
				auto p1 = from + Vec2(1, perp);
				auto p2 = from + Vec2(-1, -perp);
				auto p3 = to + Vec2(-1, -perp);
				auto p4 = to + Vec2(1, perp);
				line->drawRect(p1, p2, p3, p4, Color4F(Colours::ORANGE));*/
				line->drawSegment(from, to,
						2.0f * CC_CONTENT_SCALE_FACTOR(),
						Color4F(Colours::ORANGE));
			}
			
			sentinel = sentinel->next;
		}
	}
	
	//line->drawSolidCircle(startpos, 10, 7, 20, Color4F::RED);
	//line->drawSolidCircle(endpos, 10, 7, 20, Color4F::BLACK);
}

void Grid::drawSelected() {
	selectedLayer->removeAllChildren();
	
	if (chain) {
		Chain *sentinel = chain;
		while (sentinel) {
			// Draw from us to next
			auto gem = get(sentinel->i, sentinel->j);
			gem->sprite->setColor(Color3B::WHITE);
			auto glow = LoadSprite("gems/glow.png");
			glow->setScale(gem->sprite->getScale());
			glow->setPosition(gem->sprite->getPosition());
			selectedLayer->addChild(glow);
			sentinel = sentinel->next;
		}
	}
}

void Grid::cancelCurrentSpell() {
	if (chain) {
		Chain *sentinel = chain;
		while (sentinel) {
			Chain *current = sentinel;
			
			sentinel = current->next;
			
			delete current;
		}
	}
	currentTouch = nullptr;
	//line->clear();
}

void Grid::setActive(bool active) {
	this->active = active;
	overlay->setVisible(! active);
	overlay->setLocalZOrder(1);
}

bool Grid::isActive() {
	return this->active;
}

void Grid::castCurrentSpell() {
	if (chain) {
		bool success = Game::get()->onCastSpell(chain);
		if (success) {
			Chain *sentinel = chain;
			while (sentinel) {
				Chain *current = sentinel;
				
				sentinel = current->next;
				removeChild(get(current->i, current->j)->sprite);
				set(current->i, current->j, nullptr);
				
				delete current;
			}
		}
	}
	currentTouch = nullptr;
	
	refill();
	//line->clear();
}

void Grid::scramble() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			removeChild(get(i, j)->sprite);
			set(i, j, nullptr);
		}
	}
	
	refill();
}

int Grid::destroyGemsOfType(GemType type, Chain *chain) {
	int number = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (! ChainContainsCoords(chain, i, j)) {
				if (get(i, j)->type == type) {
					number++;
					removeChild(get(i, j)->sprite);
					set(i, j, nullptr);
				}
			}
		}
	}
	return number;
}

void Grid::makeCrystalsOverGemsOfType(GemType type, Chain *chain) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (! ChainContainsCoords(chain, i, j)) {
				if (get(i, j)->type == type) {
					removeChild(get(i, j)->sprite);
					auto g = new Gem;
					set(i, j, g, true, CRYSTAL);
				}
			}
		}
	}
}

void Grid::convertGemsOfType(GemType from, GemType to, Chain *chain) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (! ChainContainsCoords(chain, i, j)) {
				if (get(i, j)->type == from) {
					removeChild(get(i, j)->sprite);
					auto g = new Gem;
					set(i, j, g, true, to);
				}
			}
		}
	}
}

void Grid::createRandomCrystalGems(int amount, Chain *chain) {
	// create list of coords, and shuffle it.
	std::vector<std::pair<int, int> > coords;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			coords.push_back(std::pair<int, int>(i,j));
		}
	}
	std::random_shuffle(coords.begin(), coords.end());
	int i = 0;
	while (amount > 0 && i < coords.size()) {
		int x = coords[i].first;
		int y = coords[i].second;
		i++;
		// If this is in chain, skip it.
		if (! ChainContainsCoords(chain, x, y)) {
			removeChild(get(x, y)->sprite);
			auto g = new Gem;
			set(x, y, g, true, CRYSTAL);
			amount--;
		}
	}
}

void Grid::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	if (currentTouch && currentTouch->getID() == touch->getID()) {
		// check we are on the last gem
		cocos2d::Vec2 loc = touch->getLocation();
		//line->drawSolidCircle(loc - getPosition(), 2, 7, 20, Color4F::BLACK);
		endpos = loc - getPosition();
		drawChain();
		float swidth = Gem::getSize().width;
		float sheight = Gem::getSize().height;
		cocos2d::Vec2 size = getSize();
		cocos2d::Vec2 pos = getPosition();
		float left = pos.x - size.x/2, bottom = pos.y - size.y/2;
		
		auto centre = cocos2d::Vec2((chain->i + 0.5f) * swidth + left, (chain->j + 0.5f) * sheight + bottom);
		
		// You must finish close-ish to the last tile of the chain
		if ((loc - centre).length() < Gem::getSize().width) {
			castCurrentSpell();
		} else {
			cancelCurrentSpell();
		}
		chain = nullptr;
	}
	drawSelected();
	line->clear();
}

bool Grid::isAboveLine(Vec2 start, Vec2 end, Vec2 point) {
	return ((end.x - start.x)*(point.y - start.y) - (end.y - start.y)*(point.x - start.x)) > 0;
}

bool Grid::doesLineIntersectCell(Vec2 start, Vec2 end, int column, int row) {
	Vec2 size = getSize();
	Vec2 pos = getPosition();
	float left = pos.x - size.x/2, bottom = pos.y - size.y/2;
	
	// Check if all four corners of the cell are on the same side of the line.
	float swidth = Gem::getSize().width;
	float sheight = Gem::getSize().height;
	// centre of cell.
	auto centre = cocos2d::Vec2((column + 0.5f) * swidth + left, (row + 0.5f) * sheight + bottom);
	Vec2 topleft  = centre + Vec2(-swidth/2, -sheight/2);
	Vec2 topright = centre + Vec2( swidth/2, -sheight/2);
	Vec2 btmleft  = centre + Vec2(-swidth/2,  sheight/2);
	Vec2 btmright = centre + Vec2( swidth/2,  sheight/2);
	
	// at least one of the points has to be on the other side of the line to intersect.
	auto match = isAboveLine(start, end, topleft);
	return !(match == isAboveLine(start, end, topright) && match == isAboveLine(start, end, btmleft) && match == isAboveLine(start, end, btmright));
}

void Grid::onTouchMovePart(Vec2 loc) {
	Vec2 size = getSize();
	Vec2 pos = getPosition();
	float left = pos.x - size.x/2, bottom = pos.y - size.y/2;
	
	// Is it in the grid?
	if (   loc.x > left
		&& loc.x < pos.x + size.x/2
		&& loc.y > bottom
		&& loc.y < pos.y + size.y/2) {
		
		float swidth = Gem::getSize().width;
		float sheight = Gem::getSize().height;
		
		int column = (int) ((loc.x - left)/swidth);
		int row = (int) ((loc.y - bottom)/sheight);
		
		// Check that we are central to gem
		// make sure to add 1/2 onto the coords as we need gem-centre.
		auto centre = cocos2d::Vec2((column + 0.5f) * swidth + left, (row + 0.5f) * sheight + bottom);
		//			bool central = (centre - loc).length() < Gem::getWidth() * GEM_CENTRAL_SENSITIVITY;
		
		bool ongem = true;
		
#if GEM_TOUCH_TYPE == CENTRE_CIRCLE
		// METHOD 1 = central hitbox
		ongem = (centre - loc).length() < GEM_CENTRAL_SENSITIVITY * Gem::getSize().width;
#elif GEM_TOUCH_TYPE == OCTAGON
		// METHOD 2 = octagon hitbox
		for (int I=-1; I<=1; I+=2) {
			for (int J=-1; J<=1; J+=2) {
				auto corner = centre + Vec2(I * Gem::getWidth()/2, J * Gem::getHeight()/2);
				auto distance = (corner - loc).length();
				if (distance < Gem::getWidth() / 3.0f) {
					// too close!
					ongem = false;
					goto done;
				}
			}
		}
		done:;
#elif GEM_TOUCH_TYPE == MAX_OVAL
		// METHOD 3 = ellipse of maximum size
		// Assume centred on (0,0)
		auto p = loc - centre;
		auto a = Gem::getSize().width;
		auto b = Gem::getSize().height;
		
		auto k = (p.x / a) * (p.x / a) + (p.y / b) * (p.y / b);
		// Equation of ellipse is:
		//   (x/Gem::getSize().width)^2 + (y/Gem::getSize().height)^2 = 1
		ongem = k <= 1;
#elif GEM_TOUCH_TYPE == DIAMOND
		// METHOD 4 = diamond
		
		// Assume centred on (0,0)
		auto p = loc - centre;
		auto a = Gem::getSize().width/2.0;
		auto b = Gem::getSize().height/2.0;
		auto below = 0;
		
		// Check that the point is below/above each of the four edges
		// Top-right:
		auto m1 = -b/a;
		if (p.y <= m1 * p.x + b)
			below++;
		
		// Bottom-right
		auto m2 = b/a;
		if (p.y >= m2 * p.x - b)
			below++;
		
		// Bottom-left
		if (p.y >= m1 * p.x - b)
			below++;
		
		// Top-left
		if (p.y <= m2 * p.x + b)
			below++;
		
		ongem = below == 4;
#endif
		
		if (ongem) {
			// Check this isn't already in the chain!
			Chain *sentinel = chain;
			Chain *unique = nullptr;
			while (sentinel) {
				if (sentinel->i == column && sentinel->j == row) {
					// We're already here...don't add
					unique = sentinel;
					break;
				}
				sentinel = sentinel->next;
			}
			if (!unique) {
				// Check we are actually going adjacent!
				bool allowed = true;
				if (!diagonals_allowed) {
					if (abs(chain->i - column) == 1 && abs(chain->j - row) == 1) {
						allowed = false;
					}
				}
				if (abs(chain->i - column) <= 1 && abs(chain->j - row) <= 1 && allowed) {
					Chain *oldChain = chain;
					chain = new Chain;
					chain->next = oldChain;
					chain->i = column;
					chain->j = row;
					chain->type = get(column, row)->type;
					
					// draw line from previous to current
					SoundManager::get()->playEffect(kSoundEffect_SelectGem);
					drawChain();
					drawSelected();
				}
			} else {
				// If it's anywhere in the chain (except the first) revert to that point
				if (unique != chain) {
					
					// Only allow going backwards?
#if SINGLE_BACKWARDS
					if (unique == chain->next) {
#endif
						Chain *oldChain = chain;
						while (oldChain != unique) {
							Chain *temp = oldChain;
							oldChain = oldChain->next;
							delete temp;
						}
						chain = unique;
						
						//redraw line...
						SoundManager::get()->playEffect(kSoundEffect_DeselectGem);
						drawChain();
						drawSelected();
#if SINGLE_BACKWARDS
					}
#endif
				}
			}
		}
	} else {
		//cancelCurrentSpell();
	}
}

void Grid::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	if (currentTouch && currentTouch->getID() == touch->getID()) {
		Vec2 loc = touch->getLocation();
		onTouchMovePart(loc);
	}
}

bool Grid::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event) {
	if (!active) return false;
	
    Vec2 loc = touch->getLocation();
    Vec2 size = getSize();
    Vec2 pos = getPosition();
    
    float left = pos.x - size.x/2, bottom = pos.y - size.y/2;
	
	/*if (loc.y < pos.y + size.y/2)
	 line->drawSolidCircle(loc - pos, 10, 7, 20, Color4F::RED);*/
    
    // Is it in the grid?
    if (   loc.x > left
        && loc.x < pos.x + size.x/2
        && loc.y > bottom
        && loc.y < pos.y + size.y/2) {
		
		float swidth = Gem::getSize().width;
		float sheight = Gem::getSize().height;
		
        int column = (int) ((loc.x - left)/swidth);
        int row = (int) ((loc.y - bottom)/sheight);
		
		SoundManager::get()->playEffect(kSoundEffect_SelectGem);
		currentTouch = touch;
		chain = new Chain;
		chain->next = nullptr;
		chain->i = column;
		chain->j = row;
		chain->type = get(column, row)->type;
		line->clear();
		
		startpos = loc - pos;
		endpos = Vec2(-1000,-1000);
		drawSelected();
        return true;
	}
#if GEM_WIGGLE
	else {
		// shuffle all gems a bit
		for (int i = 0; i < width; i++) {
			for (int j = height - 1; j >= 0; j--) {
				Gem *g = get(i, j);
				g->sprite->setPosition(g->sprite->getPosition() + cocos2d::Vec2(-0.5f+(rand()%60) / 60.f, -0.5f+(rand()%60) / 60.f));
			}
		}
	}
#endif
	drawSelected();
    return false;
}

void Grid::refill() {
	// This could be done in a single loop, but for some reason I'm doing three loops!
	
    for (int i = 0; i < width; i++) {
        for (int j = height - 1; j >= 0; j--) {
            if (get(i, j) == nullptr) {
                // All gems above this get moved down by one.
                for (int jj = j+1; jj < height; jj++) {
                    Gem *above = get(i, jj);
                    set(i, jj - 1, above);
                    if (above) {
                        above->downBy += 1;
                    }
                }
                auto g = new Gem;
                set(i, height - 1, g);
            }
        }
    }
	
	// Animate gems downwards
	int maxdownby = 0;
    for (int i = 0; i < width; i++) {
        for (int j = height - 1; j >= 0; j--) {
            Gem *g = get(i, j);
            if (g->downBy && g->didInit) {
				if (g->downBy > maxdownby) {
					maxdownby = g->downBy;
				}
#if GEM_ANIMATION
				auto moveby = cocos2d::MoveBy::create(g->downBy * GEM_ANIMATION_TIME, cocos2d::Vec2(0, g->downBy * -Gem::getSize().height));
                g->sprite->runAction(moveby);
#else
				g->sprite->setPosition(g->sprite->getPosition() + cocos2d::Vec2(0, g->downBy * -Gem::getHeight()));
#endif
                g->downBy = 0;
            }
        }
    }
	float column_stagger = -COLUMN_STAGGER_AMOUNT;
	for (int i = 0; i < width; i++) {
		int colmaxdown = 0;
		bool column_stagger_done = false;
		for (int j = 0; j < height; j++) {
			Gem *g = get(i, j);
			if (!g->didInit) {
				g->init();
				if (!column_stagger_done) {
					column_stagger_done = true;
					column_stagger += COLUMN_STAGGER_AMOUNT;
				}
#if GEM_ANIMATION
				g->sprite->setOpacity(0);
				float ii = i - (width - 1)/2.f;
				float jj = height - (height - 1)/2.f;
				g->sprite->setPosition(ii * Gem::getSize().width, jj * Gem::getSize().height);
				int dj = height - j;
				if (!colmaxdown) {
					colmaxdown = g->downBy;
				}
				float staggered = 0;
#if	GEM_STAGGERED_ARRIVAL
				staggered = ROW_STAGGER_AMOUNT;
#endif
				auto delay = cocos2d::DelayTime::create((1 + colmaxdown - g->downBy - staggered + column_stagger) * (GEM_ANIMATION_TIME + staggered));
				auto fadeIn = cocos2d::FadeIn::create(.1f);
				auto moveby = cocos2d::MoveBy::create(dj * GEM_ANIMATION_TIME, cocos2d::Vec2(0, dj * -Gem::getSize().height));
				auto spawn = cocos2d::Spawn::create(fadeIn, moveby, nullptr);
				auto sequence = cocos2d::Sequence::create(delay, spawn, nullptr);
				g->sprite->runAction(sequence);
#else
				float ii = i - (width - 1)/2.f;
				float jj = j - (height - 1)/2.f;
				g->sprite->setPosition(ii * Gem::getWidth(), jj * Gem::getHeight());
#endif
				g->downBy = 0;
				addChild(g->sprite);
			}
		}
	}
}
