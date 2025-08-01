#include "Geode/loader/Log.hpp"
#include <Geode/Bindings.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <cocos2d.h>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

using namespace geode::prelude;

static bool g_isInLevel = false;

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        // Gameplay is starting
        g_isInLevel = true;
        return true;
    }
    void onExit() {
        g_isInLevel = false;
        PlayLayer::onExit();
    }
};

class BlackoutLayer : public cocos2d::CCLayerColor {
    float _duration;
public:
    static BlackoutLayer* createWithDuration(float duration) {
        cocos2d::ccColor4B color = Mod::get()->getSettingValue<cocos2d::ccColor4B>("color");
        auto layer = new BlackoutLayer();
        if (layer && layer->initWithColor(color)) {
            layer->_duration = duration;
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
    struct Fields { };

    bool pushButton(PlayerButton button) {
        // Always call the original tap logic first
        PlayerObject::pushButton(button);

        // Only active during levels
        if (!g_isInLevel) return true;

        float dur = Mod::get()->getSettingValue<float>("duration");
        auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
        if (scene) {
            auto black = BlackoutLayer::createWithDuration(dur);
            if (black) {
                black->setContentSize(cocos2d::CCDirector::sharedDirector()->getWinSize());
                scene->addChild(black, INT_MAX);
            }
        }
        return true;
    }
};
