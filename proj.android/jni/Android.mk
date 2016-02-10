LOCAL_PATH := $(call \
my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/Abilities.cpp \
../../Classes/ControlKeyboard.cpp \
../../Classes/ControlTouch.cpp \
../../Classes/Cheats.cpp \
../../Classes/CheatsPanel.cpp \
../../Classes/DailyTask.cpp \
../../Classes/DailyMissions.cpp \
../../Classes/DailyMissionsPopup.cpp \
../../Classes/EffectsLayer.cpp \
../../Classes/EffectFlyingIcon.cpp \
../../Classes/EffectExplosion.cpp \
../../Classes/Enemy.cpp \
../../Classes/EnemyWidget.cpp \
../../Classes/Field.cpp \
../../Classes/FieldLayer.cpp \
../../Classes/FieldSector.cpp \
../../Classes/FieldLevel.cpp \
../../Classes/FieldSectorWidget.cpp \
../../Classes/FieldCamera.cpp \
../../Classes/GameInfo.cpp \
../../Classes/GameInterface.cpp \
../../Classes/GameScene.cpp \
../../Classes/Hero.cpp \
../../Classes/HeroAction.cpp \
../../Classes/HeroWidget.cpp \
../../Classes/ModelBase.cpp \
../../Classes/Obstacle.cpp \
../../Classes/ObstacleWidget.cpp \
../../Classes/Projectile.cpp \
../../Classes/ProjectileWidget.cpp \
../../Classes/PlayerInfo.cpp \
../../Classes/Store.cpp \
../../Classes/StartInterface.cpp \
../../Classes/StatisticsInterface.cpp \
../../Classes/MissionStartPopup.cpp \
../../Classes/MapPlayerMark.cpp \
../../Classes/MapLevelMark.cpp \
../../Classes/MapWidget.cpp \
../../Classes/MapInterface.cpp \
../../Classes/PauseMenu.cpp \
../../Classes/StoreCategoryButton.cpp \
../../Classes/StoreInterface.cpp \
../../Classes/StoreItemButton.cpp \
../../Classes/StoreItemBaseWidget.cpp \
../../Classes/StoreWeaponWidget.cpp \
../../Classes/StoreArmorWidget.cpp \
../../Classes/StoreCrystallWidget.cpp \
../../Classes/PopupCore.cpp \
../../Classes/LevelsCache.cpp \
../../Classes/ScreenChanger.cpp \
../../Classes/Utils.cpp \
../../Classes/Log.cpp \
../../Classes/VariablesSet.cpp \
../../Classes/Item.cpp \
../../Classes/VictoryCondition.cpp

LOCAL_CPPFLAGS := -DSDKBOX_ENABLED
LOCAL_LDLIBS := -landroid \
-llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_WHOLE_STATIC_LIBRARIES := PluginChartboost \
sdkbox

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginchartboost)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
