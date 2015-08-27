-- gui.lua
-- SlashingTrough
--
-- Created by Eugene Shcherbakov on 01/04/15.
--

require "Cocos2d"
require "Cocos2dConstants"
require "GuiConstants"

PlayerResultGoldPoints = 0
PlayerResultKillPoints = 0
PlayerBestResultGoldPoints = 0
PlayerBestResultKillPoints = 0
PlayerTotalGoldPoints = 123456
PlayerTotalDamagePoints = 345
StartScreenSceneName = "StartScreenScene"
ResultScreenSceneName = "ResultScreenScene"

local StartRunEventName = "StartButtonPressed"
local MoveToHomeEventName = "MoveToHomeScreen"
local MoveToStoreEventName = "MoveToStore"

local function StratchingBounceEffect()
	local stratchOut = cc.ScaleBy:create(0.3, 1.4, 0.8, 1.0)
	local stratchIn = cc.ScaleTo:create(0.55, 1.0, 1.0, 1.0)
	local delay = cc.DelayTime:create(0.5)
	local stratchOutEase = cc.EaseSineOut:create(stratchOut)
	local stratchInEase = cc.EaseBounceOut:create(stratchIn)
	local effect = cc.Sequence:create(delay, stratchOutEase, stratchInEase, nil)
	return effect
end

local function AppearBouncedWithDelay(delay, finalScale)
	local upscale = cc.ScaleTo:create(0.8, finalScale)
	local bounce = cc.EaseBounceOut:create(upscale)
	local fadein = cc.FadeIn:create(0.2)
	local wait = cc.DelayTime:create(delay)
	local motion = cc.Spawn:create(bounce, fadein)
	local effect = cc.Sequence:create(wait, motion)
	return effect
end
