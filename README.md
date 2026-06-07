# Empirical Evaluation of a Layered Edge–Cloud IoT Security Architecture for Resilient Residential Access Control

This repository contains the source code, firmware, configuration schemas, and replication instructions for the stratified edge-cloud IoT security framework designed for robust residential access control.

## Project Overview
Traditional smart lock systems frequently experience slowness and susceptibility to vulnerabilities during network interruptions due to their dependence on centralized cloud computing. This research evaluates a stratified edge–cloud IoT security framework that balances local real-time responsiveness with strong security integrity.

### Key Performance Metrics
* **Overall Authentication Accuracy:** 98.0% ($n=50$)
* **False Acceptance Rate (FAR):** 0.00% (0/15 unauthorized attempts)
* **False Rejection Rate (FRR):** 2.86% (1/35 authorized attempts)
* **Local Latency Buffer:** 1,215 ms average response time during normal/offline states.
* **Edge Autonomy:** 100% functionality maintained during total network failovers.

---

## Repository Structure
```text
├── src/
│   └── main_edge_controller.ino     # ESP32 Core Firmware
├── schema/
│   └── MQTT_Payload_Schema.json     # JSON payload structure for cloud synchronization
├── LICENSE                          # OSI-approved MIT License
└── README.md                        # Project documentation
Hardware Architecture & Setup
The decentralized edge architecture is built on an ESP32 microcontroller, paired with an MFRC522 RFID module (13.56 MHz), a biometric fingerprint sensor, and a 12V Solenoid electromagnetic lock linked via a logic-level MOSFET switch.

Communication with the centralized cloud monitoring dashboard runs over MQTT utilizing its Last Will and Testament (LWT) feature for node health checks.

Citation
If you use this software or data in an academic context, please cite it as:

Jama, Y. A., Muse, K. W., & Ali, R. M. (2026). Underlying and Extended Data for: Empirical Evaluation of a Layered Edge–Cloud IoT Security Architecture for Resilient Residential Access Control. Zenodo. https://doi.org/10.5281/zenodo.19696536
