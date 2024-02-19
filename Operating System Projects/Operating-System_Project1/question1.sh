#Şevval Alp 150119815
#Reyyan Rana İnan 150119723
#Melih Sinan Çubukçuoğlu 150122828
# A histogram is a representation of the distribution of numerical data

if [ "$#" -ne 1 ] #checking if user did not type more than one argument
then
echo "Incorrect amount of argument, this script only needs ONE argument as text file."
exit 1
fi

fileName=$1

count0=0
count1=0
count2=0
count3=0
count4=0
count5=0
count6=0
count8=0
count9=0
while IFS= read -r line
do
if [ "$line" -eq 0 ]
then
((count0++))
elif [ "$line" -eq 1 ]
then
((count1++))
elif [ "$line" -eq 2 ]
then
((count2++))
elif [ "$line" -eq 3 ]
then
((count3++))
elif [ "$line" -eq 4 ]
then
((count4++))
elif [ "$line" -eq 5 ]
then
((count5++))
elif [ "$line" -eq 6 ]
then
((count6++))
elif [ "$line" -eq 7 ]
then
((count7++))
elif [ "$line" -eq 8 ]
then
((count8++))
elif [ "$line" -eq 9 ]
then
((count9++))
else
echo "Invalid variable"
exit 1
fi
done < $fileName
i=0
echo -n "0:"
for((i;i<count0;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "1:"
for((i;i<count1;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "2:"
for((i;i<count2;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "3:"
for((i;i<count3;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "4:"
for((i;i<count4;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "5:"
for((i;i<count5;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "6:"
for((i;i<count6;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "7:"
for((i;i<count7;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "8:"
for((i;i<count8;i++))
do
echo -n "*"
done
echo ""
i=0
echo -n "9:"
for((i;i<count9;i++))
do
echo -n "*"
done
echo ""
i=0