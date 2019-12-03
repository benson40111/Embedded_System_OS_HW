#!/bin/bash

echo "$(date): start write (file creation)"
dd if=/dev/zero of=testfile bs=1M count=1K
echo "$(date): end write"

rm -f testfile
