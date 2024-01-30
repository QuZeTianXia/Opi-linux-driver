cmd_/home/linux-orangepi/mylinux-driver/modules.order := {   echo /home/linux-orangepi/mylinux-driver/chrdevbase.ko; :; } | awk '!x[$$0]++' - > /home/linux-orangepi/mylinux-driver/modules.order
