if not ppp  then
    print("this is first ppp");
    ppp = {}
end

if not version then
    version = "1,23,45";
    end

function aaaaa(num)
	print(num-1000);
	ppp[num+2]=1;
	return num+3000;
end

function bbbb(xx)
    xx.luaSay();
end

for key,value in pairs(ppp) do
	print(key,":",value,"\n");
end