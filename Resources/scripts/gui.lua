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

-- this scene must send StartButtonPressed event
function CreateStartscreenScene()
	local scene = cc.Scene:create()
	local director = cc.Director:getInstance()
	local frameSize = director:getVisibleSize()
	local frameOrigin = director:getVisibleOrigin()
	local allowToContinue = false

	-- calculate center of the screen
	local center = {x = 0.0, y = 0.0}
	center.x = frameOrigin.x + frameSize.width * 0.5
	center.y = frameOrigin.y + frameSize.height * 0.5

	local whiteBkg = cc.LayerColor:create(cc.c4b(255, 255, 255, 255))

	local imageBkg = cc.Sprite:create("ui/ui_startscreen_girl.png")
	imageBkg:setScale(1.8)
	imageBkg:setAnchorPoint(cc.p(0.0, 0.0))
	imageBkg:setPosition(cc.p(0.0, 0.0))

	local function tipEndCallback()
		allowToContinue = true
	end

	local imageTip = cc.Sprite:create("ui/ui_run_slash_tip.png")
	local imageTipYShift = -218.0
	local imageTipScale = 1.8
	imageTip:setScale(imageTipScale)
	imageTip:setPositionX(center.x)
	imageTip:setPositionY(frameSize.height + imageTip:getContentSize().height * imageTipScale * 0.5)
	local tipMoveDown = cc.MoveTo:create(0.7, cc.p(center.x, center.y + imageTipYShift * 1.1))
	local tipMoveEase = cc.EaseCubicActionOut:create(tipMoveDown)
	local tipMoveEnd = cc.CallFunc:create(tipEndCallback)
	imageTip:runAction(cc.Sequence:create(tipMoveEase, tipMoveEnd))

	local gameLogo = cc.Sprite:create("ui/ui_st_tmp-logo.png")
	gameLogo:setScale(1.8)
	gameLogo:setPositionX(center.x)
	gameLogo:setPositionY(frameSize.height - gameLogo:getContentSize().height)

	local studioLogo = cc.Sprite:create("ui/ui_blank_logo.png")
	local studioLogoScale = 1.8
	studioLogo:setScale(studioLogoScale)
	studioLogo:setPositionX(frameSize.width - studioLogo:getContentSize().width * studioLogoScale * 0.5 - 15.0)
	studioLogo:setPositionY(studioLogo:getContentSize().height * studioLogoScale * 0.5 + 15.0)

	local optionsBtn = ccui.Button:create("ui/ui_btn_options.png")
	local optionsBtnScale = 1.8
	optionsBtn:setScale(optionsBtnScale)
	optionsBtn:setPositionX(optionsBtn:getContentSize().width * optionsBtnScale * 0.5)
	optionsBtn:setPositionY(optionsBtn:getContentSize().height * optionsBtnScale * 0.5)
	optionsBtn:setScale(0.1)
	optionsBtn:setOpacity(0)
	optionsBtn:runAction(AppearBouncedWithDelay(0.0, optionsBtnScale))

	local shopBtn = ccui.Button:create("ui/ui_btn_shop.png")
	local shopBtnScale = 1.8
	shopBtn:setScale(shopBtnScale)
	shopBtn:setPositionX(optionsBtn:getContentSize().width * optionsBtnScale + shopBtn:getContentSize().width * shopBtnScale * 0.5)
	shopBtn:setPositionY(shopBtn:getContentSize().height * shopBtnScale * 0.5)
	shopBtn:setScale(0.1)
	shopBtn:setOpacity(0)
	shopBtn:runAction(AppearBouncedWithDelay(0.2, shopBtnScale))

	local panelYShift = -200.0
	local bestScorePanel = ccui.Layout:create()
	bestScorePanel:setBackGroundImage("ui/ui_frame_best-score_blank.png")
	bestScorePanel:setScale(1.8)
	bestScorePanel:setPositionX(frameSize.width - bestScorePanel:getBackGroundImageTextureSize().width)
	bestScorePanel:setPositionY(frameSize.height - bestScorePanel:getBackGroundImageTextureSize().height + panelYShift)

	local scoreTitle = ccui.Text:create("Best SCORE:", "font_prototype.ttf", 25)
	scoreTitle:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextColor(cc.c4b(0, 0, 0, 255))
	scoreTitle:setPositionX(0.0)
	scoreTitle:setPositionY(bestScorePanel:getBackGroundImageTextureSize().height * 0.5 + scoreTitle:getContentSize().height * 0.5)

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

	local iconGold = cc.Sprite:create("icon_gold.png")
	iconGold:setPositionX(-40.0)
	iconGold:setPositionY(35.0)
	iconGold:setScale(0.8)

	local iconKill = cc.Sprite:create("icon_kill.png")
	iconKill:setPositionX(-40.0)
	iconKill:setPositionY(-8.0)
	iconKill:setScale(0.8)

	bestScorePanel:addChild(scoreTitle)
	bestScorePanel:addChild(goldPointsText)
	bestScorePanel:addChild(killPointsText)
	bestScorePanel:addChild(iconGold)
	bestScorePanel:addChild(iconKill)

	local function onTouchBegan(touch, event)
		return true
	end
	local function onTouchEnded(touch, event)
		if allowToContinue then
			local event = cc.EventCustom:new("StartButtonPressed")
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end

	local listener = cc.EventListenerTouchOneByOne:create()
	listener:registerScriptHandler(onTouchBegan, cc.Handler.EVENT_TOUCH_BEGAN)
	listener:registerScriptHandler(onTouchEnded, cc.Handler.EVENT_TOUCH_ENDED)
	listener:registerScriptHandler(onTouchEnded, cc.Handler.EVENT_TOUCH_CANCELLED)
	local dispatcher = director:getEventDispatcher()
	dispatcher:addEventListenerWithSceneGraphPriority(listener, scene)

	scene:addChild(whiteBkg, 0)
	scene:addChild(imageBkg, 1)
	scene:addChild(gameLogo, 2)
	scene:addChild(studioLogo, 2)
	scene:addChild(optionsBtn, 2)
	scene:addChild(shopBtn, 2)
	scene:addChild(bestScorePanel, 3)
	scene:addChild(imageTip, 4)

	return scene
end

function CreateResultScene()
	local scene = cc.Scene:create()
	local director = cc.Director:getInstance()
	local frameSize = director:getVisibleSize()
	local frameOrigin = director:getVisibleOrigin()

	local function OnRunPressed(touch, event)
		if event == ccui.TouchEventType.ended then
			local event = cc.EventCustom:new("StartButtonPressed")
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end

	local function OnKeyReleased(key, event)
		if key == cc.KeyCode.KEY_SPACE then
			local event = cc.EventCustom:new("StartButtonPressed")
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end
	local keyboardListener = cc.EventListenerKeyboard:create()
	keyboardListener:registerScriptHandler(OnKeyReleased, cc.Handler.EVENT_KEYBOARD_RELEASED)
	director:getEventDispatcher():addEventListenerWithSceneGraphPriority(keyboardListener, scene)

	local background = cc.LayerColor:create(cc.c4b(255, 255, 255, 255))

	local runButton = ccui.Button:create("run_button.png")
	runButton:setPositionX(frameSize.width * 0.5)
	runButton:setPositionY(frameOrigin.y - runButton:getContentSize().height)
	runButton:setScale(1.8)
	runButton:addTouchEventListener(OnRunPressed)
	local btnMove = cc.MoveTo:create(0.7, cc.p(frameSize.width * 0.5, 150.0))
	local btnEase = cc.EaseCubicActionOut:create(btnMove)
	local btnWait = cc.DelayTime:create(0.5)
	runButton:runAction(cc.Sequence:create(btnWait, btnEase))

	local panelYShift = -100.0
	local panel = ccui.Layout:create()
	panel:setBackGroundImage("panel.png")
	panel:setScale(1.8)
	panel:setPositionX(frameSize.width - panel:getBackGroundImageTextureSize().width)
	panel:setPositionY(frameSize.height - panel:getBackGroundImageTextureSize().height + panelYShift)

	local scoreTitle = ccui.Text:create("SCORE:", "font_prototype.ttf", 25)
	scoreTitle:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	scoreTitle:setTextColor(cc.c4b(0, 0, 0, 255))
	scoreTitle:setPositionX(20.0)
	scoreTitle:setPositionY(panel:getBackGroundImageTextureSize().height * 0.5 + scoreTitle:getContentSize().height * 0.5)

	local goldPointsText = ccui.Text:create("", "font_prototype.ttf", 30)
	goldPointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	goldPointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	goldPointsText:setTextColor(cc.c4b(0, 0, 0, 255))
	goldPointsText:setPositionX(30.0)
	goldPointsText:setPositionY(18.0)
	goldPointsText:setString(tostring(PlayerResultGoldPoints))

	local killPointsText = ccui.Text:create("", "font_prototype.ttf", 30)
	killPointsText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	killPointsText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	killPointsText:setTextColor(cc.c4b(0, 0, 0, 255))
	killPointsText:setPositionX(30.0)
	killPointsText:setPositionY(-28.0)
	killPointsText:setString(tostring(PlayerResultKillPoints))

	local iconGold = cc.Sprite:create("icon_gold.png")
	iconGold:setPositionX(-50.0)
	iconGold:setPositionY(18.0)
	iconGold:setScale(0.9)

	local iconKill = cc.Sprite:create("icon_kill.png")
	iconKill:setPositionX(-50.0)
	iconKill:setPositionY(-28.0)
	iconKill:setScale(0.9)

	panel:addChild(scoreTitle)
	panel:addChild(goldPointsText)
	panel:addChild(killPointsText)
	panel:addChild(iconGold)
	panel:addChild(iconKill)
	local rotate0 = cc.RotateTo:create(3.0, 5.0)
	local rotate1 = cc.RotateTo:create(3.0, -5.0)
	local rotateEase0 = cc.EaseSineInOut:create(rotate0)
	local rotateEase1 = cc.EaseSineInOut:create(rotate1)
	local seq = cc.Sequence:create(rotateEase0, rotateEase1)
	local effect = cc.RepeatForever:create(seq)
	panel:runAction(effect)

	local barbarianImage = cc.Sprite:create("barbarian_sketch.jpg")
	local barbarianXShift = -100.0
	barbarianImage:setPositionX(frameSize.width * 0.5 + barbarianXShift)
	barbarianImage:setPositionY(frameSize.height * 0.5)
	barbarianImage:setScale(0.85)
	
	background:addChild(barbarianImage, 0)
	background:addChild(runButton, 1)
	background:addChild(panel, 2)

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
	scene:addChild(background)

	return scene
end
