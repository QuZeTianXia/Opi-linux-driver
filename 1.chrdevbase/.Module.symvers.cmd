cmd_/home/linux-orangepi/mylinux-driver/Module.symvers := sed 's/\.ko$$/\.o/' /home/linux-orangepi/mylinux-driver/modules.order | scripts/mod/modpost -m -a   -o /home/linux-orangepi/mylinux-driver/Module.symvers -e -i Module.symvers   -T -