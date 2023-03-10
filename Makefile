## Cross-compilation commands 
CC      = arm-none-eabi-gcc
LD      = arm-none-eabi-gcc
AR      = arm-none-eabi-ar
AS      = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size

# our code
OBJS  = main.o
OBJS += stubs.o
OBJS += protocol.o

# startup files and anything else
OBJS += ./Device/Nuvoton/M031/Source/GCC/startup_M031Series.o

## Platform and optimization options
CFLAGS  = -O0 -g3 -mcpu=cortex-m0 -mthumb
CFLAGS += -Wall -ffunction-sections -fdata-sections
LFLAGS  = -T./Device/Nuvoton/M031/Source/GCC/gcc_arm_32K.ld -mcpu=cortex-m0 -mthumb -specs=nano.specs -lc -lm -Wl,--gc-sections -Wl,--Map=main.map

## Library headers
CFLAGS += -I./ 
CFLAGS += -I./CMSIS/Include/
CFLAGS += -I./StdDriver/inc/
CFLAGS += -I./Device/Nuvoton/M031/Include/

## Library objects
OBJS += ./StdDriver/src/clk.o
OBJS += ./StdDriver/src/gpio.o
OBJS += ./StdDriver/src/uart.o
OBJS += ./StdDriver/src/timer.o
OBJS += ./StdDriver/src/adc.o
OBJS += ./StdDriver/src/sys.o

OBJS += ./mcu_sdk/mcu_api.o
OBJS += ./mcu_sdk/system.o

#OBJS += ./Device/Nuvoton/M031/Source/GCC/_syscalls.o
OBJS += ./Device/Nuvoton/M031/Source/system_M031Series.o
## Rules
all: main.bin size

main.elf: $(OBJS) ./Device/Nuvoton/M031/Source/GCC/gcc_arm_32K.ld
	$(LD) $(LFLAGS) -o main.elf $(OBJS)

%.bin: %.elf
	$(OBJCOPY) --strip-unneeded -O binary $< $@

## Convenience targets
size: main.elf
	$(SIZE) $< 

clean:
	-rm -f $(OBJS) main.lst main.elf main.hex main.map main.bin main.list

.PHONY: all  size clean 