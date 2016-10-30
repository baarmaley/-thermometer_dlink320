#!/usr/bin/python

import os
import sys
import subprocess

#print u'\u00B0'

path_lock = '/tmp/temperature.lock' 

if os.path.isfile(path_lock):
	print "The script is runing!"
	exit()

if len(sys.argv) <= 1:
	print "Use: lumos.py /dir_name/name.xml"
	exit()

#lock
file_lock = open(path_lock, "w")
file_lock.close()

#dictionary

dictionary = {}

for line in  open('dictionary_lumos.txt'):
        (rom, string) = line.split(":")
        dictionary[rom] = string.replace('\r\n', '')

#end dictionary

cmd_search = 'relay -searchROM'


PIPE = subprocess.PIPE

p_search = subprocess.Popen(cmd_search, shell=True, stdin=PIPE, stdout=PIPE,
		stderr=subprocess.STDOUT, close_fds=True)
	
list_rom = list()

#search rom sensor temperature in lst
while True:
	rom = p_search.stdout.readline()
	if not rom: break
	if rom[:2] != "28": #other sensor
		continue
	list_rom.append(rom.replace('\n', ''))

#print list_rom

list_count = len(list_rom) - 1

#print list_count 

#read temperature

xml = '<?xml version = "1.0" ?>\n'

xml += '<inform>\n'

while list_count >= 0:
	#print list_rom[list_count]
	p_temp = subprocess.Popen('relay -temp ' + list_rom[list_count], shell=True, stdin=PIPE, stdout=PIPE,
                stderr=subprocess.STDOUT, close_fds=True)
	p_temp.wait()
	
	if list_rom[list_count] in dictionary:
		xml += '        <message sensor=\"' + dictionary[list_rom[list_count]] + '\">'
	else:
		xml += '	<message sensor=\"' + list_rom[list_count] + '\">'
	
	xml += p_temp.stdout.read().replace('\n', '') + u'\u00b0'.encode("utf-8") + 'c'
	#gradus = u'\u00b0' 
	#xml += gradus.encode("utf-8")
	xml += '</message>\n'
	#print p_temp.stdout.read()
	list_count -= 1

xml += '</inform>\n'

print xml

#write in file
file_xml = open(sys.argv[1], "w")
file_xml.write(xml)
file_xml.close()

#unlock
os.remove(path_lock)
