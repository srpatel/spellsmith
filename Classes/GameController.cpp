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
#include "OptionsScreen.hpp"
#include "Spellbook.hpp"
#include "Constants.h"
#include "SoundManager.hpp"
#include "Tutorial.hpp"
#include "SaveData.hpp"

#include "ui/CocosGUI.h"

// Nav bar and back button
#define kDepthFader 100
#define kDepthPopup 200
#define kDepthNavBar 101
#define kDepthBackButton 51
#define kDepthScene 0

#define BACK_BUTTON_LEFT 1

GameController *GameController::instance = nullptr;

static Screen **stateScreens;

void GameController::init(Scene *root) {
	// instance must be null here
	instance = new GameController;
	instance->root = root;
	
	stateScreens = (Screen **) malloc(kStateCount * sizeof(Screen *));
	
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
	
	stateScreens[kStateOptions] = OptionsScreen::create();
	stateScreens[kStateOptions]->retain();
	
	instance->state = kStateMap;
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(Colours::BACKGROUND));
	grad->setPosition(Director::getInstance()->getVisibleOrigin());
	grad->setContentSize(Director::getInstance()->getVisibleSize());
	root->addChild(grad);
	
	instance->bar = NavigationBar::create();
	instance->bar->setPosition(Vec2(0, 0));
	root->addChild(instance->bar, kDepthNavBar);
	
	auto size = Director::getInstance()->getVisibleSize();
	auto button = LoadSprite("ui/buttonback.png");
	auto onClick = EventListenerTouchOneByOne::create();
	button->setPosition(Vec2(
#if BACK_BUTTON_LEFT
		(button->getContentSize().width/2 + 10),
#else
		Game::get()->getBoundingBox().size.width - (button->getContentSize().width/2 + 10),
#endif
		size.height + button->getContentSize().height/2 + 10));
	onClick->setSwallowTouches(true);
	onClick->onTouchBegan = [size, button](Touch *touch, Event *event) -> bool {
		auto bounds = Rect(
			button->getPosition().x - button->getContentSize().width/2 - 10,
			button->getPosition().y - button->getContentSize().height/2 - 10,
			button->getContentSize().width + 20,
			button->getContentSize().height + 20
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
	root->addChild(instance->button, kDepthBackButton);
	
	auto fader = LayerColor::create();
	fader->initWithColor(Color4B::BLACK);
	fader->setContentSize(size);
	fader->setOpacity(0);
	root->addChild(fader, kDepthFader);
	instance->fader = fader;
	
	auto origin = Director::getInstance()->getVisibleOrigin();
	auto layer = stateScreens[instance->state];
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
		button->getPosition().x,
		size.height + (show ? -1 : 1) * (button->getContentSize().height/2 + 10))));
}

void GameController::showBar(bool showBar) {
	bar->stopAllActions();
	bar->resetButtons();
	bar->runAction(MoveTo::create(0.2f, Vec2(0, showBar ? 0 : -NavigationBar::HEIGHT)));
}

void GameController::setState(State newstate) {
	// Do animations I guess
	auto origin = Director::getInstance()->getVisibleOrigin();
	
	auto prevLayer = stateScreens[this->state];
	
	auto layer = stateScreens[newstate];
	
	layer->setPosition(origin.x, origin.y);
	
	if (newstate != this->state) {
		this->state = newstate;
		// Show the fader!
		fader->runAction(Sequence::create(
			FadeIn::create(0.2f),
			CallFunc::create([this, prevLayer, layer, newstate](){
				root->removeChild(prevLayer);
				root->addChild(layer, kDepthScene);
			
				// Navigation bar
				bool show = newstate != kStateGame;//newstate != kStateSpellbook && newstate != kStateGame;
				showBar(show);
			
				// Back button
				//showButton(newstate == kStateSpellbook);
				showButton(false);
			
				// tutorials
				if (newstate == kStateMap) {
					Tutorial::activate(11);
				} else if (newstate == kStateSpellbook) {
					if(! Tutorial::activate(7)) {
						Tutorial::activate(201);
					}
				}
			}),
			FadeOut::create(0.2f),
			nullptr));
	}
	
	// do refreshing
	prevLayer->onDeselect();
	layer->onSelect();
	
	SoundManager::get()->loader_game(newstate == kStateGame);
}
Screen *GameController::getScreen(State state) {
	if (state == kStateCount) state = this->state;
	return stateScreens[state];
}
void GameController::startArena(std::string state) {
	setState(kStateGame);
	Game::get()->startArena(state);
}
void GameController::startRound(RoundDef *round) {
	popDialog();
	setState(kStateGame);
	Game::get()->startRound(round);
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
