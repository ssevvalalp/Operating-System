#Şevval Alp 150119815
#Reyyan Rana İnan 150119723
#Melih Sinan Çubukçuoğlu 150122828
# find all the files whose name obeys the wildcard 
# and copy them into a directory

if [ "$1" = "-R" ]; then
recursion="true"
wildcardArgument=$2

else
recursion="false"
wildcardArgument=$1
fi

# Create the 'copied' directory in case it's not already present.
mkdir -p copied

# Locate files that match the specified wildcard pattern.
fileOfWildcard=$(find . -name "$wildcardArgument")

# Transfer files into the 'copied' directory
for FILE in $fileOfWildcard 
do
destinationFile="copied/${FILE##*/}"

cp "$FILE" "$destinationFile"

# Copy files within subdirectories in a recursive manner.
if [ "$recursion" = "true" ]
then
if [ -d "$FILE" ] 
then

subDirectory="$destinationFile/$(basename "$FILE")"
mkdir -p "$subDirectory"
./myprog5.sh -R "$wildcardArgument" "$FILE"

fi
fi
done
