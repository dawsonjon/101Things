import os
import datetime

with open("header.text") as inf:
  header = inf.read()

for directory in os.listdir():
  if os.path.isdir(directory) and directory[0].isdigit():
    for file in os.listdir(directory) :

      #cpp
      if file.endswith(".cpp") or file.endswith(".h"):
        full_file = os.path.join(directory, file)

        with open(full_file) as inf:
          first_line = inf.readline()

        if not first_line.startswith("//"):
          with open(full_file) as inf:
            text = inf.read()
          year = datetime.datetime.now().year
          text = header%(year, file) + "\n" + text
          with open(full_file, 'w') as outf:
            outf.write(text)
      #python
      if file.endswith(".py"):
        header = header.replace("//", "#")

        full_file = os.path.join(directory, file)

        with open(full_file) as inf:
          first_line = inf.readline()

        if not first_line.startswith("#"):
          with open(full_file) as inf:
            text = inf.read()
          year = datetime.datetime.now().year
          text = header%(year, file) + "\n" + text
          with open(full_file, 'w') as outf:
            outf.write(text)
        
        
      
