#!/bin/bash
#./boot.sh
#./configure --with-linux=/lib/modules/`uname -r`/build

#cp datapath/Makefile.save datapath/Makefile
#cp Makefile.save Makefile
#make check TESTSUITEFLAGS=-j8
#make check TESTSUITEFLAGS='-k ofproto'

make && make install
make modules_install
rmmod openvswitch 
modprobe vxlan && modprobe gre && modprobe libcrc32c
/sbin/modprobe openvswitch
#/sbin/modinfo openvswitch.ko 
#/sbin/modinfo /lib/modules/`uname -r`/kernel/net/bridge/bridge.ko
#cp /home/air/openvswitch-2.5.0/datapath/linux/openvswitch.ko /lib/modules/3.13.0-24-generic/kernel/net/openvswitch
#insmod /home/air/openvswitch-2.5.0/datapath/linux/openvswitch.ko
cp /home/air/glob-openvswitch-2.5.0/utilities/ovs-ofctl /usr/local/bin/ovs-ofctl
ovs-vsctl --no-wait init
#ovs-vsctl --no-wait init set controller br0 connection-mode=out-of-band
#added rm
#ovs-vswitchd -v --log-file=/var/log/openvswitch/amr-vswitchd.log -vconsole:warn -vsyslog:warn -vfile:warn
rm /var/log/openvswitch/vswitchd-2.5.0.log
ovs-vswitchd --pidfile -v --log-file=/var/log/openvswitch/vswitchd-2.5.0.log -vconsole:dbg -vsyslog:dbg -vfile:dbg

#rm  /lib/modules/3.13.0-24-generic/kernel/net/openvswitch/openvswitch.ko
#cp /home/air/fin-ovs/openvswitch-2.3.1/datapath/linux/openvswitch.ko /lib/modules/3.13.0-24-generic/kernel/net/openvswitch
#cp utilities/ovs-ofctl /usr/local/bin/ovs-ofctl
#/sbin/modprobe openvswitch
#mkdir -p /usr/local/etc/openvswitch
#ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema
 
#ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock                      --remote=db:Open_vSwitch,Open_vSwitch,manager_options                      --private-key=db:Open_vSwitch,SSL,private_key                      --certificate=db:Open_vSwitch,SSL,certificate                      --bootstrap-ca-cert=db:Open_vSwitch,SSL,ca_cert                      --pidfile --detach


#ovs-vswitchd --pidfile --detach
#ovs-vsctl show

#exit 0 
