#!/bin/sh

my_private_ipv4=`ec2-metadata -o|awk '{print $2;}'`

echo "my ip is $my_private_ipv4 "

echo "'NetworkInterfaces[?PrivateIpAddress==`$my_private_ipv4`].NetworkInterfaceId'"