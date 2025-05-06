#!/usr/bin/env bash

set -eo pipefail

trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

./install/bin/coroutines &
sleep 1
./install/bin/broadcast 127.0.0.1 62040 &
./install/bin/broadcast 127.0.0.1 62041 &

wait
