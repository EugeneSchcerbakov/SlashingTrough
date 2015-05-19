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

local function StratchingBounceEffect()
	local stratchOut = cc.ScaleBy:create(0.3, 1.4, 0.8, 1.0)
	local stratchIn = cc.ScaleTo:create(0.55, 1.0, 1.0, 1.0)
	local delay = cc.DelayTime:create(0.5)
	local stratchOutEase = cc.EaseSineOut:create(stratchOut)
	local stratchInEase = cc.EaseBounceOut:create(stratchIn)
	local effect = cc.Sequence:create(delay, stratchOutEase, stratchInEase, nil)
	return effect
end

-- this scene must send StartButtonPressed event
function CreateStartScene()
	local scene = cc.Scene:create()
	local director = cc.Director:getInstance()
	local frameSize = director:getVisibleSize()
	local frameOrigin = director:getVisibleOrigin()

	-- calculate center of the screen
	local center = {x = 0.0, y = 0.0}
	center.x = frameOrigin.x + frameSize.width * 0.5
	center.y = frameOrigin.y + frameSize.height * 0.5

	local buttonNormalColor = cc.c4f(1.0, 1.0, 1.0, 1.0)
	local buttonPressedColor = cc.c4f(0.7, 0.7, 0.7, 1.0)

	local title = ccui.Text:create("Slashing\nTrough", "font_marigoldwild.ttf", 200)
	title:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	title:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	title:setTextColor(cc.c4b(255, 0, 0, 255))
	title:setRotation(-30.0)
	title:setPositionX(center.x)
	title:setPositionY(center.y + 200.0)

	local caption = ccui.Text:create("Start", "font_marigoldwild.ttf", 130)
	caption:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	caption:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	caption:setTextColor(cc.c4b(0, 0, 0, 255))
	caption:setPosition(cc.p(0.0, 0.0))

	local btnYOffset = -220.0
	local btnPos = {x = center.x, y = center.y + btnYOffset}
	local button = cc.DrawNode:create()
	local radius = 200.0
	local yScale = 0.5
	button:setPosition(btnPos)
	button:drawSolidCircle(cc.p(0.0, 0.0), radius, 0.0, 70, 1.0, yScale, buttonNormalColor)
	button:setContentSize(cc.size(radius * 2.0, (radius * yScale) * 2.0))
	button:addChild(caption)
	button:runAction(StratchingBounceEffect())

	local wasPressed = false
	local function OnTouchBegin(touch, event)
		local touchPos = scene:convertToNodeSpace(touch:getLocation())
		local hitArea = cc.rect(
			-- lower left corner
			button:getPositionX() - button:getContentSize().width * 0.5,
			button:getPositionY() - button:getContentSize().height * 0.5,
			-- initial size
			button:getContentSize().width,
			button:getContentSize().height
		)
		if cc.rectContainsPoint(hitArea, touchPos) then
			button:cleanup()
			button:drawSolidCircle(cc.p(0.0, 0.0), radius, 0.0, 70, 1.0, yScale, buttonPressedColor)
			wasPressed = true
		end
		return true
	end

	local function OnTouchEnded(touch, event)
		button:cleanup()
		button:drawSolidCircle(cc.p(0.0, 0.0), radius, 0.0, 70, 1.0, yScale, buttonNormalColor)
		if wasPressed then
			local event = cc.EventCustom:new("StartButtonPressed")
			local dispatcher = scene:getEventDispatcher()
			dispatcher:dispatchEvent(event)
		end
	end

	local function OnTouchCancelled(touch, event)
		button:cleanup()
		button:drawSolidCircle(cc.p(0.0, 0.0), radius, 0.0, 70, 1.0, yScale, buttonNormalColor)
	end
	
	local listener = cc.EventListenerTouchOneByOne:create()
	listener:registerScriptHandler(OnTouchBegin, cc.Handler.EVENT_TOUCH_BEGAN)
	listener:registerScriptHandler(OnTouchEnded, cc.Handler.EVENT_TOUCH_ENDED)
	listener:registerScriptHandler(OnTouchCancelled, cc.Handler.EVENT_TOUCH_CANCELLED)
	local dispatcher = director:getEventDispatcher()
	dispatcher:addEventListenerWithSceneGraphPriority(listener, scene)

	scene:addChild(title)
	scene:addChild(button)

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
