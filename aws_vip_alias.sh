#!/bin/sh

my_private_ipv4=`ec2-metadata -o|awk '{print $2;}'`
#my_private_ipv4='127.0.0.1'
echo "my ip is $my_private_ipv4 "

echo "aws ec2 describe-network-interfaces --query 'NetworkInterfaces[?PrivateIpAddress==\`$my_private_ipv4\`].NetworkInterfaceId'"

aws ec2 describe-network-interfaces --query 'NetworkInterfaces[?PrivateIpAddress==\`$my_private_ipv4\`].NetworkInterfaceId'