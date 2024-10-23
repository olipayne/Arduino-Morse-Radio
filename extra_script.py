Import("env")

# Ensure the firmware.bin is generated
def after_build(source, target, env):
    print("Building combined firmware.bin")
    env.Execute("esptool.py --chip esp32 merge_bin -o firmware.bin --flash_mode dio --flash_freq 40m --flash_size 4MB 0x1000 .pio/build/fs3_radio/bootloader.bin 0x8000 .pio/build/fs3_radio/partitions.bin 0x10000 .pio/build/fs3_radio/firmware.bin")

env.AddPostAction("buildprog", after_build)
