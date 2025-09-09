################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_public.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/caps.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_ecdsa.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/loader.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.c \
../ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.c 

C_DEPS += \
./ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_public.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/caps.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ecdsa.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/loader.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.d \
./ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.d 

OBJS += \
./ra/mcu-tools/MCUboot/boot/bootutil/src/boot_record.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_misc.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/bootutil_public.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/caps.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/encrypted.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ecdsa.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_ed25519.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_rsa.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/image_validate.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/loader.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_misc.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_move.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/swap_scratch.o \
./ra/mcu-tools/MCUboot/boot/bootutil/src/tlv.o 

SREC += \
ra_mcuboot_ra6m4_swap_enc_qspi.srec 

MAP += \
ra_mcuboot_ra6m4_swap_enc_qspi.map 


# Each subdirectory must supply rules for building sources it contributes
ra/mcu-tools/MCUboot/boot/bootutil/src/%.o: ../ra/mcu-tools/MCUboot/boot/bootutil/src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -gdwarf-4 -D_RA_CORE=CM33 -DMCUBOOT_BOOTSTRAP -D_RENESAS_RA_ -D_RA_ORDINAL=1 -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/src" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/inc" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/inc/api" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/inc/instances" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_gen" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/fsp_cfg/bsp" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/fsp_cfg" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/arm" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/arm/mbedtls/include" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/arm/mbedtls/library" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/mcu-tools/MCUboot/boot/bootutil/src" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/mcu-tools/include" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/mcu-tools/MCUboot/boot/bootutil/include" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/r_sce/crypto_procedures/src/sce9/plainkey/private/inc" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/r_sce/crypto_procedures/src/sce9/plainkey/public/inc" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/r_sce/crypto_procedures/src/sce9/plainkey/primitive" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/r_sce/common" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/r_sce" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/rm_mcuboot_port" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/fsp/src/rm_psa_crypto/inc" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/mcu-tools/include/mcuboot_config" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/mcu-tools/include/sysflash" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/arm/mbedtls" -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra_cfg/driver" -I"." -I"C:/repos/Ra6m4_dsps_OTA/MCUboot_Encryption_QSPI/ra_mcuboot_ra6m4_swap_enc_qspi/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -w -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

