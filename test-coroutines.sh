#!/usr/bin/env bash

set -eo pipefail

trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

./packaged/bin/coroutines &
sleep 1
./packaged/bin/broadcast 127.0.0.1 62040 &
./packaged/bin/broadcast 127.0.0.1 62041 &

wait
