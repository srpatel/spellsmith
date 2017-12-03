//
//  ClickMe.cpp
//  Gems
//
//  Created by Sunil Patel on 03/12/2017.
//

#include "ClickMe.hpp"

bool ClickMe::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	// Circles...
	auto circle1 = LoadSprite("ui/circle.png");
	circle1->setAnchorPoint(Vec2(0.5, 0.5));
	addChild(circle1);
	
//	auto circle2 = LoadSprite("ui/circle.png");
//	circle2->setAnchorPoint(Vec2(0.5, 0.5));
//	circle2->setOpacity(0);
//	addChild(circle2);
	
	circle1->runAction(
		RepeatForever::create(
			Sequence::create(
				FadeTo::create(0, 255),
				ScaleTo::create(0, 0.1),
				Spawn::create(
					FadeTo::create(1, 0),
					ScaleTo::create(1, 1),
					nullptr
				),
				DelayTime::create(0.1),
				nullptr
			)
		)
	);
	
	/*circle2->runAction(
		Sequence::create(
			DelayTime::create(0.2),
			CallFunc::create([circle2](){
				circle2->runAction(RepeatForever::create(
					Sequence::create(
						FadeTo::create(0, 255),
						ScaleTo::create(0, 0.1),
						Spawn::create(
							FadeTo::create(1, 0),
							ScaleTo::create(1, 1),
							nullptr
						),
						nullptr
					)
				));
			}),
			nullptr
		)
	);*/
	
//	circle2->runAction(
//		Sequence::create(
//			DelayTime::create(0.5),
//			RepeatForever::create(
//				Sequence::create(
//					FadeTo::create(0, 255),
//					ScaleTo::create(0, 0.1),
//					Spawn::create(
//						FadeTo::create(1, 0),
//						ScaleTo::create(1, 1),
//						nullptr
//					),
//					nullptr
//				)
//			),
//			nullptr
//		)
//	);
	
	return true;
}
