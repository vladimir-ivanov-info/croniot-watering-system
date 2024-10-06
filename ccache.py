Import("env")
env.Replace(CC="ccache /home/vladimir/.platformio/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-gcc")
env.Replace(CXX="ccache /home/vladimir/.platformio/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-g++")
