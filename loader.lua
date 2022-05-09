local args = {}
for word in string.gmatch(..., "%S+") do
	table.insert(args, word)
end

SERVICE_NAME = args[1]

local main, pattern
local err = {}
for pat in string.gmatch(LUA_SERVICE, "([^;]+);*") do
	local filename = string.gsub(pat, "?", SERVICE_NAME)
	local f, msg
	if SERVICE_NAME == "testservice" then
		print("gooooooooo==============")
		local strr = "local skynet = require \"skynet\" \
		Log.i(\"test service header\") \
		skynet.start(function() \
			local chunk = require(\"chunk\") \
			local success = chunk.loadzip(\"testzip.zip\",\"222222\") \
			Log.i(\"test service start\") \
			require \"testzip\" \
			Log.i(\"finish testzip\") \
			require \"dir.testdir\" \
			Log.i(\"finish test dir\") \
		end)"
		f = load(strr,SERVICE_NAME)
		print("type =======",type(f))
	else
		f, msg = loadfile(filename)
	end
	if not f then
		table.insert(err, msg)
	else
		pattern = pat
		main = f
		break
	end
end

if not main then
	error(table.concat(err, "\n"))
end

LUA_SERVICE = nil
package.path , LUA_PATH = LUA_PATH
package.cpath , LUA_CPATH = LUA_CPATH

local service_path = string.match(pattern, "(.*/)[^/?]+$")

if service_path then
	service_path = string.gsub(service_path, "?", args[1])
	package.path = service_path .. "?.lua;" .. package.path
	SERVICE_PATH = service_path
else
	local p = string.match(pattern, "(.*/).+$")
	SERVICE_PATH = p
end

if LUA_PRELOAD then
	local f = assert(loadfile(LUA_PRELOAD))
	f(table.unpack(args))
	LUA_PRELOAD = nil
end

main(select(2, table.unpack(args)))
