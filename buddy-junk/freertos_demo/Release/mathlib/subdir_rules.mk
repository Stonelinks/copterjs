################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
mathlib/mathlib.obj: ../mathlib/mathlib.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/bmichini/Documents/Projects/HackathonQuad/freertos_demo" --include_path="C:/Users/bmichini/Documents/boards/ek-lm4f120xl" --include_path="C:/Users/bmichini/Documents" --include_path="C:/Users/bmichini/Documents/third_party" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS/Source/include" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="mathlib/mathlib.pp" --obj_directory="mathlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

mathlib/quaternion.obj: ../mathlib/quaternion.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/bmichini/Documents/Projects/HackathonQuad/freertos_demo" --include_path="C:/Users/bmichini/Documents/boards/ek-lm4f120xl" --include_path="C:/Users/bmichini/Documents" --include_path="C:/Users/bmichini/Documents/third_party" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS/Source/include" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS" --include_path="C:/Users/bmichini/Documents/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="mathlib/quaternion.pp" --obj_directory="mathlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


