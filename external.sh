#!/bin/sh

# led gpio same as switch.ko module option
led="57"

log="/tmp/qrdata.log"
img="/tmp/img.jpg"

rm -f ${log}

while ! [ -s ${log} ]; do
  curl -s -o ${img} http://127.0.0.1/image.jpg ; qrscan -p ${img} > ${log}
  sleep 2 # throttle
done

if [ ${led} ]; then
  # make off led if selected
  echo 0 > /sys/class/gpio/gpio${led}/value
fi
