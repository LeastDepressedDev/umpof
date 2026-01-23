#!/bin/bash
./gradlew clean build
if [[ $1 = "+run" ]]; then
  java -jar ./build/libs/umpof-1.0.jar $(cat test_args.txt)
fi