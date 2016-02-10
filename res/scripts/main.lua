function init()
end

function addProp(name)
	print(name)
	
	local script = loadfile(name)
	script()
end

