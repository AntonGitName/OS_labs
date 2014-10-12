#-------------------------------------------------------------------------------
# part 1

biggestJune=$(find . -type f -name "*june*" -printf '%s %p\n' | sort -nr | head -n 1 | awk '{print $NF}')
smallestSent=$(find . -type f -iname "*.sent" -printf '%s %p\n' | sort -n | head -n 1 | awk '{print $NF}')
biggestAttr=$(find . -type f -perm 444 -name "*2014*" -printf "%s %p\n" | sort -nr | head -n 1 | awk '{print $NF}')

if [[ ! -n "$biggestJune" ]] || [[ ! -n "$smallestSent" ]] || [[ ! -n "$biggestAttr" ]] ; then
	part1="ec"
else
	op1=$(cat $biggestJune | uniq)
	op2=$(cat $smallestSent | sort -k 2,2)
	if [[ "$op1" == "$op2" ]]; then
		part1='0'
	else
		part1='1'
	fi
	op1=$(cat $biggestJune | sort -k 5,5)
	op2=$(cat $biggestAttr | sort -k 5,5 | uniq)
	if [[ "$op1" == "$op2" ]]; then
		part1=$part1'0'
	else
		part1=$part1'1'
	fi
fi

#-------------------------------------------------------------------------------
# part 2

ssFiles=$(find . -type f -name "*.ss")
if [[ ! -n "$ssFiles" ]] ; then
	part2="000"
else
	theFile=$(wc -m $ssFiles | sort -nk 1,1 | head -n 1 | awk '{print $NF}')
	part2=$(echo -n "000$(wc -w $theFile | awk '{print $1}')" | tail -c 3)
fi

#-------------------------------------------------------------------------------
# part 3

result=0
for i in $(find . -type f); do
	op1=$(cat $i)
	op2=$(cat $i | uniq)
	if [[ "$op1" == "$op2" ]]; then
		result=$(($result + 1))
	fi
done
part3=$(printf "%02i" $result | head -c 2)

#-------------------------------------------------------------------------------
# part 4

day=$(date -d "-5 day" '+%A')
md5_1=$(find . -type f -iname "*$day*" -printf '%s %p\n' | sort -nr | awk '{print $2}' | xargs md5sum | awk {'print $1'} | md5sum | awk '{print $1}')
ind=$(date '+%j')
IFS=$'\n'
for x in $(find . -type f -printf '%s %p\n'); do
	size=$(echo $x | awk {'print $1'})
	fname=$(echo $x | awk {'print $2'})

	if (($size%365==$ind)); then
		list=$list$(md5sum $fname | awk '{print $1}')$'\n'
	fi
done
IFS=' '
md5_2=$(echo -n $list | sort | md5sum | awk '{print $1}')
ans=$(echo -n $md5_1$'\n'$md5_2 | md5sum)
part4=${ans:4:9}

echo $part1$part2$part3$part4