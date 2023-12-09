import sys

sample_rate = 10000

def calculate_frequencies():
  f = 261.63 #middle C
  notes = ["c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"]
  ratio = pow(2, 1/12)
  frequencies = {}
  for octave in [4, 5, 6, 7, 8]:
    for note in notes:
      frequencies["%s%u"%(note, octave)] = round(f)
      f *= ratio
  return frequencies

frequencies = calculate_frequencies()
print(frequencies)

def sanitise_name(name):
  new_name = []
  for c in name:
    if c.isalpha():
      new_name.append(c)
  return "".join(new_name)

def parse_rtttl(rtttl, output_file):
  title, header, body = rtttl.split(":")
  title = title.strip()
   
  header_dict = {"o":6, "b":63, "d":4}
  for key_value in header.split(","):
    key, value = key_value.split("=")
    key = key.strip()
    value = value.strip()
    header_dict[key.lower()] = value.lower()


  output_file.write("const note_t %s[] = {\n"%sanitise_name(title))
  num_notes = 0
  for note in body.split(","):
    note = note.strip()
    if not note:
      break
    
    duration = ""
    while note and note[0].isdigit():
      duration += note[0]
      note = note[1:]
    if duration:
      duration = int(duration)
    else:
      duration = int(header_dict["d"])

    if note and note[0]==".":
      dotted = True
      note = note[1:]
    else:
      dotted = False
    
    pitch = note[0]
    note = note[1:]
    if note and note[0] == "#":
      pitch += "#"
      note = note[1:]

    if note and note[0]==".":
      dotted = True
      note = note[1:]
    else:
      dotted = False

    if note and note[0].isdigit():
      octave = int(note[0])
      note = note[0]
    else:
      octave = int(header_dict["o"])

    if note and note[0]==".":
      dotted = True
      note = note[1:]
    else:
      dotted = False

    #convert duration to samples
    if dotted:
      duration = (sample_rate*60*3*4)//(duration*2*int(header_dict["b"]))
    else:
      duration = (sample_rate*60*4)//(duration*int(header_dict["b"]))

    #convert pitch to frequency
    frequency = frequencies.get("%s%u"%(pitch, octave), 0)

    output_file.write("{%u, %u},//%s%u\n"%(frequency, duration, pitch, octave))
    num_notes += 1

  output_file.write("};\n")

  return sanitise_name(title), num_notes

with open("tunes.hpp", "w") as output_file: 
  output_file.write('#include "music.hpp"\n')
  titles_num_notes = []
  for file in sys.argv[1:]:
    print(file)
    with open(file, "r") as input_file:
      rtttl = input_file.read()
    titles_num_notes.append(parse_rtttl(rtttl, output_file))
  output_file.write("uint16_t num_tunes = %u;\n"%len(titles_num_notes))
  output_file.write("const note_t *tunes[] = {\n")
  output_file.write(",\n".join(["&"+i[0]+"[0]" for i in titles_num_notes]))
  output_file.write("};\n")
  output_file.write("const uint16_t num_notes[] = {\n")
  output_file.write(",\n".join([str(i[1]) for i in titles_num_notes]))
  output_file.write("};\n")
