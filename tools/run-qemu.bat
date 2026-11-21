@echo off
REM
if not defined QNX_IMAGE_DIR (
    echo QNX_IMAGE_DIR is not set. Point it to the folder with the image built by mkqnximage.
    echo Example: set QNX_IMAGE_DIR=C:\path\to\qnximage
    exit /b 1
)
set PATH=%PATH%;C:\Program Files\qemu
cd /d "%QNX_IMAGE_DIR%"
qemu-system-x86_64 -smp 2 -cpu max -m 1G -drive file=output/disk-qemu.vmdk,if=ide,id=drv0 -pidfile output/qemu.pid -kernel output/ifs.bin -serial mon:stdio -nographic -object rng-builtin,id=rng0 -device virtio-rng-pci,rng=rng0 -netdev user,id=n0,hostfwd=tcp::2222-:22 -device virtio-net-pci,netdev=n0