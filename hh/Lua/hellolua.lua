local num = {0,0,2,1,3,6};
local zero = 0;
local mis = {};
local miss = {};
local pre = nil;
function find(tb)
	table.sort(tb);
	for key,value in pairs(tb) do
		if(value == 0) then
			zero=zero+1;
		else
			table.insert(mis,value);
--			print(value);
		end
	end

	print(zero);
	for key,value in pairs(mis) do
		if(pre==nil) then
			table.insert(miss,value);
			pre  = value;
		else
			table.insert(miss,value-pre);
			pre = value;
		end
	end
	local total=0;
	for key,value in pairs(miss) do
		if(value == 0) then
			return false;
		end
		total = total + value - 1;
	end
	if(total > zero) then
		return false;
	else
		return true;
	end
end

result = find(num);
print(result);