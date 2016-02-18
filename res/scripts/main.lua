function init(name)
	local script = loadfile(name.."/script.lua")
	script()
end

function addProp(name)
	local script = loadfile(name)
	script()
end

