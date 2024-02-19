#Şevval Alp 150119815
#Reyyan Rana İnan 150119723
#Melih Sinan Çubukçuoğlu 150122828
# Convert the input string into a ciphered one

if [ "$#" -ne 2 ]
then
echo "Usage: $0 <string> <number>"
exit 1
fi

# Save the parameters in variables.
stringArg="$1"
numberArg="$2"

# Determine whether the second argument is a solitary numeric value.
if [[ $numberArg =~ ^[0-9]$ ]]
then
# Duplicate the numeral based on the length of the input string.
numberArg=$(printf "%0${#stringArg}d" "$input_number")
elif [ ${#numberArg} -ne ${#stringArg} ]
then
echo "Second argument must be a single number or have the same length as the first argument."
exit 1
fi

# Establish the character set for the alphabet.
alphabet="abcdefghijklmnopqrstuvwxyz"
lenOfAlphabet=${#alphabet}

#Encode the input string using the provided number.
updatedString=""

for ((i = 0; i < ${#stringArg}; i++)); do
char="${stringArg:i:1}"
shift="${numberArg:i:1}"
    
if [ -n "$char" ]
then
index=$(expr index "$alphabet" "$char")
if [ "$index" -gt 0 ]
then
# Determine the adjusted index, making sure it wraps around.
index=$((($index - 1 + $shift) % $lenOfAlphabet))
updatedChar="${alphabet:$index:1}"

else
updatedChar="$char"
fi

else
updatedChar=""
fi

updatedString="$updatedString$updatedChar"
done

# Output the encoded string
echo "$updatedString"
