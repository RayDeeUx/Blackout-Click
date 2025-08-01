#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class BlackoutLayer : public cocos2d::CCLayerColor {
public:
    static BlackoutLayer* createWithDuration(float duration) {
        cocos2d::ccColor4B color = Mod::get()->getSettingValue<cocos2d::ccColor4B>("color");
        auto layer = new BlackoutLayer();
        if (layer && layer->initWithColor(color)) {
            layer->scheduleOnce(
                schedule_selector(BlackoutLayer::onBlackoutFinished),
                duration
            );
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

    void onBlackoutFinished(float) {
        this->removeFromParent();
    }
};

class $modify(PlayerObject) {
    bool pushButton(PlayerButton button) {
        // Always call the original tap logic first
        bool result = PlayerObject::pushButton(button);

        // Only active during levels
        if (!PlayLayer::get()) return result;

        float dur = Mod::get()->getSettingValue<float>("duration");
        auto scene = CCScene::get();
        if (scene) {
            auto black = BlackoutLayer::createWithDuration(dur);
            if (black) {
                black->setContentSize(cocos2d::CCDirector::sharedDirector()->getWinSize());
                scene->addChild(black, INT_MAX);
            }
        }
        return result;
    }
};