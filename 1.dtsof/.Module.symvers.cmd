cmd_/home/linux-orangepi/mylinux-driver/1.dtsof/Module.symvers := sed 's/\.ko$$/\.o/' /home/linux-orangepi/mylinux-driver/1.dtsof/modules.order | scripts/mod/modpost -m -a   -o /home/linux-orangepi/mylinux-driver/1.dtsof/Module.symvers -e -i Module.symvers   -T -