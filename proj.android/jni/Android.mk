LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

PCH_FILE := $(LOCAL_PATH)/../../Classes/CommonPrefix.h
LOCAL_CPPFLAGS += -include $(PCH_FILE)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/Characters.cpp \
../../Classes/Dialogs.cpp \
../../Classes/DoSpell.cpp \
../../Classes/GameController.cpp \
../../Classes/GameHUD.cpp \
../../Classes/GameScene.cpp \
../../Classes/GameScenery.cpp \
../../Classes/Gem.cpp \
../../Classes/Grid.cpp \
../../Classes/Level.cpp \
../../Classes/MainMenu.cpp \
../../Classes/Monster.cpp \
../../Classes/Popup.cpp \
../../Classes/SaveData.cpp \
../../Classes/Shaders.cpp \
../../Classes/Spell.cpp \
../../Classes/SpellPicker.cpp \
../../Classes/SpriteLoader.cpp \
../../Classes/Strings.cpp \
../../Classes/spine/Animation.c \
../../Classes/spine/AnimationState.c \
../../Classes/spine/AnimationStateData.c \
../../Classes/spine/Atlas.c \
../../Classes/spine/AtlasAttachmentLoader.c \
../../Classes/spine/Attachment.c \
../../Classes/spine/AttachmentLoader.c \
../../Classes/spine/Bone.c \
../../Classes/spine/BoneData.c \
../../Classes/spine/BoundingBoxAttachment.c \
../../Classes/spine/Event.c \
../../Classes/spine/EventData.c \
../../Classes/spine/extension.c \
../../Classes/spine/IkConstraint.c \
../../Classes/spine/IkConstraintData.c \
../../Classes/spine/Json.c \
../../Classes/spine/MeshAttachment.c \
../../Classes/spine/PolygonBatch.cpp \
../../Classes/spine/RegionAttachment.c \
../../Classes/spine/Skeleton.c \
../../Classes/spine/SkeletonAnimation.cpp \
../../Classes/spine/SkeletonBounds.c \
../../Classes/spine/SkeletonData.c \
../../Classes/spine/SkeletonJson.c \
../../Classes/spine/SkeletonRenderer.cpp \
../../Classes/spine/Skin.c \
../../Classes/spine/SkinnedMeshAttachment.c \
../../Classes/spine/Slot.c \
../../Classes/spine/SlotData.c \
../../Classes/spine/spine-cocos2dx.cpp \


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
