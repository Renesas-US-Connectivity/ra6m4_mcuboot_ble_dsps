################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include ra_gen/subdir.mk
-include ra/mcu-tools/MCUboot/sim/mcuboot-sys/csupport/subdir.mk
-include ra/mcu-tools/MCUboot/boot/bootutil/src/subdir.mk
-include ra/fsp/src/rm_psa_crypto/subdir.mk
-include ra/fsp/src/rm_mcuboot_port/custom_crypto_stacks/protected_mode/subdir.mk
-include ra/fsp/src/rm_mcuboot_port/subdir.mk
-include ra/fsp/src/r_sce/crypto_procedures/src/sce9/plainkey/primitive/subdir.mk
-include ra/fsp/src/r_sce/crypto_procedures/src/sce9/plainkey/adaptors/subdir.mk
-include ra/fsp/src/r_qspi/subdir.mk
-include ra/fsp/src/r_ioport/subdir.mk
-include ra/fsp/src/r_flash_hp/subdir.mk
-include ra/fsp/src/bsp/mcu/all/subdir.mk
-include ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/subdir.mk
-include ra/board/ra6m4_ek/subdir.mk
-include ra/arm/mbedtls/library/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
ra_mcuboot_ra6m4_swap_enc_qspi.srec \

SECONDARY_SIZE += \
ra_mcuboot_ra6m4_swap_enc_qspi.siz \


# All Target
all: ra_mcuboot_ra6m4_swap_enc_qspi.elf secondary-outputs

# Tool invocations
ra_mcuboot_ra6m4_swap_enc_qspi.elf: $(OBJS) $(USER_OBJS) $(LINKER_SCRIPT)
	$(file > ra_mcuboot_ra6m4_swap_enc_qspi.elf.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -gdwarf-4 -T "fsp.ld" -Xlinker --gc-sections -L "C:\\repos\\ra6m4_mcuboot_ble_dsps\\MCUboot_Encryption_QSPI\\ra_mcuboot_ra6m4_swap_enc_qspi/script" -Wl,-Map,"ra_mcuboot_ra6m4_swap_enc_qspi.map" --specs=nano.specs --specs=rdimon.specs -o "ra_mcuboot_ra6m4_swap_enc_qspi.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	@echo Building target: $@ && arm-none-eabi-gcc @"ra_mcuboot_ra6m4_swap_enc_qspi.elf.in"
ra_mcuboot_ra6m4_swap_enc_qspi.srec: ra_mcuboot_ra6m4_swap_enc_qspi.elf
	arm-none-eabi-objcopy -O srec "ra_mcuboot_ra6m4_swap_enc_qspi.elf"  "ra_mcuboot_ra6m4_swap_enc_qspi.srec"
ra_mcuboot_ra6m4_swap_enc_qspi.siz: ra_mcuboot_ra6m4_swap_enc_qspi.elf
	arm-none-eabi-size --format=berkeley "ra_mcuboot_ra6m4_swap_enc_qspi.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SECONDARY_SIZE)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(SX_DEPS)  $(ASM_UPPER_DEPS)  $(SX_UPPER_DEPS)  $(OBJS)  $(SECONDARY_FLASH)  $(ASM_DEPS)  $(SREC)  $(S_UPPER_DEPS)  $(MAP) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  ra_mcuboot_ra6m4_swap_enc_qspi.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
