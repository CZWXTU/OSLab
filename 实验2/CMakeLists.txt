cmake_minimum_required(VERSION 3.16)

project(rpi-os VERSION 0.1.0 LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(aarch64_prefix "aarch64-none-elf")
set(aarch64_gcc "gcc")
set(aarch64_ld "ld")
set(aarch64_objdump "objdump")
set(aarch64_objcopy "objcopy")

set(aarch64_qemu "qemu-system-aarch64")
set(aarch64_gdb "gdb-multiarch")

add_subdirectory(src)
add_subdirectory(boot)

get_property(kernel_elf GLOBAL PROPERTY kernel_elf_path)
get_property(kernel_image GLOBAL PROPERTY kernel_image_path)
get_property(sd_image GLOBAL PROPERTY sd_image_path)

set(qemu_flags
    -M "raspi3"
    -nographic
    -serial "null"
    -serial "mon:stdio"
    -drive "file=${sd_image},if=sd,format=raw"
    -kernel "${kernel_image}")

add_custom_target(qemu
    COMMAND ${aarch64_qemu} ${qemu_flags}
    DEPENDS image)
add_custom_target(qemu-debug
    COMMAND ${aarch64_qemu} ${qemu_flags} -S -gdb tcp::1234
    DEPENDS image)
add_custom_target(debug
    COMMAND ${aarch64_gdb} --nx --quiet
            -ex "set architecture aarch64"
            -ex "file ${kernel_elf}"
            -ex "target remote localhost:1234"
    DEPENDS kernel)
