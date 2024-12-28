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



## [0.15.3](https://github.com/olipayne/Arduino-Morse-Radio/compare/v0.15.2...v0.15.3) (2024-12-28)


### Bug Fixes

* **adc:** reduce crosstalk between potentiometers ([d656d89](https://github.com/olipayne/Arduino-Morse-Radio/commit/d656d89f9e45e5aa7009ad254d31342f63129318))



