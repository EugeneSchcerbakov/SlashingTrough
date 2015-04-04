-- gui.lua
-- SlashingTrough
--
-- Created by Eugene Shcherbakov on 01/04/15.
--

require "Cocos2d"
require "Cocos2dConstants"

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