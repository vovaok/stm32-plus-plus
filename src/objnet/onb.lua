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

local header = 
{
	id		= ProtoField.new("ID", "onb.id", ftypes.UINT32, nil, base.HEX),
	mac = ProtoField.new("BusAddress", "onb.mac", ftypes.UINT8, nil, base.DEC),
	--isLocal = ProtoField.new("Area", "onb.local", ftypes.BOOLEAN, {[2]="Global", [1]="Local"}, base.HEX, 0x10000000)
	isLocal = ProtoField.bool("onb.local", "Area", base.DEC, {[2]="Global", [1]="Local"}, 0x10000000),
	svc = ProtoField.new("Service", "onb.svc", ftypes.BOOLEAN, nil, base.DEC, 0x00800000),
	frag = ProtoField.new("Fragmented", "onb.frag", ftypes.BOOLEAN, nil, base.DEC, 0x00008000),
	sender = ProtoField.new("Sender", "onb.sender", ftypes.UINT8, nil, base.DEC),
	receiver = ProtoField.new("Receiver", "onb.receiver", ftypes.UINT8, nil, base.DEC),
	oid = ProtoField.new("ObjectID", "onb.oid", ftypes.UINT8, SvcOID, base.DEC),
	aid = ProtoField.new("ActionID", "onb.aid", ftypes.UINT8, ActionID, base.DEC),
	payload = ProtoField.new("Payload", "onb.payload", ftypes.UINT8, nil, base.DEC)
}

onb_proto.fields = header

function node(netaddr)
	return nodes[netaddr] or {["name"]="(UNKNOWN NODE)", ["objects"]={}}
end

--function nodeName(netaddr)
	--local nm = string.format("%s", nodes[netaddr].name or "(node "..netaddr..")")
	--if nm ~= "" then
	--	return nm
	--end
	--return "(node "..netaddr..")"
--end

function objName(netaddr, oid)
	return nodes[netaddr].objects[oid] or "Object #"..oid
end

function onb_proto.dissector(tvbuf, pktinfo, root)
	
	local header_len = 4
	local data_len = tvbuf:len() - header_len
	local msg_len = header_len + data_len
	
	local data = tvbuf:range(4, data_len)
	local msgdata = ""
	
	pktinfo.cols.protocol:set("ONB")
	
	local msgId = tvbuf:range(0, 4):le_uint()
	local mac = bit.band(tvbuf:range(3, 1):uint(), 0x0F)
	local isLocal = bit.band(msgId, 0x10000000)
	local svc = bit.band(msgId, 0x00800000)
	
	local tree = root:add(onb_proto, tvbuf:range(0,msg_len))
	local header_tree = tree:add(header.id, msgId)
	tree:add(header.mac, mac)
	tree:add(header.isLocal, msgId)
	tree:add(header.svc, svc)
	
	local msginfo = (isLocal~=0) and "Local" or "Global"
	--local msgsvc = svc and "Service"
	--msginfo = msginfo.." "..msgsvc
	
	if isLocal == 0x10000000 then
	
		local frag = bit.band(msgId, 0x00008000)
		local oid = tvbuf:range(0, 1):uint()
		local sender = bit.band(tvbuf:range(1, 1):uint(), 0x7F)
		local receiver = bit.band(tvbuf:range(2, 1):uint(), 0x7F)
		local netaddr = sender~=0 and sender or receiver
		
		tree:add(header.frag, frag)
		tree:add(header.sender, sender)
		tree:add(header.receiver, receiver)
		tree:add(header.oid, oid)
		
		local senderName = node(sender).name
		local receiverName = node(receiver).name
		
		msginfo = senderName.." > "..receiverName
		
		local msgoid = "Object["..oid.."]"
		
		if svc ~= 0 then
			msgoid = SvcOID[oid]
			
			if sender == 0 and msgoid == "svcWelcome" then
				nodes[receiver] = {["name"] = "(node"..receiver..")", ["objects"]={}}
			elseif msgoid == "svcName" then
				msgdata = "\""..data:string().."\""
				if data:range(0, 1):uint() ~= 0 then
					node(sender).name = data:string()
				end
			elseif msgoid == "svcFullName" or msgoid == "svcBuildDate" or msgoid == "svcCpuInfo" then
				msgdata = "\""..data:string().."\""
			elseif msgoid == "svcClass" or msgoid == "svcSerial" then
				msgdata = string.format("0x%08X", data:le_uint())
			elseif msgoid == "svcVersion" then
				msgdata = string.format("%d.%d", data:range(1, 1):uint(), data:range(0, 1):uint())
			elseif msgoid == "svcBurnCount" or msgoid == "svcObjectCount" then
				msgdata = data:uint()
			elseif msgoid == "svcBusType" then
				msgdata = string.format("[%s]", bustypes[data:uint()] or "INVALID")
			elseif msgoid == "svcObjectInfo" then
				if sender ~= 0 then
					local offset = 0
					while data:range(offset, 1):uint() == 0xFF do
						local _oid = data:range(offset+1, 1):uint()
						msgdata = string.format("%s%s.", msgdata, objName(sender, _oid))
						offset = offset + 2;
					end
					local _oid = data:range(offset, 1):uint()
					offset = offset + 6
					local datastr = data:range(offset, data_len-offset):string()
					node(sender).objects[_oid] = datastr
					msgdata = "\""..msgdata..datastr.."\""
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
			end
			
		else
		
			msgoid = objName(netaddr, oid)
		
		end
		
		msginfo = msginfo.." "..msgoid
		if msgdata ~= "" then
			msginfo = msginfo.." = "..msgdata
		end
	
	else -- this is global message --
		
		local aid = tvbuf:range(0, 1):uint()
		local payload = tvbuf:range(1, 1):uint()
		local sender = bit.band(tvbuf:range(2, 1):uint(), 0x7F)
		
		local senderName = node(sender).name
		
		tree:add(header.sender, sender)
		tree:add(header.payload, payload)
		tree:add(header.aid, aid)
		
		if (svc) then
			local msgaid = ActionID[aid]
			msginfo = senderName.." > "..msgaid
		end
		
	end
	
	if data_len > 0 then
		tree:add(onb_proto, data:bytes():tohex())
	end
	
	pktinfo.cols.info:set(msginfo)
	
	return msg_len
end

DissectorTable.get("udp.port"):add(default_settings.port, onb_proto)