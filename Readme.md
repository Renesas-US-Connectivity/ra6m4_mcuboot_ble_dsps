# RA6M4 MCU Boot Image Update over DSPS (BLE)

This project demonstrates how to perform secure and high-speed firmware updates on a Renesas RA6M4 MCU using the MCUBoot bootloader over Bluetooth Low Energy (BLE) via the Dialog Serial Port Service (DSPS) protocol. The example extends the traditional MCUBoot xmodem UART flow to support wireless image delivery from a DA14592 module running DSPS.

---

## Overview

This implementation:
- Leverages MCUBoot as the secure bootloader
- Uses the DSPS protocol over BLE for data transport
- Optimizes for high-speed firmware update performance
- Supports both internal dual-bank flash and external QSPI memory

---

## 📚 References

- **MCUBoot App Note**: [RA6 MCU Advanced Secure Bootloader Design using MCUBoot and Code Flash Dual-Bank Mode](https://www.renesas.com/en/document/apn/ra6-booting-encrypted-image-using-mcuboot-and-qspi)
- **Dialog DSPS SDK**: [Serial Port Service (SPS)](https://www.renesas.com/en/software-tool/serial-port-service-sps)

---

## Required Hardware

- **RA6M4 Evaluation Kit**  
  [EK-RA6M4 Product Page](https://www.renesas.com/en/design-resources/boards-kits/ek-ra6m4#design_development)
- **DA14592 Pro Kit**  
  [DA14592-016FDEVKT-P Product Page](https://www.renesas.com/en/design-resources/boards-kits/da14592-016fdevkt-p#parametric_options)
- **Mobile Device with BLE Support**  
  - Install **Renesas SmartConsole** from iOS App Store or Google Play

---

## Project Contents

The repository includes:
- Full **source code** for bootloader and demo applications
- Pre-built **binaries** located in the release packages
- QSPI-encoded and internal flash demo images

---

## ⚙️ Hardware Setup

### 🔌 Wiring: RA6M4 to DA14592

| RA6M4 Pin | DA14592 Pin |
|-----------|-------------|
| GND       | GND         |
| P103      | P011        |
| P100      | P013        |
| P101      | P015        |
| P800      | P100        |

### DA14592 DIP Switches

- On the DA14592 board, locate **S1** (center-left).
- Set the **first four DIP switches to OFF** (toward the bottom edge).

![RA6M4 BLE Connections](resources/ble_ra6_connections.png)

---

## DA14592 DSPS Firmware Programming

1. Connect the DA14592 motherboard via USB.
2. Launch **J-Link GDB Server**:
   - Target: `Cortex-M33`
   - Interface: `SWD`, Little Endian
   - Confirm that "J-Link Connected" appears.

     ![GDB Server](resources/gdbserver.PNG)

3. Obtain cli_programmer and sps_peripheral_da14592.bin and Open a terminal in the directory and run:

    ```bash
    ./cli_programmer.exe gdbserver chip_erase_eflash
    ./cli_programmer.exe gdbserver write_eflash 0 sps_peripheral_da14592.bin
    ```

4. Press the **green reset button** on the DA14592 daughtercard.

5. Open **Renesas SmartConsole** on your mobile device and confirm you see `Renesas SPS Demo` advertising.

---

## RA6M4 Initial Image Programming

1. Open **J-Flash Lite**:
   - Device: `R7A6M4AF`
   - Interface: `SWD`
   - Frequency: `4000 kHz`

2. Select **Erase Chip**.

3. Use the file browser (`...`) to select your image obtained from release artifacts:
   - For QSPI: `app_ra6m4_primary_enc_qspi_dsps_ota_w_mcuboot.hex`
   - For Dual Bank Internal - `app_ra6m4_primary_dsps_ota_w_mcboot.hex`

4. Click **Program Device**.

5. Press the **red reset button** on the RA6M4 board.

6. You should see **3 LEDs blinking**, indicating bootloader + app are ready.

---

## 📱 Uploading the Update Image via SmartConsole App

1. **Add the update image to the app**:
   - If using iOS:
     - Open iTunes → File Sharing
     - Select **SmartConsole** and upload the binary (e.g., `app_ra6m4_secondary_enc_qspi_dsps_ota.bin`, `app_ra6m4_secondary_dsps_ota.bin` )

2. In the app:
   - Connect to `Renesas SPS Demo`
   - You should see: `UART DSPS MCU Boot Downloader Start`

3. In the hamburger menu (☰), select **Data File Streaming**:
   - Tap **Touch to select file** → choose the update binary
   - Set:
     - **Chunk size**: `244`
     - **Interval**: `1 ms`
   - Tap **Start**

4. For QSPI demos, expect a short pause around **2%** (QSPI erase).

5. Upon completion:
   - The RA6M4 will reset
   - Only the **Blue LED** should blink

---

## Performance

| Configuration                  | Image Size | Transfer Time (iOS ≥ iPhone 11) |
|-------------------------------|------------|-------------------------------|
| Dual Bank (Internal Flash)    | ~460 KB    | ~7 seconds                    |
| QSPI Encrypted (QSPI_Enc)     | ~918 KB    | ~23 seconds                   |

> ⏱️ QSPI transfers are slower due to erase and write latency.

---

## Artifacts

All necessary binaries to run the demo are artifacts tagged to release folder:

```
artifacts/
├── DA14592/
│   ├── sps_peripheral_da14592.bin
│   └── cli_programmer
├── QSPI_Enc/
│   ├── app_ra6m4_primary_enc_qspi_dsps_ota_w_mcuboot.hex
│   └── app_ra6m4_secondary_enc_qspi_dsps_ota.bin
├── Dual_Bank Internal/
├   ├── app_ra6m4_primary_dsps_ota_w_mcboot.hex
│   └── app_ra6m4_secondary_dsps_ota.bin
└── ...
```

You can run the demo without rebuilding from source.

## Building the Sample Code

To build the provided sample code in **e² studio**, please refer to the steps outlined in **MCUBoot Application Note – Section 3.2.2**.  
That section provides the required setup for e² studio, including project import, build configurations, and toolchain settings.  

Following the guidance in Section 3.2.2 ensures that the MCUBoot environment and the sample project are configured correctly for compilation and debugging.  

---

## 🧑‍💻 License

This project is provided under the **MIT License** unless otherwise noted.

---

