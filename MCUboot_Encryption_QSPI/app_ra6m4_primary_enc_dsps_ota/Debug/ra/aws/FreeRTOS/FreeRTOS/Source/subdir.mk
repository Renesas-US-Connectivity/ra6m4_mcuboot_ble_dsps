################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/aws/FreeRTOS/FreeRTOS/Source/event_groups.c \
../ra/aws/FreeRTOS/FreeRTOS/Source/list.c \
../ra/aws/FreeRTOS/FreeRTOS/Source/queue.c \
../ra/aws/FreeRTOS/FreeRTOS/Source/stream_buffer.c \
../ra/aws/FreeRTOS/FreeRTOS/Source/tasks.c \
../ra/aws/FreeRTOS/FreeRTOS/Source/timers.c 

C_DEPS += \
./ra/aws/FreeRTOS/FreeRTOS/Source/event_groups.d \
./ra/aws/FreeRTOS/FreeRTOS/Source/list.d \
./ra/aws/FreeRTOS/FreeRTOS/Source/queue.d \
./ra/aws/FreeRTOS/FreeRTOS/Source/stream_buffer.d \
./ra/aws/FreeRTOS/FreeRTOS/Source/tasks.d \
./ra/aws/FreeRTOS/FreeRTOS/Source/timers.d 

OBJS += \
./ra/aws/FreeRTOS/FreeRTOS/Source/event_groups.o \
./ra/aws/FreeRTOS/FreeRTOS/Source/list.o \
./ra/aws/FreeRTOS/FreeRTOS/Source/queue.o \
./ra/aws/FreeRTOS/FreeRTOS/Source/stream_buffer.o \
./ra/aws/FreeRTOS/FreeRTOS/Source/tasks.o \
./ra/aws/FreeRTOS/FreeRTOS/Source/timers.o 

SREC += \
app_ra6m4_primary_enc_dsps_ota.srec 

MAP += \
app_ra6m4_primary_enc_dsps_ota.map 


# Each subdirectory must supply rules for building sources it contributes
ra/aws/FreeRTOS/FreeRTOS/Source/%.o: ../ra/aws/FreeRTOS/FreeRTOS/Source/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -D_RA_BOOT_IMAGE -D_RA_ORDINAL=1 -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/src" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc/api" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc/instances" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/src/rm_freertos_port" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_gen" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/fsp_cfg/bsp" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/fsp_cfg" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/aws" -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/driver" -I"." -I"C:/repos/ra6m4_mcuboot_ble_dsps/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -w -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

