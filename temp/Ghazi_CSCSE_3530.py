import random

# List of first names
first_names = ['Happy', 'Sleepy', 'Bashful', 'Sneezy', 'Grumpy', 'Dopey', 'Doc']

# List of last names
last_names = ['Apple', 'Orange', 'Banana', 'Cherry', 'Berry', 'Peach', 'Plum']

# Randomly choose a first and last name
random_first = random.choice(first_names)
random_last = random.choice(last_names)

# Combine them into a full name
full_name = random_first + " " + random_last

print("Your random name is:", full_name)
