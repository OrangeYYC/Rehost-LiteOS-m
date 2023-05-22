#!/bin/bash

#Copyright (c) 2021 Huawei Device Co., Ltd.
#Licensed under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License.
#You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing, software
#distributed under the License is distributed on an "AS IS" BASIS,
#WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#See the License for the specific language governing permissions and
#limitations under the License.

if [ "$1" = "" ]; then
  echo "vendor/ohemu/common/qemu_mini_test_monitor.sh [log.txt]"
fi

test_file=$1
qemu_count_max=30 # 5 minutes
qemu_count=0

rm -f $test_file

function kill_specified_process(){
    qemu_name=$1
    while true
    do
        pid=`ps -ef | grep $qemu_name | grep -v grep | awk '{print $2}' | head -n 1`
        if [ "$pid" == "" ]; then
            break
        fi
        echo "kill pid $pid"
        kill -15 $pid
    done
}

function kill_qemu() {
    kill_specified_process qemu-system-
    exit 0
}

while true
do
    if [ ! -f "$test_file" ]; then
        sleep 10s
    else
        result=`tail -1 $test_file`
        if [ "$result" = "--- Test End ---" ]; then
            kill_qemu
            break
        else
            sleep 10s
        fi
    fi
    let qemu_count++
    if [ $qemu_count = "$qemu_count_max" ]; then
        kill_qemu
        break
    fi
done
