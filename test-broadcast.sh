#!/usr/bin/env bash

set -eo pipefail

trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

./packaged/bin/blocking-receive 0.0.0.0 62040 &
./packaged/bin/broadcast 224.0.0.1 62040 &

wait
