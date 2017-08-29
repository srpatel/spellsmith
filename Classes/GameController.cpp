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
#include "ArenaScreen.hpp"
#include "Spellbook.hpp"
#include "Constants.h"
#include "SoundManager.hpp"
#include "Tutorial.hpp"

#include "ui/CocosGUI.h"

// Nav bar and back button
#define kDepthFader 200
#define kDepthPopup 100
#define kDepthNavBar 50
#define kDepthBackButton 51
#define kDepthScene 0

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
	
	stateScreens[kStateArena] = ArenaScreen::create();
	stateScreens[kStateArena]->retain();
	
	instance->state = kStateMainMenu;
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(Colours::BACKGROUND));
	grad->setPosition(Director::getInstance()->getVisibleOrigin());
	grad->setContentSize(Director::getInstance()->getVisibleSize());
	root->addChild(grad);
	
	instance->bar = NavigationBar::create();
	instance->bar->setPosition(Vec2(0, -NavigationBar::HEIGHT));
	root->addChild(instance->bar, kDepthNavBar);
	
	auto size = Director::getInstance()->getVisibleSize();
	auto button = LoadSprite("ui/buttonback.png");
	auto onClick = EventListenerTouchOneByOne::create();
	onClick->setSwallowTouches(true);
	onClick->onTouchBegan = [size, button](Touch *touch, Event *event) -> bool {
		auto bounds = Rect(
			0, size.height - button->getContentSize().height - 20,
			button->getContentSize().width + 20, button->getContentSize().height + 20
		);
		if (bounds.containsPoint(touch->getLocation())){
			SoundManager::get()->playEffect( kSoundEffect_UISelect );
			GameController::get()->setState(kStateMap);
			return true;
		}
		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(onClick, button);
	
	instance->button = button;
	instance->button->setPosition(Vec2(button->getContentSize().width/2 + 10, size.height + button->getContentSize().height/2 + 10));
	root->addChild(instance->button, kDepthBackButton);
	
	auto fader = LayerColor::create();
	fader->initWithColor(Color4B::BLACK);
	fader->setContentSize(size);
	fader->setOpacity(0);
	root->addChild(fader, kDepthFader);
	instance->fader = fader;
	
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto layer = stateScreens[kStateMainMenu];
	layer->setPosition(origin.x, origin.y);
	root->addChild(layer, kDepthScene);
}

GameController *GameController::get() {
	return instance;
}

void GameController::showButton(bool show) {
	auto size = Director::getInstance()->getVisibleSize();
	button->stopAllActions();
	button->runAction(MoveTo::create(0.2f, Vec2(
		button->getContentSize().width/2 + 10,
		size.height + (show ? -1 : 1) * (button->getContentSize().height/2 + 10))));
}

void GameController::setState(State newstate) {
	if (newstate == this->state)
		return;
	
	// Do animations I guess
	auto origin = Director::getInstance()->getVisibleOrigin();
	
	auto prevLayer = stateScreens[this->state];
	
	auto layer = stateScreens[newstate];
	this->state = newstate;
	
	layer->setPosition(origin.x, origin.y);
	
	// Show the fader!
	fader->runAction(Sequence::create(
		FadeIn::create(0.2f),
		CallFunc::create([this, prevLayer, layer, newstate](){
			root->removeChild(prevLayer);
			root->addChild(layer, kDepthScene);
		
			// Navigation bar
			bool showBar = newstate == kStateMap;
			bar->stopAllActions();
			bar->resetButtons();
			bar->runAction(MoveTo::create(0.2f, Vec2(0, showBar ? 0 : -NavigationBar::HEIGHT)));
		
			// Back button
			showButton(newstate == kStateSpellbook || newstate == kStateArena);
		
			// tutorials
			if (newstate == kStateMap) {
				Tutorial::activate(11);
			} else if (newstate == kStateSpellbook) {
				Tutorial::activate(7);
				Tutorial::activate(201);
			}
		}),
		FadeOut::create(0.2f),
		nullptr));
	
	// do refreshing
	if (newstate == kStateMap) {
		((MapScreen *)layer)->refreshNodes();
	} else if (newstate == kStateSpellbook) {
		((Spellbook *)layer)->refreshEquips();
		((Spellbook *)layer)->refreshSpells();
	}
	
	SoundManager::get()->loader_game(newstate == kStateGame);
}
Layer *GameController::getScreen(State state) {
	if (state == kStateCount) state = this->state;
	return stateScreens[state];
}
void GameController::startArena() {
	Game::get()->startArena();
	setState(kStateGame);
}
void GameController::startRound(RoundDef *round) {
	popDialog();
	Game::get()->startRound(round);
	setState(kStateGame);
}
/*void GameController::enableBar(bool enable) {
	// Place something over the nav bar (like the grid)
	bar->setEnabled(enable);
}*/
void GameController::popDialog() {
	if (dialog_stack.size()) {
		Dialog *last_dialog = dialog_stack[dialog_stack.size() - 1];
		last_dialog->runAction(Sequence::create(
			FadeOut::create(0.1f),
			RemoveSelf::create(),
			nullptr
		));
		//root->removeChild(last_dialog);
		dialog_stack.pop_back();
	} else {
		// shouldn't get here!
	}
}
void GameController::pushDialog(Dialog *dialog) {
	dialog->setPosition(root->getContentSize()/2);
	root->addChild(dialog, kDepthPopup);
	dialog->setOpacity(0);
	dialog->setCascadeOpacityEnabled(true);
	dialog_stack.push_back(dialog);
	dialog->runAction(FadeIn::create(0.1f));
}
void GameController::showSpellInfoDialog(Spell *spell) {
	Dialog *dialog = SpellInfoDialog::create(spell);
	pushDialog(dialog);
	Tutorial::activate(8);
}
void GameController::showOptionsDialog() {
	Dialog *dialog = OptionsDialog::create();
	pushDialog(dialog);
	dialog_stack.push_back(dialog);
}
void GameController::showPreLevelDialog(RoundDef *round) {
	Dialog *dialog = PreLevelDialog::create(round);
	pushDialog(dialog);
	dialog_stack.push_back(dialog);
}
