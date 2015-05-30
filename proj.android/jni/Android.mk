LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/Utils.cpp \
                   ../../Classes/GameInfo.cpp \
                   ../../Classes/GameScene.cpp \
                   ../../Classes/GameInterface.cpp \
                   ../../Classes/GameField.cpp \
                   ../../Classes/GameplayObjects.cpp \
                   ../../Classes/GameplayObjectsWidgets.cpp \
                   ../../Classes/PathSector.cpp \
                   ../../Classes/PathSectorWidget.cpp \
                   ../../Classes/Hero.cpp \
                   ../../Classes/HeroAction.cpp \
                   ../../Classes/HeroWidget.cpp \
                   ../../Classes/HeroControlKeyboard.cpp \
                   ../../Classes/HeroControlTouch.cpp \
                   ../../Classes/StaminaBarWidget.cpp \
                   ../../Classes/EffectsLayer.cpp \
                   ../../Classes/EffectEnemyDeath.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../cocos2d/scripting/lua-bindings/manual \
                    $(LOCAL_PATH)/../../cocos2d/external/lua/tolua

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES := cocos2d_lua_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,scripting/lua-bindings/proj.android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
