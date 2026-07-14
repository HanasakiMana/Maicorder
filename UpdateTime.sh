#!/bin/bash

PORT="/dev/cu.usbmodem12101"
BAUD_RATE="115200"

stty -f "$PORT" "$BAUD_RATE" cs8 -cstopb -parenb cread clocal

timeStamp=$(date +%s)
echo "T${timeStamp}" > $PORT

echo "Time updated to: $timeStamp"