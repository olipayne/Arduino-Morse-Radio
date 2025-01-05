# [1.2.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.1.0...v1.2.0) (2025-01-05)


### Bug Fixes

* swap power and morse LEDs ([98d6073](https://github.com/olipayne/Arduino-Morse-Radio/commit/98d60733f6d7b62710b6a39a0a3ddc4e3374ab45))


### Features

* **power:** improve LED behavior ([14dafd7](https://github.com/olipayne/Arduino-Morse-Radio/commit/14dafd7fffe9e26f2a570c73e9216ca1f87c79c1))



# [1.1.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.0.0...v1.1.0) (2024-12-28)


### Features

* force deep sleep at critical battery level ([16e55e7](https://github.com/olipayne/Arduino-Morse-Radio/commit/16e55e7e04569ded2b29b6d837b1560be7759dbd))



# [1.0.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v0.17.0...v1.0.0) (2024-12-28)


* feat!: optimize power management and LED handling ([c873bef](https://github.com/olipayne/Arduino-Morse-Radio/commit/c873beff1eb753575d708b1c908ac359d30187fb))


### BREAKING CHANGES

* LED control moved to FreeRTOS task, battery check no longer controls LED directly



# [0.17.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v0.16.0...v0.17.0) (2024-12-28)


### Features

* enhance power LED behavior with FreeRTOS task ([40ed174](https://github.com/olipayne/Arduino-Morse-Radio/commit/40ed174dfbee5af9d326a76ef5409288fd8f53c6))



# [0.16.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v0.15.3...v0.16.0) (2024-12-28)


### Bug Fixes

* forgot to migrate ([9014d0a](https://github.com/olipayne/Arduino-Morse-Radio/commit/9014d0aa599449c36cbeb957b50106c0857d22b7))
* improve power switch reliability and deep sleep wake-up ([939e821](https://github.com/olipayne/Arduino-Morse-Radio/commit/939e82170dbf46deeba808d6f421f735a6ec969e))


### Features

* add station enable/disable functionality ([6d532db](https://github.com/olipayne/Arduino-Morse-Radio/commit/6d532dbaf752f026502f86f4bde700b757154303))
* **audio:** implement volume smoothing ([2054707](https://github.com/olipayne/Arduino-Morse-Radio/commit/2054707e2c7a1417f434ba86e77f7bd4dc4ac400))
* improve station configuration UI ([ad893c5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ad893c580c0bfc273143bfa4160b126b883d8570))



