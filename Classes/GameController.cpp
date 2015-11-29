//
//  GameController.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "GameController.hpp"
#include "MainMenu.hpp"
#include "GameScene.hpp"
#include "LevelSelect.hpp"
#include "Constants.h"

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
	stateScreens[kStateLevelSelect] = LevelSelect::create();
	stateScreens[kStateLevelSelect]->retain();
	
	instance->state = kStateMainMenu;
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(Colours::BACKGROUND));
	grad->setPosition(Director::getInstance()->getVisibleOrigin());
	grad->setContentSize(Director::getInstance()->getVisibleSize());
	root->addChild(grad);
	
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
	
	auto prevLayer = stateScreens[this->state];
	root->removeChild(prevLayer);
	
	auto layer = stateScreens[newstate];
	this->state = newstate;
	
	layer->setPosition(origin.x, origin.y);
	root->addChild(layer);
}
void GameController::startGame(Level *level) {
	Game::get()->startLevel(level);
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
void GameController::showLevelEndDialog(bool victory) {
	// Create the dialog and add it to the root
	Dialog *dialog;
	if (victory) {
		dialog = LevelWonDialog::create();
	} else {
		dialog = LevelLostDialog::create();
	}
	dialog->setPosition(root->getContentSize()/2);
	root->addChild(dialog);
	dialog_stack.push_back(dialog);
}