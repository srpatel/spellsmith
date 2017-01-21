//
//  GameController.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "NavigationBar.hpp"
#include "GameController.hpp"
#include "MainMenu.hpp"
#include "GameScene.hpp"
#include "MapScreen.hpp"
#include "Spellbook.hpp"
#include "Constants.h"
#include "SoundManager.hpp"

GameController *GameController::instance = nullptr;

static Layer **stateScreens;

void GameController::init(Scene *root) {
	// instance must be null here
	instance = new GameController;
	instance->root = root;
	
	stateScreens = (Layer **) malloc(kStateCount * sizeof(Layer *));
	
	stateScreens[kStateMainMenu] = MainMenu::create();
	stateScreens[kStateMainMenu]->retain();
	
	stateScreens[kStateGame] = Game::create();
	stateScreens[kStateGame]->retain();
	
	stateScreens[kStateMap] = MapScreen::create();
	stateScreens[kStateMap]->retain();
	
	stateScreens[kStateSpellbook] = Spellbook::create();
	stateScreens[kStateSpellbook]->retain();
	
	instance->state = kStateMainMenu;
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(Colours::BACKGROUND));
	grad->setPosition(Director::getInstance()->getVisibleOrigin());
	grad->setContentSize(Director::getInstance()->getVisibleSize());
	root->addChild(grad);
	
	instance->bar = NavigationBar::create();
	instance->bar->retain();
	
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto layer = stateScreens[kStateMainMenu];
	layer->setPosition(origin.x, origin.y);
	root->addChild(layer);
}

GameController *GameController::get() {
	return instance;
}

void GameController::setState(State newstate) {
	// Do animations I guess
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto size = Director::getInstance()->getVisibleSize();
	
	auto prevLayer = stateScreens[this->state];
	root->removeChild(prevLayer);
	
	auto layer = stateScreens[newstate];
	this->state = newstate;
	
	layer->setPosition(origin.x, origin.y);
	root->addChild(layer);

	// Show or hide the navigation bar as appropriate
	if (bar->getParent())
		bar->removeFromParent();
	
	bool showBar = newstate == kStateMap || newstate == kStateSpellbook;
	if (showBar) {
		bar->setPosition(0, 0);
		root->addChild(bar);
	}
	
	// do refreshing
	if (newstate == kStateMap) {
		((MapScreen *)layer)->refreshNodes();
	} else if (newstate == kStateSpellbook) {
		((Spellbook *)layer)->refreshSpells();
	}
	
	SoundManager::get()->loader_game(newstate == kStateGame);
}
void GameController::startArena() {
	Game::get()->startArena();
	setState(kStateGame);
}
void GameController::startRound(RoundDef *round) {
	Game::get()->startRound(round);
	setState(kStateGame);
}
void GameController::popDialog() {
	if (dialog_stack.size()) {
		Dialog *last_dialog = dialog_stack[dialog_stack.size() - 1];
		root->removeChild(last_dialog);
		dialog_stack.pop_back();
	} else {
		// shouldn't get here!
	}
}
void GameController::showSpellInfoDialog(Spell *spell) {
	Dialog *dialog = SpellInfoDialog::create(spell);
	dialog->setPosition(root->getContentSize()/2);
	root->addChild(dialog);
	dialog_stack.push_back(dialog);
}
void GameController::showOptionsDialog() {
	Dialog *dialog = OptionsDialog::create();
	dialog->setPosition(root->getContentSize()/2);
	root->addChild(dialog);
	dialog_stack.push_back(dialog);
}
void GameController::showPreLevelDialog(RoundDef *round) {
	Dialog *dialog = PreLevelDialog::create(round);
	dialog->setPosition(root->getContentSize()/2);
	root->addChild(dialog);
	dialog_stack.push_back(dialog);
}
