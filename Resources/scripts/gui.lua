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

function CreateResultScene()
	local scene = cc.Scene:create()
	local director = cc.Director:getInstance()
	local frameSize = director:getVisibleSize()
	local frameOrigin = director:getVisibleOrigin()
	local allowToContinue = false

	local center = {x = 0.0, y = 0.0}
	center.x = frameOrigin.x + frameSize.width * 0.5
	center.y = frameOrigin.y + frameSize.height * 0.5

	local function onHomePressed(touch, event)
		if event == ccui.TouchEventType.ended then
			local event = cc.EventCustom:new(MoveToHomeEventName)
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end

	local function onKeyReleased(key, event)
		if key == cc.KeyCode.KEY_SPACE then
			local event = cc.EventCustom:new(StartRunEventName)
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end
	local keyboardListener = cc.EventListenerKeyboard:create()
	keyboardListener:registerScriptHandler(onKeyReleased, cc.Handler.EVENT_KEYBOARD_RELEASED)
	director:getEventDispatcher():addEventListenerWithSceneGraphPriority(keyboardListener, scene)

	local background = cc.LayerColor:create(cc.c4b(255, 255, 255, 255))

	local panelYShift = -200.0
	local scorePanel = ccui.Layout:create()
	scorePanel:setBackGroundImage("ui/ui_frame_best-score_blank.png")
	scorePanel:setScale(1.8)
	scorePanel:setPositionX(frameSize.width - scorePanel:getBackGroundImageTextureSize().width)
	scorePanel:setPositionY(frameSize.height - scorePanel:getBackGroundImageTextureSize().height + panelYShift)

	local scoreTitle = ccui.Text:create("SCORE:", "font_prototype.ttf", 25)
	scoreTitle:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextColor(cc.c4b(0, 0, 0, 255))
	scoreTitle:setPositionX(20.0)
	scoreTitle:setPositionY(scorePanel:getBackGroundImageTextureSize().height * 0.5 + scoreTitle:getContentSize().height * 0.5)

	local goldPointsText = ccui.Text:create("", "font_prototype.ttf", 28)
	goldPointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	goldPointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	goldPointsText:setTextColor(cc.c4b(0, 0, 0, 255))
	goldPointsText:setPositionX(30.0)
	goldPointsText:setPositionY(35.0)
	goldPointsText:setString(tostring(PlayerBestResultGoldPoints))

	local killPointsText = ccui.Text:create("", "font_prototype.ttf", 28)
	killPointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	killPointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	killPointsText:setTextColor(cc.c4b(0, 0, 0, 255))
	killPointsText:setPositionX(30.0)
	killPointsText:setPositionY(-8.0)
	killPointsText:setString(tostring(PlayerBestResultKillPoints))

	local iconGold = cc.Sprite:create("icons/icon_coin.png")
	iconGold:setPositionX(-40.0)
	iconGold:setPositionY(35.0)

	local iconKill = cc.Sprite:create("icons/icon_kills.png")
	iconKill:setPositionX(-40.0)
	iconKill:setPositionY(-8.0)

	scorePanel:addChild(scoreTitle)
	scorePanel:addChild(goldPointsText)
	scorePanel:addChild(killPointsText)
	scorePanel:addChild(iconGold)
	scorePanel:addChild(iconKill)

	local barbarianImage = cc.Sprite:create("ui/ui_endscreen_bg_dude.png")
	barbarianImage:setScale(1.8)
	barbarianImage:setAnchorPoint(cc.p(0.0, 0.5))
	barbarianImage:setPositionY(frameSize.height * 0.5)
	local move0 = cc.MoveBy:create(6.0, cc.p(0.0, -20.0))
	local move1 = cc.MoveBy:create(6.0, cc.p(-20.0, 0.0))
	local move2 = cc.MoveBy:create(6.0, cc.p(0.0, 20))
	local move3 = cc.MoveBy:create(6.0, cc.p(20.0, 0.0))
	local moveEase0 = cc.EaseSineInOut:create(move0)
	local moveEase1 = cc.EaseSineInOut:create(move1)
	local moveEase2 = cc.EaseSineInOut:create(move2)
	local moveEase3 = cc.EaseSineInOut:create(move3)
	local moveSeq = cc.Sequence:create(moveEase0, moveEase1, moveEase2, moveEase3)
	local imageEff = cc.RepeatForever:create(moveSeq)
	barbarianImage:runAction(imageEff)

	local downDecorFrame = cc.Sprite:create("ui/ui_shop_wood-frame_down.png")
	downDecorFrame:setPositionX(frameSize.width * 0.5)
	downDecorFrame:setPositionY(downDecorFrame:getContentSize().height * 0.5)

	local topDecorFrame = cc.Sprite:create("ui/ui_shop_wood-frame_top.png")
	topDecorFrame:setPositionX(frameSize.width * 0.5)
	topDecorFrame:setPositionY(frameSize.height - topDecorFrame:getContentSize().height * 0.5)

	local homeBtn = ccui.Button:create("ui/ui_btn_home.png")
	local homeBtnScale = 1.8
	homeBtn:setScale(homeBtnScale)
	homeBtn:setPositionX(homeBtn:getContentSize().width * homeBtnScale * 0.5)
	homeBtn:setPositionY(homeBtn:getContentSize().height * homeBtnScale * 0.5)
	homeBtn:setScale(0.1)
	homeBtn:setOpacity(0)
	homeBtn:addTouchEventListener(onHomePressed)
	homeBtn:runAction(AppearBouncedWithDelay(0.0, homeBtnScale))

	local function onStorePressed(touch, event)
		if event == ccui.TouchEventType.ended then
			local event = cc.EventCustom:new(MoveToStoreEventName)
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end

	local shopBtn = ccui.Button:create("ui/ui_btn_shop.png")
	local shopBtnScale = 1.8
	shopBtn:setScale(shopBtnScale)
	shopBtn:setPositionX(homeBtn:getContentSize().width * homeBtnScale + shopBtn:getContentSize().width * shopBtnScale * 0.5)
	shopBtn:setPositionY(shopBtn:getContentSize().height * shopBtnScale * 0.5)
	shopBtn:setScale(0.1)
	shopBtn:setOpacity(0)
	shopBtn:addTouchEventListener(onStorePressed)
	shopBtn:runAction(AppearBouncedWithDelay(0.2, shopBtnScale))

	local coinsShopBtn = ccui.Button:create("ui/ui_btn_coins_shop.png")
	local coinsShopBtnXShift = -200.0
	local coinsShopBtnYShift = -20
	local coinsShopBtnScale = 1.8
	coinsShopBtn:setScale(coinsShopBtnScale)
	coinsShopBtn:setPositionX(center.x + coinsShopBtnXShift)
	coinsShopBtn:setPositionY(frameSize.height - coinsShopBtn:getContentSize().height * coinsShopBtnScale * 0.5 + coinsShopBtnYShift)

	local totalGoldIcon = cc.Sprite:create("icons/icon_coin.png")
	local totalGoldIconScale = 1.8
	totalGoldIcon:setScale(totalGoldIconScale)
	totalGoldIcon:setPositionX(center.x - 120.0)
	totalGoldIcon:setPositionY(frameSize.height - totalGoldIcon:getContentSize().height * totalGoldIconScale * 0.5 - 20.0)

	local totalDamageIcon = cc.Sprite:create("icons/icon_dmg.png")
	local totalDamageIconScale = 1.8
	totalDamageIcon:setScale(totalDamageIconScale)
	totalDamageIcon:setPositionX(center.x + 170.0)
	totalDamageIcon:setPositionY(frameSize.height - totalDamageIcon:getContentSize().height * totalDamageIconScale * 0.5 - 20.0)

	local totalGoldPointsText = ccui.Text:create("", "font_prototype.ttf", 45)
	totalGoldPointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	totalGoldPointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	totalGoldPointsText:setTextColor(cc.c4b(254, 228, 146, 255))
	totalGoldPointsText:setString(tostring(PlayerTotalGoldPoints))
	totalGoldPointsText:setAnchorPoint(cc.p(0.0, 0.5))
	totalGoldPointsText:setPositionX(totalGoldIcon:getPositionX() + 
									 totalGoldIcon:getContentSize().width * totalGoldIcon:getScale() * 0.5 + 10.0)
	totalGoldPointsText:setPositionY(frameSize.height - 45.0)

	local totalDamagePointsText = ccui.Text:create("", "font_prototype.ttf", 45)
	totalDamagePointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	totalDamagePointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	totalDamagePointsText:setTextColor(cc.c4b(254, 228, 146, 255))
	totalDamagePointsText:setString(tostring(PlayerTotalDamagePoints))
	totalDamagePointsText:setAnchorPoint(cc.p(0.0, 0.5))
	totalDamagePointsText:setPositionX(totalDamageIcon:getPositionX() + 
									   totalDamageIcon:getContentSize().width * totalDamageIcon:getScale() * 0.5 + 10.0)
	totalDamagePointsText:setPositionY(frameSize.height - 45.0)

	local function tipEndCallback()
		allowToContinue = true
	end

	local imageTip = cc.Sprite:create("ui/ui_run_slash_tip.png")
	local imageTipYShift = -200.0
	local imageTipScale = 1.8
	imageTip:setScale(imageTipScale)
	imageTip:setPositionX(center.x)
	imageTip:setPositionY(frameSize.height + imageTip:getContentSize().height * imageTipScale * 0.5)
	local tipMoveDown = cc.MoveTo:create(0.9, cc.p(center.x, center.y + imageTipYShift * 1.1))
	local tipMoveEase = cc.EaseBackOut:create(tipMoveDown)
	local tipMoveEnd = cc.CallFunc:create(tipEndCallback)
	imageTip:runAction(cc.Sequence:create(tipMoveEase, tipMoveEnd))

	local function onTouchBegan(touch, event)
		return true
	end
	local function onTouchEnded(touch, event)
		if allowToContinue then
			local function sendEvent()
				local event = cc.EventCustom:new(StartRunEventName)
				local dispatcher = scene:getEventDispatcher()
				dispatcher:dispatchEvent(event)
			end
			local moveUp = cc.MoveTo:create(0.5, cc.p(center.x, frameSize.height + imageTip:getContentSize().height * imageTipScale * 0.5))
			local moveEase = cc.EaseBackIn:create(moveUp)
			local callback = cc.CallFunc:create(sendEvent)
			local effect = cc.Sequence:create(moveEase, callback)
			imageTip:runAction(effect)
		end
	end

	local listener = cc.EventListenerTouchOneByOne:create()
	listener:registerScriptHandler(onTouchBegan, cc.Handler.EVENT_TOUCH_BEGAN)
	listener:registerScriptHandler(onTouchEnded, cc.Handler.EVENT_TOUCH_ENDED)
	listener:registerScriptHandler(onTouchEnded, cc.Handler.EVENT_TOUCH_CANCELLED)
	local dispatcher = director:getEventDispatcher()
	dispatcher:addEventListenerWithSceneGraphPriority(listener, scene)
	
	scene:addChild(background, 0)
	scene:addChild(barbarianImage, 1)
	scene:addChild(scorePanel, 2)
	scene:addChild(downDecorFrame, 3)
	scene:addChild(topDecorFrame, 3)
	scene:addChild(homeBtn, 4)
	scene:addChild(shopBtn, 4)
	scene:addChild(coinsShopBtn, 4)
	scene:addChild(totalGoldIcon, 4)
	scene:addChild(totalDamageIcon, 4)
	scene:addChild(totalGoldPointsText, 4)
	scene:addChild(totalDamagePointsText, 4)
	scene:addChild(imageTip, 5)
	scene:setName(ResultScreenSceneName)

	local visualGoldPoints = 0
	local visualKillPoints = 0
	local goldCountingSpeed = 250.0
	local killCountingSpeed = 50.0

	local function update(dt)
		-- counting gold points
		if visualGoldPoints <= PlayerResultGoldPoints then
			local absdiff = math.abs(visualGoldPoints - PlayerResultGoldPoints)
			if (goldCountingSpeed * dt > absdiff) then
				visualGoldPoints = PlayerResultGoldPoints
			else
				visualGoldPoints = visualGoldPoints + goldCountingSpeed * dt
			end
			goldPointsText:setString(tostring(math.floor(visualGoldPoints)))
		end
		-- counting kill points
		if visualKillPoints <= PlayerResultKillPoints then
			local absdiff = math.abs(visualKillPoints - PlayerResultKillPoints)
			if (killCountingSpeed * dt > absdiff) then
				visualKillPoints = PlayerResultKillPoints
			else
				visualKillPoints = visualKillPoints + killCountingSpeed * dt
			end
			killPointsText:setString(tostring(math.floor(visualKillPoints)))
		end
	end

	scene:scheduleUpdateWithPriorityLua(update, 0)

	return scene
end
