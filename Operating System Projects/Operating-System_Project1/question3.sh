#Şevval Alp 150119815
#Reyyan Rana İnan 150119723
#Melih Sinan Çubukçuoğlu 150122828
# find the oldest file under current working directory

if [ $# -eq 0 ] #checking if user gave an argument file to delete directly if not 
then
oldestFile="$(ls -t -1 | tail -1)" #finding the oldest file in the current working directory
read -p "Do you want to delete $oldestFile? (y/n) :" answer

if [ "$answer" == "y" ] #user input whether he/she wants to delete of not
then
rm "$oldestFile" #remove the oldest file
echo "1 file deleted."

elif [ "$answer" == "n" ]
then
echo "$oldestFile is not going to be deleted."

else
echo "Incorrect input! Please try again and type 'y' for yes or 'n' for no."
exit 1
fi
 
elif [ $# -eq 1 ] #if there is 1 argument execute after here
then
argumentFile="$1" #passing the argument as argumentFile
cd "$argumentFile" #changing the current working directory to argumentFile so we
                   #search the oldest file in there
                   
oldestFileInArgument=$(ls -t | tail -1) #check the oldest file in argument's directory

if [ -e "$oldestFileInArgument" ] #checking if there is a oldest file 
then
read -p "Do you want to delete $oldestFileInArgument? (y/n) :" answer

if [ "$answer" == "y" ]
then
rm "$oldestFileInArgument"
echo "1 file deleted."

else
echo "$oldestFileInArgument is NOT going to be deleted."
fi

else
echo "There is no file called '$oldestFileInArgument' in the current working directory."
exit 1
fi

else
echo "You have entered more than 1 argument. Please enter only one."
exit 1

fi
