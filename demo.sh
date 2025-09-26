#!/bin/bash

echo "Yandex Images Parser Demo"

# Start GeckoDriver if not running
if ! pgrep -f "geckodriver" > /dev/null; then
    echo "Starting GeckoDriver..."
    geckodriver --port=4445 &
    sleep 2
fi

# Compile and run
gcc -o demo demo.c -lyandex_parser -lcurl
./demo

# Cleanup
rm -f demo
echo "Demo completed!"