#include "ImageManager.hpp"

ImageManager *ImageManager::instance = nullptr;

ImageManager *ImageManager::get() {
	if (instance == nullptr) {
		instance = new ImageManager;
	}
	return instance;
}

void ImageManager::init(spAtlas* atlas) {
	this->atlas = atlas;
}
