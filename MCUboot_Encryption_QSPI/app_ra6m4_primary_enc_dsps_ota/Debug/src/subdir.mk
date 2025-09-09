################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/blinky_thread_entry.c \
../src/dsps_downloader_thread_entry.c \
../src/hal_entry.c \
../src/qspi_operations.c \
../src/ring_buffer.c \
../src/uart_printf.c 

C_DEPS += \
./src/blinky_thread_entry.d \
./src/dsps_downloader_thread_entry.d \
./src/hal_entry.d \
./src/qspi_operations.d \
./src/ring_buffer.d \
./src/uart_printf.d 

OBJS += \
./src/blinky_thread_entry.o \
./src/dsps_downloader_thread_entry.o \
./src/hal_entry.o \
./src/qspi_operations.o \
./src/ring_buffer.o \
./src/uart_printf.o 

SREC += \
app_ra6m4_primary_enc_dsps_ota.srec 

MAP += \
app_ra6m4_primary_enc_dsps_ota.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O1 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -D_RA_BOOT_IMAGE -D_RA_ORDINAL=1 -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/src" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc/api" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/inc/instances" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/fsp/src/rm_freertos_port" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_gen" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/fsp_cfg/bsp" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/fsp_cfg" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/aws" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra_cfg/driver" -I"." -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/app_ra6m4_primary_enc_dsps_ota/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

