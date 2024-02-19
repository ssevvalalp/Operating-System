#Şevval Alp 150119815
#Reyyan Rana İnan 150119723
#Melih Sinan Çubukçuoğlu 150122828
# change the numbers to text for each number in between 0-9

if [ "$#" -ne 1 ] #checking if user did not type more than one argument
then
  echo "Incorrect amount of argument, this script only needs ONE argument."
  exit 1
fi
#if the argument amount is correct
fileName="$1" # passing argument as 'fileName'

if [ ! -f "$fileName" ] # checking if the argument file exists
then
  echo "There is no such file called :> $fileName"
  exit 1
fi
while IFS= read -r word # reading the file and passing the read var to temp
do
 
word=$(echo "$word" | sed 's/0/zero /g') #if the word var is actually a number then we pipe it 
word=$(echo "$word" | sed 's/1/one /g') #to the sed(stream editor) and changing the number
word=$(echo "$word" | sed 's/2/two /g') #to how it is written
word=$(echo "$word" | sed 's/3/three /g')
word=$(echo "$word" | sed 's/4/four /g')
word=$(echo "$word" | sed 's/5/five /g')
word=$(echo "$word" | sed 's/6/six /g')
word=$(echo "$word" | sed 's/7/seven /g')
word=$(echo "$word" | sed 's/8/eight /g')
word=$(echo "$word" | sed 's/9/nine /g')

 
  echo "$word" #after operations send the temporary words to a temporary file until
done < "$fileName" > temp_file.txt #all numbers changed to how they are written
                                   
mv temp_file.txt "$fileName" #after we change every number then move temporariy file to argument file

echo "The numbers in your file has been changed to how they are written."
echo "Please check your file:> $fileName"
