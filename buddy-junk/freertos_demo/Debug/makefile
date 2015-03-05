################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/tms470_4.9.5

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./comm/comm.obj" \
"./comm/UART5.obj" \
"./comm/UART1.obj" \
"./comm/UART0.obj" \
"./comm/I2C1.obj" \
"./control/integrator.obj" \
"./control/effectors.obj" \
"./control/control.obj" \
"./control/RC.obj" \
"./driverlib/uart.obj" \
"./driverlib/timer.obj" \
"./driverlib/sysctl.obj" \
"./driverlib/interrupt.obj" \
"./driverlib/i2c.obj" \
"./driverlib/gpio.obj" \
"./drivers/rgb.obj" \
"./drivers/buttons.obj" \
"./estimation/estimation.obj" \
"./mathlib/quaternion.obj" \
"./mathlib/mathlib.obj" \
"./sensors/sensors.obj" \
"./third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/portasm.obj" \
"./third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/port.obj" \
"./third_party/FreeRTOS/Source/portable/MemMang/heap_2.obj" \
"./third_party/FreeRTOS/Source/tasks.obj" \
"./third_party/FreeRTOS/Source/queue.obj" \
"./third_party/FreeRTOS/Source/list.obj" \
"./utils/utils.obj" \
"./utils/ustdlib.obj" \
"./utils/uartstdio.obj" \
"./startup_ccs.obj" \
"./led_task.obj" \
"./freertos_demo.obj" \
"../freertos_demo_ccs.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include utils/subdir_vars.mk
-include third_party/FreeRTOS/Source/subdir_vars.mk
-include third_party/FreeRTOS/Source/portable/MemMang/subdir_vars.mk
-include third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/subdir_vars.mk
-include sensors/subdir_vars.mk
-include mathlib/subdir_vars.mk
-include estimation/subdir_vars.mk
-include drivers/subdir_vars.mk
-include driverlib/subdir_vars.mk
-include control/subdir_vars.mk
-include comm/subdir_vars.mk
-include subdir_rules.mk
-include utils/subdir_rules.mk
-include third_party/FreeRTOS/Source/subdir_rules.mk
-include third_party/FreeRTOS/Source/portable/MemMang/subdir_rules.mk
-include third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F/subdir_rules.mk
-include sensors/subdir_rules.mk
-include mathlib/subdir_rules.mk
-include estimation/subdir_rules.mk
-include drivers/subdir_rules.mk
-include driverlib/subdir_rules.mk
-include control/subdir_rules.mk
-include comm/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Hackathon\ Quad.out

# Tool invocations
Hackathon\ Quad.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual -z --stack_size=512 -m"freertos_demo_ccs.map" --heap_size=0 -i"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/lib" -i"C:/StellarisWare/driverlib/" -i"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --reread_libs --warn_sections --display_error_number --rom_model -o "Hackathon Quad.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "Hackathon Quad.out"
	-$(RM) "freertos_demo.pp" "led_task.pp" "startup_ccs.pp" "utils\uartstdio.pp" "utils\ustdlib.pp" "utils\utils.pp" "third_party\FreeRTOS\Source\list.pp" "third_party\FreeRTOS\Source\queue.pp" "third_party\FreeRTOS\Source\tasks.pp" "third_party\FreeRTOS\Source\portable\MemMang\heap_2.pp" "third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\port.pp" "sensors\sensors.pp" "mathlib\mathlib.pp" "mathlib\quaternion.pp" "estimation\estimation.pp" "drivers\buttons.pp" "drivers\rgb.pp" "driverlib\gpio.pp" "driverlib\i2c.pp" "driverlib\interrupt.pp" "driverlib\sysctl.pp" "driverlib\timer.pp" "driverlib\uart.pp" "control\RC.pp" "control\control.pp" "control\effectors.pp" "control\integrator.pp" "comm\I2C1.pp" "comm\UART0.pp" "comm\UART1.pp" "comm\UART5.pp" "comm\comm.pp" 
	-$(RM) "freertos_demo.obj" "led_task.obj" "startup_ccs.obj" "utils\uartstdio.obj" "utils\ustdlib.obj" "utils\utils.obj" "third_party\FreeRTOS\Source\list.obj" "third_party\FreeRTOS\Source\queue.obj" "third_party\FreeRTOS\Source\tasks.obj" "third_party\FreeRTOS\Source\portable\MemMang\heap_2.obj" "third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\port.obj" "third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\portasm.obj" "sensors\sensors.obj" "mathlib\mathlib.obj" "mathlib\quaternion.obj" "estimation\estimation.obj" "drivers\buttons.obj" "drivers\rgb.obj" "driverlib\gpio.obj" "driverlib\i2c.obj" "driverlib\interrupt.obj" "driverlib\sysctl.obj" "driverlib\timer.obj" "driverlib\uart.obj" "control\RC.obj" "control\control.obj" "control\effectors.obj" "control\integrator.obj" "comm\I2C1.obj" "comm\UART0.obj" "comm\UART1.obj" "comm\UART5.obj" "comm\comm.obj" 
	-$(RM) "third_party\FreeRTOS\Source\portable\CCS\ARM_CM4F\portasm.pp" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv5/utils/tiobj2bin/tiobj2bin" "Hackathon Quad.out" "Hackathon Quad.bin" "C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/ofd470" "C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/hex470" "C:/ti/ccsv5/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
