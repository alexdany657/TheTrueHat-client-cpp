#!/bin/bash

# importing config
config=`cat config.sh`
$config

for (( i=0; i < roomsCount; ++i )); do
    playersCount=$((RANDOM * (2 * playersCountDelta + 1) / 32767 - playersCountDelta + playersCountAv))
    key=`wget -qO- https://m20-sch57.site:3005/api/getFreeKey | sed -E "s/\{.*\":\"//g" | sed -E "s/\".//g"`
    while [[ $key = "" ]]; do
        key=`wget -qO- https://m20-sch57.site:3005/api/getFreeKey | sed -E "s/\{.*\":\"//g" | sed -E "s/\".//g"`
    done
    for (( j=0; j < playersCount; ++j )); do
        ./client $key $playersCount $j"_" &
    done
done
