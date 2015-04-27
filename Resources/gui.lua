-- gui.lua
-- SlashingTrough
--
-- Created by Eugene Shcherbakov on 01/04/15.
--

require "scripts/Cocos2d"
require "scripts/Cocos2dConstants"

local PlayerHealthString = "null"
local PlayerHealthDirty = false
local PlayerScoreString = "null"
local PlayerScoreDirty = false
local TimeScaleString = "null"
local TimeScaleDirty = false

function UpdateHealthWidget(health)
	PlayerHealthString = string.format("%s%%", health)
	PlayerHealthDirty = true
end

function UpdateScoreWidget(score)
	PlayerScoreString = string.format("Kills: %s", score)
	PlayerScoreDirty = true
end

function UpdateTimeScaleWidget(timeScale)
	TimeScaleString = string.format("x%s", timeScale)
	TimeScaleDirty = true
end

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

function CreateInterfaceLayer()
	local director = cc.Director:getInstance()
	local scheduler = director:getScheduler()
	local layer = cc.Layer:create()
	local hud = cc.CSLoader:createNode("hud.csb")
	
	local origin = director:getVisibleOrigin()
	local size = director:getVisibleSize()

	local timeScaleText = ccui.Text:create()
	timeScaleText:setFontName("font_prototype.ttf")
	timeScaleText:setFontSize(150)
	timeScaleText:setString("")
	timeScaleText:setTextHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	timeScaleText:setTextVerticalAlignment(cc.TEXT_ALIGNMENT_CENTER)
	timeScaleText:setVisible(false)
	local timeScaleTextYShift = -400.0
	timeScaleText:setPositionX(origin.x + size.width * 0.5)
	timeScaleText:setPositionY(origin.y + size.height * 0.5 + timeScaleTextYShift)

	local function update(dt)
		if PlayerHealthDirty then
			local widget = hud:getChildByName("health")
			widget:setString(PlayerHealthString)
			PlayerHealthDirty = false
		elseif PlayerScoreDirty then
			local widget = hud:getChildByName("score")
			widget:setString(PlayerScoreString)
			PlayerScoreDirty = false
		elseif TimeScaleDirty then
			timeScaleText:setVisible(true)
			timeScaleText:setString(TimeScaleString)
			TimeScaleDirty = false
			if TimeScaleString == "x1.0" then
				local function func( ... )
					timeScaleText:setVisible(false)
				end
				local delay = cc.DelayTime:create(2.0)
				local call = cc.CallFunc:create(func)
				local seq = cc.Sequence:create(delay, call)
				timeScaleText:runAction(seq)
			end
		end
	end

	layer:scheduleUpdateWithPriorityLua(update, 0)
	layer:addChild(hud, 0)
	layer:addChild(timeScaleText, 1)
	return layer
end