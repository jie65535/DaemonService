@echo off
echo Daemon Service Initialize Start...

netsh ipsec static add policy name=DaemonService
netsh ipsec static add filterlist name=whitelist
netsh ipsec static add filterlist name=blacklist
netsh ipsec static add filter filterlist=whitelist srcaddr=192.169.0.100 dstaddr=me protocol=ICMP
netsh ipsec static add filter filterlist=blacklist srcaddr=192.169.0.100 dstaddr=me protocol=ICMP
netsh ipsec static add filteraction name=permit action=permit
netsh ipsec static add filteraction name=block action=block
netsh ipsec static add rule name=permitRule policy=DaemonService filterlist=whitelist filteraction=permit
netsh ipsec static add rule name=blockRule policy=DaemonService filterlist=blacklist filteraction=block
netsh ipsec static delete filter filterlist=whitelist srcaddr=192.169.0.100 dstaddr=me protocol=ICMP
netsh ipsec static delete filter filterlist=blacklist srcaddr=192.169.0.100 dstaddr=me protocol=ICMP
netsh ipsec static set policy name=DaemonService assign=y

rem 禁止任何人通过Tcp访问指定端口，请使用下面语句，并把dstport改成对应端口
rem netsh ipsec static add filter filterlist=blacklist srcaddr=any dstaddr=me protocol=TCP dstport=9876

echo End of initialization
pause