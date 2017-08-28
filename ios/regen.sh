#!/usr/bin/env bash
set -x

cmake -DBUILD_IOS=ON -G Xcode ../
