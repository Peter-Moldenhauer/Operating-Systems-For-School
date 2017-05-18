# Name: Peter Moldenhauer
# Date: 5/17/17
# Description: Program Py = mypython.py 


# Create and open 3 files in the same directory as this script
# I can pick the names of the files 
f1 = open("file1", 'w')
f2 = open("file2", 'w')
f3 = open("file3", 'w')

# Import random and string modules
import random, string

# Array of the file objects 
files = [f1, f2, f3]

# Loop through all of the file objects 
for file in files:
   # Generate random strings of 10 lowercase characters with no spaces 
   rand  = ''.join(random.choice(string.ascii_lowercase) for n in xrange(10))

   #write strings to files
   file.write(rand)
   
   # The 11th character is a newline character 
   file.write('\n')

   file.close()

# Open the files for reading
f1 = open("file1", 'r')
f2 = open("file2", 'r')
f3 = open("file3", 'r')

# Array of the file objects 
files = [f1, f2, f3]

# Print out the contents of each file 
for file in files:
    line = file.read(10)

    print line

# Generate random numbers in the range of 1 to 42
from random import randint
randNum1 = randint(1,42)
randNum2 = randint(1,42)

# Print out the random numbers 
print randNum1
print randNum2

# Print the sum of rand nums
ranSum = randNum1 * randNum2
print ranSum 