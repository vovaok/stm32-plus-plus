local onb_proto = Proto("onb", "ONB")

local default_settings = 
{
	port = 51967
}

local ActionID = 
{
	[0x40] = "aidPropagationUp",
	[0x80] = "aidPropagationDown",
	[0x00] = "PollNodes",
	[0x01] = "ConnReset",
	[0x10] = "Sync"
}

local SvcOID = 
{
	[0x00] = "svcClass",
	[0x01] = "svcName",
	[0x02] = "svcFullName",
	[0x03] = "svcSerial",
	[0x04] = "svcVersion",
	[0x05] = "svcBuildDate",
	[0x06] = "svcCpuInfo",
	[0x07] = "svcBurnCount",
	[0x08] = "svcObjectCount",
	[0x09] = "svcBusType",
	[0x0A] = "svcBusAddress",
	[0x80] = "svcObjectInfo",
	[0x90] = "svcTimedObject",
	[0x91] = "svcGetTimedObject",
	[0xA0] = "svcGroupObject",
	[0xF0] = "svcEcho",
	[0xF1] = "svcHello",
	[0xF2] = "svcWelcome",
	[0xF3] = "svcWelcomeAgain",
	[0xF4] = "svcConnected",
	[0xF5] = "svcDisconnected",
	[0xF6] = "svcKill",
	[0xF8] = "svcRequestAllInfo",
	[0xF9] = "svcRequestObjInfo",
	[0xFA] = "svcAutoRequest",
	[0xFB] = "svcTimedRequest",
	[0xFC] = "svcGroupedRequest",
	[0xFE] = "svcUpgradeRequest",
	[0xFF] = "svcFail"
}

local bustypes = 
{
	[0] = "BusUnknown",
	[1] = "BusCan",
	[2] = "BusUsbHid",
	[3] = "BusWifi",
	[4] = "BusSwonb",
	[5] = "BusVirtual",
	[6] = "BusRadio",
	[7] = "BusEthernet"
}

local nodes =
{
	[0] = {["name"] = "(master)", ["objects"] = {}},
	[127] = {["name"] = "(universal)", ["objects"] = {}}
}

local ObjectInfoTypes = 
{
	[43] = "Void",
	[1] = "Bool",
	[2] = "Int",
	[3] = "UInt",
	[4] = "LongLong",
	[5] = "UlongLong",
	[6] = "Double",
	[32] = "Long",
	[33] = "Short",
	[34] = "Char",
	[35] = "ULong",
	[36] = "UShort",
	[37] = "UChar",
	[38] = "Float",
	[40] = "SChar",
	[80] = "QTransform",
	[81] = "QMatrix4x4",
	[82] = "QVector2D",
	[83] = "QVector3D",
	[84] = "QVector4D",
	[85] = "QQuaternion",
	[10] = "String",
	[12] = "Common",
	[0x80] = "Compound_base"
}

local header = 
{
	prefix = ProtoField.new("Prefix", "onb.prefix", ftypes.STRING, nil),
	
	id = ProtoField.new("ID", "onb.id", ftypes.UINT32, nil, base.HEX),
	mac = ProtoField.new("BusAddress", "onb.mac", ftypes.UINT8, nil, base.DEC, 0x0F),
	-- isLocal = ProtoField.bool("onb.local", "Area", base.DEC, {[2]="Global", [1]="Local"}, 0x10),
	isLocal = ProtoField.new("Area", "onb.local", ftypes.UINT8, {[2]="Global", [1]="Local"}, base.DEC, 0x10),
	svc = ProtoField.new("Service", "onb.svc", ftypes.UINT8, nil, base.DEC, 0x80),
	frag = ProtoField.new("Fragmented", "onb.frag", ftypes.UINT8, nil, base.DEC, 0x80),
	sender = ProtoField.new("Sender", "onb.sender", ftypes.UINT8, nil, base.DEC, 0x7F),
	receiver = ProtoField.new("Receiver", "onb.receiver", ftypes.UINT8, nil, base.DEC, 0x7F),
	oid = ProtoField.new("ObjectID", "onb.oid", ftypes.UINT8, SvcOID, base.DEC),
	aid = ProtoField.new("ActionID", "onb.aid", ftypes.UINT8, ActionID, base.DEC),
	payload = ProtoField.new("Payload", "onb.payload", ftypes.UINT8, nil, base.DEC),
	
	-- ObjectInfo fields --
	_oid = ProtoField.new("ObjectID", "onb.ObjectInfo.oid", ftypes.UINT8, nil, base.DEC),
	flags = ProtoField.new("Flags", "onb.ObjectInfo.flags", ftypes.UINT8, nil, base.HEX),
	rType = ProtoField.new("ReadType", "onb.ObjectInfo.rType", ftypes.UINT8, ObjectInfoTypes, base.DEC),
	wType = ProtoField.new("WriteType", "onb.ObjectInfo.wType", ftypes.UINT8, ObjectInfoTypes, base.DEC),
	readSize = ProtoField.new("ReadSize", "onb.ObjectInfo.readSize", ftypes.UINT8, nil, base.DEC),
	writeSize = ProtoField.new("WriteSize", "onb.ObjectInfo.writeSize", ftypes.UINT8, nil, base.DEC),
	name = ProtoField.new("Name", "onb.ObjectInfo.name", ftypes.STRING, nil)
}

-- local ObjectInfo = 
-- {
	-- oid = ProtoField.new("ObjectID", "onb.ObjectInfo.oid", ftypes.UINT8, nil, base.DEC),
	-- flags = ProtoField.new("Flags", "onb.ObjectInfo.flags", ftypes.UINT8, nil, base.HEX),
	-- rType = ProtoField.new("ReadType", "onb.ObjectInfo.rType", ftypes.UINT8, ObjectInfoTypes, base.DEC),
	-- wType = ProtoField.new("WriteType", "onb.ObjectInfo.wType", ftypes.UINT8, ObjectInfoTypes, base.DEC),
	-- readSize = ProtoField.new("ReadSize", "onb.ObjectInfo.readSize", ftypes.UINT8, nil, base.DEC),
	-- writeSize = ProtoField.new("WriteSize", "onb.ObjectInfo.writeSize", ftypes.UINT8, nil, base.DEC),
	-- name = ProtoField.new("Name", "onb.ObjectInfo.name", ftypes.STRING, nil)
-- }

onb_proto.fields = header

function node(netaddr)
	return nodes[netaddr] or {["name"]="(UNKNOWN NODE #"..netaddr..")", ["objects"]={}}
end

--function nodeName(netaddr)
	--local nm = string.format("%s", nodes[netaddr].name or "(node "..netaddr..")")
	--if nm ~= "" then
	--	return nm
	--end
	--return "(node "..netaddr..")"
--end

function objName(netaddr, oid)
	return node(netaddr).objects[oid] or "Object #"..oid
end

function parseObjectInfo(tree, sender, data)
	local _oid = data:range(0, 1):uint()
	local name = data:range(6):string()
	node(sender).objects[_oid] = name
	tree:add(header._oid, data:range(0, 1))
	tree:add(header.flags, data:range(1, 1))
	tree:add(header.rType, data:range(2, 1))
	tree:add(header.wType, data:range(3, 1))
	tree:add(header.readSize, data:range(4, 1))
	tree:add(header.writeSize, data:range(5, 1))
	tree:add(header.name, data:range(6))
	local datastr = string.format("(%d) -> %s", _oid, name)
	return datastr
end

function onb_proto.dissector(tvbuf, pktinfo, root)
	
	local prefix_len = 4
	local header_len = 4
	local data_len = tvbuf:len() - header_len - prefix_len
	local msg_len = prefix_len + header_len + data_len
	
	local prefix = tvbuf:range(0, 4):string()
	if prefix ~= "ONB1" then
		return 0
	end
	
	pktinfo.cols.protocol:set("ONB")
	local tree = root:add(onb_proto, tvbuf:range(0, msg_len))
	-- local prefix = ProtoField.new("Prefix", "onb.prefix", ftypes.STRING, nil)
	tree:add(header.prefix, tvbuf:range(0, 4))
	
	if msg_len == prefix_len then
		tree:add(onb_proto, "[node advertising]")
		pktinfo.cols.info:set("[node advertising]")
		return msg_len
	end
	
	if data_len < 0 then
		pktinfo.cols.info:set("[MALFORMED PACKET]")
		return msg_len
	end
	
	-- tvbuf = tvbuf.range(prefix_len, msg_len - prefix_len)
	
	local id = tvbuf:range(prefix_len, 4)
	local data = tvbuf:range(prefix_len + 4, data_len)
	local value = ""
	local msgdata = ""
	
	local msgId = id:le_uint()
	local mac = bit.band(id:range(3, 1):uint(), 0x0F)
	local isLocal = bit.band(msgId, 0x10000000)
	local svc = bit.band(msgId, 0x00800000)
	
	local header_tree = tree:add_le(header.id, id)
	header_tree:add(header.isLocal, id:range(3, 1))
	header_tree:add(header.mac, id:range(3, 1))
	header_tree:add(header.svc, id:range(2, 1))
	
	local msginfo = (isLocal~=0) and "Local" or "Global"
	--local msgsvc = svc and "Service"
	--msginfo = msginfo.." "..msgsvc
	
	if isLocal == 0x10000000 then
	
		local frag = bit.band(msgId, 0x00008000)
		local oid = id:range(0, 1):uint()
		local sender = bit.band(id:range(1, 1):uint(), 0x7F)
		local receiver = bit.band(id:range(2, 1):uint(), 0x7F)
		local netaddr = sender~=0 and sender or receiver
		
		header_tree:add(header.frag, id:range(1, 1))
		header_tree:add(header.sender, id:range(1, 1))
		header_tree:add(header.receiver, id:range(2, 1))
		header_tree:add(header.oid, id:range(0, 1))
		
		local senderName = node(sender).name
		local receiverName = node(receiver).name
		
		msginfo = senderName.." > "..receiverName
		tree:add("[Path: "..msginfo.."]")
		msginfo = msginfo..": "
		
		local msgoid = "Object["..oid.."]"
		local is_request = (sender == 0 and data:len() == 0)
		
		if svc ~= 0 then
			msgoid = SvcOID[oid]
			
			if sender == 0 and msgoid == "svcWelcome" then
				local netaddr = data:range(0, 1):uint()
				nodes[netaddr] = {["name"] = "(node"..netaddr..")", ["objects"]={}}
			elseif is_request then
				value = ""
			elseif msgoid == "svcName" then
				value = "\""..data:string().."\""
				if data:range(0, 1):uint() ~= 0 then
					node(sender).name = data:string()
				end
			elseif msgoid == "svcFullName" or msgoid == "svcBuildDate" or msgoid == "svcCpuInfo" then
				value = "\""..data:string().."\""
			elseif msgoid == "svcClass" or msgoid == "svcSerial" then
				value = string.format("0x%08X", data:le_uint())
			elseif msgoid == "svcVersion" then
				value = string.format("%d.%d", data:range(1, 1):uint(), data:range(0, 1):uint())
			elseif msgoid == "svcBurnCount" or msgoid == "svcObjectCount" then
				value = data:uint()
			elseif msgoid == "svcBusType" then
				value = string.format("[%s]", bustypes[data:uint()] or "INVALID")
			elseif msgoid == "svcBusAddress" then
				value = data:uint()
			elseif msgoid == "svcObjectInfo" then
				if sender ~= 0 then
					local offset = 0
					while data:range(offset, 1):uint() == 0xFF do
						local _oid = data:range(offset+1, 1):uint()
						msgdata = string.format("%s%s.", msgdata, objName(sender, _oid))
						offset = offset + 2;
					end
					data = data:range(offset)
					-- datastr = parseObjectInfo(tree:add("ObjectInfo"), sender, data:range(offset, data_len-offset))
					-- msgdata = "\""..msgdata..datastr.."\""
				else
					local _oid = data:range(0, 1):uint()
					msgdata = string.format("\"%s\"", objName(receiver, _oid))
					local offset = 1
					while offset < data_len do
						local _sub = data:range(offset, 1):uint()
						offset = offset + 1
						msgdata = string.format("%s[%d]", msgdata, _sub)
					end
				end
				
			elseif msgoid == "svcAutoRequest" then
				local ms = data:range(0, 4):le_uint()
				local _oid = data:range(4, 1):uint()
				if ms == 0xFFFFFFFF then
					msgdata = string.format("request \"%s\"", objName(netaddr, _oid))
				else
					msgdata = string.format("\"%s\", %d ms", objName(netaddr, _oid), ms)
				end
			
			elseif msgoid == "svcTimedObject" then
				local _oid = data:range(0, 1):uint()
				local ms = data:range(2, 4):le_uint()
				msgdata = string.format("timestamp=%d %s=(data)", ms, objName(netaddr, _oid))
			end
			
		else
		
			msgoid = objName(netaddr, oid)
		
		end
		
		msginfo = msginfo.." "..msgoid
		if msgdata ~= "" then
			msginfo = msginfo.." "..msgdata
		end
		
		if msgoid ~= "" then
			tree:add(string.format("[Object: %s]", msgoid))
		end
		
		if msgoid == "svcObjectInfo" then
			if sender == 0 then
				msginfo = msginfo.." (request)"
			else
				local info = parseObjectInfo(tree:add("ObjectInfo: "), sender, data)
				msginfo = msginfo.." "..info
			end
		elseif is_request then
			msginfo = msginfo.." (request)"
		elseif value ~= "" then
			msginfo = msginfo.." = "..value
			tree:add("Value:", value)
		end
	
	else -- this is global message --
		
		local aid = id:range(0, 1):uint()
		local payload = id:range(1, 1):uint()
		local sender = bit.band(id:range(2, 1):uint(), 0x7F)
		
		local senderName = node(sender).name
		
		header_tree:add(header.sender, id:range(2, 1))
		header_tree:add(header.payload, id:range(1, 1))
		header_tree:add(header.aid, id:range(0, 1))
		
		if (svc) then
			local msgaid = ActionID[aid]
			msginfo = senderName.." > "..msgaid
		end
		
	end
	
	-- if data_len > 0 then
		-- tree:add(onb_proto, data:bytes():tohex())
	-- end
	
	pktinfo.cols.info:set(msginfo)
	
	return msg_len
end

DissectorTable.get("udp.port"):add(default_settings.port, onb_proto)