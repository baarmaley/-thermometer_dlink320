# -thermometer_dlink320
<p>Термометр для D-Link DIR 320 и отправкой данных на narodmon.ru</p>
<p>Проект был реализован летом 2014 года и проработал примерно до сентября 2016 года, когда роутер приказал долго жить.</p>
<p>Был использован мк at90usb162 как hid-usb. Прошивка была разработана с помощью статей на сайте hackaday.com, исходников lightpack 
и редких статей в рунете про hid-usb.
Использовались датчики DS18b20 работающие по протоколу 1-wire. 
Реализация протокола 1-wire была найдена в интернете (ссылок не осталось).</p>
<p>Для роутера была собрана кастомная прошивка Open-WRT, и написана программа для считывания показаний с датчиков.</p>
<p>Так как устройство планировалось сделать многофункциональным, была так же возможность управления нагрузкой 220 вольт с помощью реле. 
Но от неправильной разводки земли на плате клавиатуры, были ложные срабатывания и от этого пришлось отказаться.</p>
<p>С помощью cron и скриптов на python данные отправлялись на narodmon.ru и выводились на LCD</p>
