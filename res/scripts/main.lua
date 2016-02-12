function init()
end

function addProp(name)
	local script = loadfile(name)
	script()
end

