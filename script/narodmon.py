#!/usr/bin/python
# -*- coding: utf-8 -*-

import urllib2
import urllib

#Open file and get string
file = open('/www/lumos/temperature.xml')
str = file.read()
file.close()

# Format string 
start = str.find('<message sensor="За окном">')
end = str[start:].find('c</message>') + start - 2 # * -1 del char gradus
str = str[start:end].replace('<message sensor="За окном">', '')

sensor_value = float(str)

#print repr(sensor_value)

print sensor_value

DEVICE_MAC = ' '
SENSOR_ID = ' '

data = urllib.urlencode({
    'ID': DEVICE_MAC,
    SENSOR_ID: sensor_value
})

# формирование заголовков запроса
headers = {
    'Content-Length': len(data),
    'Content-Type': 'application/x-www-form-urlencoded',
    'Host': 'narodmon.ru'
}

# непосредственно запрос
request = urllib2.Request('http://narodmon.ru/post.php', data, headers)
response = urllib2.urlopen(request)
print response.headers