# Changelog

## [3.2.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v3.1.1...v3.2.0) (2026-03-07)


### Features

* redesign settings battery status for mobile-first use ([bde4d65](https://github.com/olipayne/Arduino-Morse-Radio/commit/bde4d656b748be15f1c8e115fcdccf63a53af357))

## [3.1.1](https://github.com/olipayne/Arduino-Morse-Radio/compare/v3.1.0...v3.1.1) (2026-03-07)


### Bug Fixes

* include Version.h in release-please version bumps ([999b463](https://github.com/olipayne/Arduino-Morse-Radio/commit/999b46368f84f7eb5e2ba9b3a1a9a5b25bc85cd5))
* mark firmware version define for release-please updates ([ed411c4](https://github.com/olipayne/Arduino-Morse-Radio/commit/ed411c44fdf7dc4f7f818814b61364895d40e6e7))

## [3.1.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v3.0.2...v3.1.0) (2026-03-07)


### Features

* add metrics ingestion pipeline and restore device responsiveness ([2e5cb93](https://github.com/olipayne/Arduino-Morse-Radio/commit/2e5cb935ad333fec5e08f90357c79b43cf417c0f))


### Performance Improvements

* reduce safe idle power overhead in runtime loops ([e028081](https://github.com/olipayne/Arduino-Morse-Radio/commit/e028081953d2766a2b7dd674c7aa2dac1cc794e7))

## [3.0.2](https://github.com/olipayne/Arduino-Morse-Radio/compare/v3.0.1...v3.0.2) (2026-03-07)


### Bug Fixes

* add Arduino ESP32 v3 API compatibility layer ([4a91ca6](https://github.com/olipayne/Arduino-Morse-Radio/commit/4a91ca62a9c7b8cdd6abf425478afb2eb7937b37))


### Performance Improvements

* skip redundant PWM writes and harden station JSON updates ([11fd947](https://github.com/olipayne/Arduino-Morse-Radio/commit/11fd94711b64dc986a167c5d39fb8284511be7fc))

## [3.0.1](https://github.com/olipayne/Arduino-Morse-Radio/compare/v3.0.0...v3.0.1) (2026-03-07)


### Bug Fixes

* **deps:** bump PlatformIO libraries to latest patch versions ([770806d](https://github.com/olipayne/Arduino-Morse-Radio/commit/770806db638d08bc4b9c589e3634baab2264d4ed))
* use repo token secret for release-please publishing ([e408b03](https://github.com/olipayne/Arduino-Morse-Radio/commit/e408b03bb157d1568c5a3661295c6733519c1491))

## [3.0.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v2.0.0...v3.0.0) (2026-03-07)


### ⚠ BREAKING CHANGES

* LED control moved to FreeRTOS task, battery check no longer controls LED directly
* wow c++ in the 21st century

### Features

* add automated firmware versioning from Git tags ([27f070c](https://github.com/olipayne/Arduino-Morse-Radio/commit/27f070c24e61eead429dafc8214a9beb63ba59eb))
* add automated release script ([276d6ee](https://github.com/olipayne/Arduino-Morse-Radio/commit/276d6eee2a340264257deb4052479c4280720f89))
* add battery level indicator using LED array ([1bb2f21](https://github.com/olipayne/Arduino-Morse-Radio/commit/1bb2f21452901c93cfbf712d6c471598c6af7258))
* add configurable inactivity timeout and enhanced power management ([398f8fc](https://github.com/olipayne/Arduino-Morse-Radio/commit/398f8fc60692ec487772f0a692d2a231385f20db))
* add fully automated semantic versioning and release process ([992b51e](https://github.com/olipayne/Arduino-Morse-Radio/commit/992b51e8d31700999d0003262d1d56a1531925c7))
* add simplified GitHub Actions release workflow ([bb98ae0](https://github.com/olipayne/Arduino-Morse-Radio/commit/bb98ae01a72bffa32b2a6038f427f09171a4735f))
* add station enable/disable functionality ([6d532db](https://github.com/olipayne/Arduino-Morse-Radio/commit/6d532dbaf752f026502f86f4bde700b757154303))
* **audio:** implement volume smoothing ([2054707](https://github.com/olipayne/Arduino-Morse-Radio/commit/2054707e2c7a1417f434ba86e77f7bd4dc4ac400))
* backlight and power leds functionality ([9f44db1](https://github.com/olipayne/Arduino-Morse-Radio/commit/9f44db1197bdc3d798efef6384e7ea9da0896786))
* **battery:** implement accurate LiPo discharge curve for battery percentage ([5eb4062](https://github.com/olipayne/Arduino-Morse-Radio/commit/5eb406269e736ae1e818f8047597f1a535829afe))
* comprehensive memory optimization for ESP32 efficiency ([bca311c](https://github.com/olipayne/Arduino-Morse-Radio/commit/bca311c3a17ccb9c27581def82a00729d5d2cc16))
* decode voltemeter functionality ([d8e1301](https://github.com/olipayne/Arduino-Morse-Radio/commit/d8e130109182741b92d16ca45a2bdc1dc717065e))
* dont disable wifi when plugged in ([e84a2df](https://github.com/olipayne/Arduino-Morse-Radio/commit/e84a2dfbe7bf18212eaaf75d9c3231dddb95ba1e))
* enable deep sleep on inactivity for release builds only ([072d0e6](https://github.com/olipayne/Arduino-Morse-Radio/commit/072d0e68785362a3406fefd523632f8bcb598de8))
* enhance power LED behavior with FreeRTOS task ([40ed174](https://github.com/olipayne/Arduino-Morse-Radio/commit/40ed174dfbee5af9d326a76ef5409288fd8f53c6))
* force deep sleep at critical battery level ([16e55e7](https://github.com/olipayne/Arduino-Morse-Radio/commit/16e55e7e04569ded2b29b6d837b1560be7759dbd))
* improve station configuration UI ([ad893c5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ad893c580c0bfc273143bfa4160b126b883d8570))
* improve WiFi mode UX with LED indicators and radio disabling ([98a39b4](https://github.com/olipayne/Arduino-Morse-Radio/commit/98a39b4c32677dc86de4ed84938c7803d2dba315))
* logging in morse code ([3423eda](https://github.com/olipayne/Arduino-Morse-Radio/commit/3423eda0721810e4c8965f5746c633ab40e9fcd0))
* morse leds and lock led working ([83d0996](https://github.com/olipayne/Arduino-Morse-Radio/commit/83d09966d7c0ec4402c6fd6abb39f0b6a08081e2))
* nicer web ui, json, ajax all the things ([fb3a2b9](https://github.com/olipayne/Arduino-Morse-Radio/commit/fb3a2b9c87a5f40fee652720f18fe09b19e66d78))
* oof ([1c1c481](https://github.com/olipayne/Arduino-Morse-Radio/commit/1c1c48110963d06fc78ff2717ea18171bc84335e))
* optimize power management and LED handling ([c873bef](https://github.com/olipayne/Arduino-Morse-Radio/commit/c873beff1eb753575d708b1c908ac359d30187fb))
* **ota:** add another wifi ([0f1589c](https://github.com/olipayne/Arduino-Morse-Radio/commit/0f1589cee611ef611800c310f677627460d87fdf))
* **ota:** improve OTA update UX with reversed LED order and clean start ([eafaa8b](https://github.com/olipayne/Arduino-Morse-Radio/commit/eafaa8b8cfc11e4d569c7af90eaf234f19822e64))
* **potentiometer:** implement advanced hysteresis for pot readings ([63ce58d](https://github.com/olipayne/Arduino-Morse-Radio/commit/63ce58df8aaa93cdef901f4770ddaf050f53b24b))
* power switch working, deep sleep when off ([c3fc46f](https://github.com/olipayne/Arduino-Morse-Radio/commit/c3fc46f110e2800f23f97cfa55b5618d38764dd1))
* **power:** implement momentary power switch functionality ([729b1b5](https://github.com/olipayne/Arduino-Morse-Radio/commit/729b1b56976dbf1bdefa7ff402d5fba7c0ead552))
* **power:** improve LED behavior ([14dafd7](https://github.com/olipayne/Arduino-Morse-Radio/commit/14dafd7fffe9e26f2a570c73e9216ca1f87c79c1))
* rebuild, missing some bits but meh ([5b4f735](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b4f73595340c32a001d3dd4f206e269e03704a0))
* remove deploy workflow, all in one place ([d571f82](https://github.com/olipayne/Arduino-Morse-Radio/commit/d571f82a71f60dcb6eb4694028f79026a799469b))
* show raw potentiometer value on web and improve OTA LED progress ([21df6d8](https://github.com/olipayne/Arduino-Morse-Radio/commit/21df6d84b83acaa93a1c7d658a9c6ffa0f065c00))
* simplify versioning to manual VERSION file approach ([11dc22f](https://github.com/olipayne/Arduino-Morse-Radio/commit/11dc22f1c88719df244aa951ce1c472001edf262))
* sleep stuff, WIP ([96e5327](https://github.com/olipayne/Arduino-Morse-Radio/commit/96e53274d75646367840b254ed3317aad9db97dd))
* smooth raw potentiometer readings with 5-sample moving average ([5b9b792](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b9b7926436928e14c3e27fd0761ae2230c57be2))
* task scheduler added, simplifies things a lot ([fec4b11](https://github.com/olipayne/Arduino-Morse-Radio/commit/fec4b115279865ee1fe0a10a76461aeba268d062))
* tests and adjust timing a bit ([2cfbe0a](https://github.com/olipayne/Arduino-Morse-Radio/commit/2cfbe0ab584b30867e88ae182d94475c30695608))
* tuning calibration ([d1eaa77](https://github.com/olipayne/Arduino-Morse-Radio/commit/d1eaa7707db71b6f95415ee21d621dd499077aad))
* updates now possible OTA with /update ([c54cfaf](https://github.com/olipayne/Arduino-Morse-Radio/commit/c54cfaf62fffe5f5236d91a019c55a1a38f19068))
* useless debugging removed and UMS3 v2 bump ([c7e1093](https://github.com/olipayne/Arduino-Morse-Radio/commit/c7e10939def0741a66b52028cf9b77414c88f468))
* **version:** implement automatic git-based versioning system ([65b4d3a](https://github.com/olipayne/Arduino-Morse-Radio/commit/65b4d3abf43bd0ff6140805c122c0ec2dc764bee))
* **web:** Split interface into focused message and tuning pages ([375d3a4](https://github.com/olipayne/Arduino-Morse-Radio/commit/375d3a42001161aa0ded049d6918d73c9ff9a568))
* workflow ([69be210](https://github.com/olipayne/Arduino-Morse-Radio/commit/69be2109f2e8e241a116aee698a4936a62032635))


### Bug Fixes

* **adc:** reduce crosstalk between potentiometers ([d656d89](https://github.com/olipayne/Arduino-Morse-Radio/commit/d656d89f9e45e5aa7009ad254d31342f63129318))
* always adjust volume properly ([b69d787](https://github.com/olipayne/Arduino-Morse-Radio/commit/b69d78743fe673f6856cf55518bd7894b537bfb6))
* attempt at versioning ([ed81aa5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ed81aa5e6ae321c0d7971ee7aa6556c837e2fbc1))
* audio good with refactor, finally ([ddb2088](https://github.com/olipayne/Arduino-Morse-Radio/commit/ddb2088841e086022d4ac71bf7eb1c6c198be740))
* **audio:** simplify volume control and fix zero volume detection ([7de41ec](https://github.com/olipayne/Arduino-Morse-Radio/commit/7de41ec1803901cfb5512a15df9df88eca170acb))
* back to single threaded lol ([89aed3f](https://github.com/olipayne/Arduino-Morse-Radio/commit/89aed3f40a0f286f1c04234fc1e72730e3b8fd64))
* better battery checks ([e097559](https://github.com/olipayne/Arduino-Morse-Radio/commit/e09755991a2347e5a5fba0790a1dde18a0f6c2c7))
* better wifi page ([87f142d](https://github.com/olipayne/Arduino-Morse-Radio/commit/87f142d0c64923db8abed0d0a228cf6ffdc788fa))
* build improvements ([d137111](https://github.com/olipayne/Arduino-Morse-Radio/commit/d137111ea39be4745309128ae7137620f73d7d62))
* built an OTA file duh ([db8b89f](https://github.com/olipayne/Arduino-Morse-Radio/commit/db8b89f1fbca8bf3434b556f0ad8723fe2b994f9))
* bump ArduinoJson to v7.x ([fccbd43](https://github.com/olipayne/Arduino-Morse-Radio/commit/fccbd4355ce37b6acb8cada23d5f6fe804c6c282))
* changelog first ([5682dd3](https://github.com/olipayne/Arduino-Morse-Radio/commit/5682dd3a8a85dfb6acf5ba5220ab8b6953e31405))
* check batto less often and print if on charge ([d236d90](https://github.com/olipayne/Arduino-Morse-Radio/commit/d236d90ee29029b1841d8cbdb388f8e2855896f1))
* **ci:** resolve esptool.py permission denied error in GitHub Actions ([d2152c7](https://github.com/olipayne/Arduino-Morse-Radio/commit/d2152c748b896ecdee1ae8fc572503c24ef0f1ba))
* **ci:** use pip-installed esptool instead of PlatformIO bundled version ([3a5c17a](https://github.com/olipayne/Arduino-Morse-Radio/commit/3a5c17a8afeecb4322f1f604b7fec8161932e349))
* **ci:** use python -m esptool for reliable execution ([a24cf8b](https://github.com/olipayne/Arduino-Morse-Radio/commit/a24cf8bce7dcfddb5921b44134189f06ded59934))
* clarify conventional commits requirement in release guide ([7b3243e](https://github.com/olipayne/Arduino-Morse-Radio/commit/7b3243ef671603e16419bf2f1d9adab4a934af0f))
* comment out deep sleep for now ([ba76bae](https://github.com/olipayne/Arduino-Morse-Radio/commit/ba76bae5f540550102761b22a291b628c8f4d6f4))
* configure release-please to bump VERSION file ([e0d1481](https://github.com/olipayne/Arduino-Morse-Radio/commit/e0d1481a411ac76de98c7611fcf7f1d624d98b5f))
* dark mode support ([9d4bd09](https://github.com/olipayne/Arduino-Morse-Radio/commit/9d4bd09fc329750d0ad50d3fc32620d9c30dc008))
* debug every 5 secs, and \n\r properly for javascript to be happy ([1585dca](https://github.com/olipayne/Arduino-Morse-Radio/commit/1585dca3054ff438ab0ca8e7a2b9a8fb82c7fc2c))
* debug/release flags fixes and github builds working (i hope) ([824dddf](https://github.com/olipayne/Arduino-Morse-Radio/commit/824dddf1f0b718061e14d963f49323776f55dfe5))
* divide by zero and logging ([b6a0bda](https://github.com/olipayne/Arduino-Morse-Radio/commit/b6a0bda272bfe9fd81899c9cd343ce08b355e09b))
* ensure clean rebuild with updated version file ([0a3379f](https://github.com/olipayne/Arduino-Morse-Radio/commit/0a3379fb666805c314c916934ff4d60cd3992a8d))
* fixed version file names so we can always grab latest automagically ([7fe946a](https://github.com/olipayne/Arduino-Morse-Radio/commit/7fe946afc6fe953f2e9ac05ac6278935b4282a8e))
* force a release for timbo to test ([461cc9b](https://github.com/olipayne/Arduino-Morse-Radio/commit/461cc9b5204aa72c349e36f05d49d1c011100947))
* force usb path for quicker flashing ([f4d2d51](https://github.com/olipayne/Arduino-Morse-Radio/commit/f4d2d51e9a409d6decc25171f95ede13be3070b0))
* forgot to migrate ([9014d0a](https://github.com/olipayne/Arduino-Morse-Radio/commit/9014d0aa599449c36cbeb957b50106c0857d22b7))
* get better battery average before printing ([d76da18](https://github.com/olipayne/Arduino-Morse-Radio/commit/d76da182d5d3b00e252cc73c23bc468da70b90cb))
* harden version parsing for OTA and release builds ([1f027ec](https://github.com/olipayne/Arduino-Morse-Radio/commit/1f027ec908c1b1a8113e369e10be230334f99aea))
* improve power switch reliability and deep sleep wake-up ([939e821](https://github.com/olipayne/Arduino-Morse-Radio/commit/939e82170dbf46deeba808d6f421f735a6ec969e))
* improve release workflow to verify build first ([b6f5ed8](https://github.com/olipayne/Arduino-Morse-Radio/commit/b6f5ed87f2d8327715d6428bddaa1f1c9bd83057))
* improve speaker control and volume response ([2761913](https://github.com/olipayne/Arduino-Morse-Radio/commit/27619138ffaac1b7f61e8b72fc205704c53b14fd))
* improve static noise generator ([8e14082](https://github.com/olipayne/Arduino-Morse-Radio/commit/8e1408206f6fd7eb7a9a8d198e42b1032d725d47))
* kill the power to the amp when sleeping ([198aa37](https://github.com/olipayne/Arduino-Morse-Radio/commit/198aa3715e3587ed225e8a2cf5393b948878a91b))
* let's make that pixel orange ([49d58d4](https://github.com/olipayne/Arduino-Morse-Radio/commit/49d58d487670f4071fc06e23077c8f09c106dab7))
* light sleep better ([e67e5c8](https://github.com/olipayne/Arduino-Morse-Radio/commit/e67e5c81b42be566ca25e55a2caaf548d130b6b7))
* light sleep better ([4d64948](https://github.com/olipayne/Arduino-Morse-Radio/commit/4d64948790a681ce33c25ccf243e5e269dad73da))
* lock led working ([fac8232](https://github.com/olipayne/Arduino-Morse-Radio/commit/fac82320e3540fc51332677a07ded03f56e107b1))
* logging when printing morse code ([79f9d88](https://github.com/olipayne/Arduino-Morse-Radio/commit/79f9d88567df73c7b28ed126b97f45a495c1f1cd))
* mmmmm ([6de2540](https://github.com/olipayne/Arduino-Morse-Radio/commit/6de25402f01f9041bb6b94ecbcd00e97339f1457))
* morse code and voltmeter on decode speed working ([661b0a1](https://github.com/olipayne/Arduino-Morse-Radio/commit/661b0a13eb04c3ff1657e367a8cbed401ddd0b9e))
* morse code fix ([1e88db4](https://github.com/olipayne/Arduino-Morse-Radio/commit/1e88db43c92f70b3079d247372b3214bd9383008))
* morse code now actually plays correctly lol ([3c2b1ee](https://github.com/olipayne/Arduino-Morse-Radio/commit/3c2b1eef6aeece895b781308ece27c0441e07f27))
* morse volume improvements ([e374d41](https://github.com/olipayne/Arduino-Morse-Radio/commit/e374d41289ec2fac187640212f37a8b777ce084e))
* morse_leds binary ([0263f61](https://github.com/olipayne/Arduino-Morse-Radio/commit/0263f61774758f596dd3b50c970cfada32855584))
* no need for extra_scripts.py ([f3f6bf0](https://github.com/olipayne/Arduino-Morse-Radio/commit/f3f6bf045388ba640dbf7e30dd6632c776c3a258))
* ok and new release to see how it looks ([d24a26a](https://github.com/olipayne/Arduino-Morse-Radio/commit/d24a26abf6bd21ee90e239e5a372163199e7b362))
* ok it turns out that the filenames are fine to have with version numbers ([d5deb0c](https://github.com/olipayne/Arduino-Morse-Radio/commit/d5deb0c8777dd2567fc63ca21c49e2908a9bdb5c))
* one-off commit to have a debug bin ([29daeb2](https://github.com/olipayne/Arduino-Morse-Radio/commit/29daeb27cc49b65dd831c7f5754ff8f0cfe6f51c))
* oops ([ee85951](https://github.com/olipayne/Arduino-Morse-Radio/commit/ee85951b64d74c6eab887bae1d81a2edd825672c))
* ota cleanup ([7c577ed](https://github.com/olipayne/Arduino-Morse-Radio/commit/7c577ed604f120984b660d0dcb33485f6f4a6248))
* ota fix? ([38a07a7](https://github.com/olipayne/Arduino-Morse-Radio/commit/38a07a714aa1002c6b158994a522132752e2acea))
* ota partitions ([4a054f7](https://github.com/olipayne/Arduino-Morse-Radio/commit/4a054f769cfd1ae142718add82fae76af839d8ac))
* ota working i think ([44acd4e](https://github.com/olipayne/Arduino-Morse-Radio/commit/44acd4e34cad53a2a1af5556d633787c3ac16a0f))
* ota working i think ([16cea4c](https://github.com/olipayne/Arduino-Morse-Radio/commit/16cea4ce02d37a0728632cb0eecce877fc2f9674))
* parallelize ([6c1dcc6](https://github.com/olipayne/Arduino-Morse-Radio/commit/6c1dcc68c50044114d6364372d3cc6fab3f7a3e9))
* parallelize ([117a2a9](https://github.com/olipayne/Arduino-Morse-Radio/commit/117a2a9eae86c48be116966babdc44e1bc9919fe))
* parallelize ([78db945](https://github.com/olipayne/Arduino-Morse-Radio/commit/78db945a83ab798fa7fb9a20d7bb904e5bdad8fb))
* parallelize ([8e6c153](https://github.com/olipayne/Arduino-Morse-Radio/commit/8e6c153859cc91801924e609663aa0fa4094795b))
* parallelize ([6a4fcaf](https://github.com/olipayne/Arduino-Morse-Radio/commit/6a4fcafeb30b62f781d83a0c3d22248ec2ef80f2))
* parallelize ([baa33e1](https://github.com/olipayne/Arduino-Morse-Radio/commit/baa33e17857284635edbf03f6e90cb928dc61ba0))
* potentiometer threshold raised so less spammy ([5b4f59c](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b4f59c34c18b26189c9e40ddc0fa1f8d9e0d390))
* power is connected to off, not on ([af5a600](https://github.com/olipayne/Arduino-Morse-Radio/commit/af5a600a65fad82da3131d4172c50b5acaa0099b))
* **power:** optimize deep sleep wake-up with USB+battery ([10234cb](https://github.com/olipayne/Arduino-Morse-Radio/commit/10234cb3644931879e945acb91b398da946c9cbf))
* prevent version script from overwriting Version.h during release builds ([ea763a1](https://github.com/olipayne/Arduino-Morse-Radio/commit/ea763a170811289e6f3c53267185be672c8e862e))
* production improvements ([0acb34b](https://github.com/olipayne/Arduino-Morse-Radio/commit/0acb34b929da1f1b510726663d163f2004618603))
* production improvements ([12af37d](https://github.com/olipayne/Arduino-Morse-Radio/commit/12af37d643d612f16bea92c92a33ecac2ea8946d))
* production improvements ([8bb2f19](https://github.com/olipayne/Arduino-Morse-Radio/commit/8bb2f190939c07c2b616ae600ad6e6b0467493d5))
* pulse ([8bd4398](https://github.com/olipayne/Arduino-Morse-Radio/commit/8bd4398b2516103cf23294b75ef5f0389c6d8a1b))
* pwm channels being overwritten ([0c3897d](https://github.com/olipayne/Arduino-Morse-Radio/commit/0c3897da75a9bf70d9d17908bbb19e24de1843a4))
* refactor ([00f9134](https://github.com/olipayne/Arduino-Morse-Radio/commit/00f9134eca012173b1dd8b7c9b16f4ffcdf36b70))
* refactor bits to chop it up a bit ([5645c82](https://github.com/olipayne/Arduino-Morse-Radio/commit/5645c8222534a555ab7e0a57cbc60e289f7bddcc))
* remove dead code and wrap all serial out in debug flag ([395fd75](https://github.com/olipayne/Arduino-Morse-Radio/commit/395fd7595b722b0adb9992b48ac90e495c6cf273))
* remove pwm channel interference(IDK) ([715d879](https://github.com/olipayne/Arduino-Morse-Radio/commit/715d879b0c7a0ae48b0121754ba996340feaef6c))
* reset idle time when reading the current tuning pot value ([38acfc4](https://github.com/olipayne/Arduino-Morse-Radio/commit/38acfc4ccf37342397594f51b4742117278732da))
* signal strength ([a4460bd](https://github.com/olipayne/Arduino-Morse-Radio/commit/a4460bd481ff1dca21b877866e6b2c6998294372))
* signal strength ([e5fdc1a](https://github.com/olipayne/Arduino-Morse-Radio/commit/e5fdc1a99cc282323b3798d004923c92d94f6bb8))
* small reference fixes ([c82c0f1](https://github.com/olipayne/Arduino-Morse-Radio/commit/c82c0f13fc3179c98e915f89ea60878f9da4e211))
* small stuff ([29dba1c](https://github.com/olipayne/Arduino-Morse-Radio/commit/29dba1c55e3c3e28fe5924e5a67214e5475765bd))
* small things, now it works ([0ff73ec](https://github.com/olipayne/Arduino-Morse-Radio/commit/0ff73ec9d60c43a2b08acb7816a5be48725f1330))
* spam less sleep debug data every time the second is matching ([8ebb150](https://github.com/olipayne/Arduino-Morse-Radio/commit/8ebb1507e212acc8d205ffa78142c0dd21c03a72))
* speed adjustments ([f50dfb9](https://github.com/olipayne/Arduino-Morse-Radio/commit/f50dfb96fb2c1f33541d903635be7eb4705e2952))
* static noise working again ([c607fb7](https://github.com/olipayne/Arduino-Morse-Radio/commit/c607fb77d5b9e0b0998bdf85718fdf68e0b9cd96))
* swap medium/short wave and medium/fast decode speed logic for toggle switches ([33fc957](https://github.com/olipayne/Arduino-Morse-Radio/commit/33fc9572774fa8de1a5698a3def872ccf4e97099))
* swap power and morse LEDs ([98d6073](https://github.com/olipayne/Arduino-Morse-Radio/commit/98d60733f6d7b62710b6a39a0a3ddc4e3374ab45))
* temp fix binary mode for the voltmeter ([583006a](https://github.com/olipayne/Arduino-Morse-Radio/commit/583006af48c956d0be75e0ba5afc44bd6241f315))
* this right? ([0622e64](https://github.com/olipayne/Arduino-Morse-Radio/commit/0622e64392dcce7a5e4f41c3d5fe3654b045d030))
* turn on blue ([8598c29](https://github.com/olipayne/Arduino-Morse-Radio/commit/8598c29cdf3369eff78c5a9151cc0bdc8f640f9c))
* usb to keep ota working ([a0b8bf8](https://github.com/olipayne/Arduino-Morse-Radio/commit/a0b8bf80721e67ce1dc3b6313dd5a09ba66d3ede))
* use safe release-please label names ([b7f5f03](https://github.com/olipayne/Arduino-Morse-Radio/commit/b7f5f0373008c7724d6f7d97e3431393889cd65e))
* use the neopixel to display battery in debug mode ([fa22b1c](https://github.com/olipayne/Arduino-Morse-Radio/commit/fa22b1c583008cf3a663f89bcaca38dd5c0736f6))
* versioned .bins ([578da6a](https://github.com/olipayne/Arduino-Morse-Radio/commit/578da6a2f4568b4e8704813ea39a0c14e3e1d833))
* versioned .bins ([feec1ea](https://github.com/olipayne/Arduino-Morse-Radio/commit/feec1ea1195abd49b06c332e52b1f6fd4f0fc2a9))
* voltmeter ([b28ccb4](https://github.com/olipayne/Arduino-Morse-Radio/commit/b28ccb4adf0c34d1dfff545b7c3174f020049442))
* voltmeter properly PWMing now ([6028ee0](https://github.com/olipayne/Arduino-Morse-Radio/commit/6028ee00e0670a9dd4cca99c70ab56c7dd635ad5))
* weird wifi issues solved ([291c4b7](https://github.com/olipayne/Arduino-Morse-Radio/commit/291c4b7ede0708c4caf9e878b52d5d0e91c8d6e7))
* wifi html ([a575c63](https://github.com/olipayne/Arduino-Morse-Radio/commit/a575c63635ada2ddc25793e94e43de6524a59b18))
* wifi working ([14884d0](https://github.com/olipayne/Arduino-Morse-Radio/commit/14884d0ba9e17044c943dbfaccf4e0ce2cba7527))
* worth releasing ([f15f2ce](https://github.com/olipayne/Arduino-Morse-Radio/commit/f15f2ce21881efae1c9af52cc745135cffb5ee42))
* worth releasing ([0af15a7](https://github.com/olipayne/Arduino-Morse-Radio/commit/0af15a7f92b22bd1bdedc0701963db4d51edd743))
* worth releasing ([cbb0a79](https://github.com/olipayne/Arduino-Morse-Radio/commit/cbb0a79f94fe4f5c13b1fff8ea17fccdd477f575))
* worth releasing ([cd03a8d](https://github.com/olipayne/Arduino-Morse-Radio/commit/cd03a8dc7021ee341e4d782ddd01ad7a9da0ab0b))


### Code Refactoring

* wow c++ in the 21st century ([9c1d183](https://github.com/olipayne/Arduino-Morse-Radio/commit/9c1d183b945035d8bd9cf255ba66e2cfb656c7f1))

## [2.0.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/arduino-morse-radio-v1.19.0...arduino-morse-radio-v2.0.0) (2026-03-07)


### ⚠ BREAKING CHANGES

* LED control moved to FreeRTOS task, battery check no longer controls LED directly
* wow c++ in the 21st century

### Features

* add automated firmware versioning from Git tags ([27f070c](https://github.com/olipayne/Arduino-Morse-Radio/commit/27f070c24e61eead429dafc8214a9beb63ba59eb))
* add automated release script ([276d6ee](https://github.com/olipayne/Arduino-Morse-Radio/commit/276d6eee2a340264257deb4052479c4280720f89))
* add battery level indicator using LED array ([1bb2f21](https://github.com/olipayne/Arduino-Morse-Radio/commit/1bb2f21452901c93cfbf712d6c471598c6af7258))
* add configurable inactivity timeout and enhanced power management ([398f8fc](https://github.com/olipayne/Arduino-Morse-Radio/commit/398f8fc60692ec487772f0a692d2a231385f20db))
* add fully automated semantic versioning and release process ([992b51e](https://github.com/olipayne/Arduino-Morse-Radio/commit/992b51e8d31700999d0003262d1d56a1531925c7))
* add simplified GitHub Actions release workflow ([bb98ae0](https://github.com/olipayne/Arduino-Morse-Radio/commit/bb98ae01a72bffa32b2a6038f427f09171a4735f))
* add station enable/disable functionality ([6d532db](https://github.com/olipayne/Arduino-Morse-Radio/commit/6d532dbaf752f026502f86f4bde700b757154303))
* **audio:** implement volume smoothing ([2054707](https://github.com/olipayne/Arduino-Morse-Radio/commit/2054707e2c7a1417f434ba86e77f7bd4dc4ac400))
* backlight and power leds functionality ([9f44db1](https://github.com/olipayne/Arduino-Morse-Radio/commit/9f44db1197bdc3d798efef6384e7ea9da0896786))
* **battery:** implement accurate LiPo discharge curve for battery percentage ([5eb4062](https://github.com/olipayne/Arduino-Morse-Radio/commit/5eb406269e736ae1e818f8047597f1a535829afe))
* comprehensive memory optimization for ESP32 efficiency ([bca311c](https://github.com/olipayne/Arduino-Morse-Radio/commit/bca311c3a17ccb9c27581def82a00729d5d2cc16))
* decode voltemeter functionality ([d8e1301](https://github.com/olipayne/Arduino-Morse-Radio/commit/d8e130109182741b92d16ca45a2bdc1dc717065e))
* dont disable wifi when plugged in ([e84a2df](https://github.com/olipayne/Arduino-Morse-Radio/commit/e84a2dfbe7bf18212eaaf75d9c3231dddb95ba1e))
* enable deep sleep on inactivity for release builds only ([072d0e6](https://github.com/olipayne/Arduino-Morse-Radio/commit/072d0e68785362a3406fefd523632f8bcb598de8))
* enhance power LED behavior with FreeRTOS task ([40ed174](https://github.com/olipayne/Arduino-Morse-Radio/commit/40ed174dfbee5af9d326a76ef5409288fd8f53c6))
* force deep sleep at critical battery level ([16e55e7](https://github.com/olipayne/Arduino-Morse-Radio/commit/16e55e7e04569ded2b29b6d837b1560be7759dbd))
* improve station configuration UI ([ad893c5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ad893c580c0bfc273143bfa4160b126b883d8570))
* improve WiFi mode UX with LED indicators and radio disabling ([98a39b4](https://github.com/olipayne/Arduino-Morse-Radio/commit/98a39b4c32677dc86de4ed84938c7803d2dba315))
* logging in morse code ([3423eda](https://github.com/olipayne/Arduino-Morse-Radio/commit/3423eda0721810e4c8965f5746c633ab40e9fcd0))
* morse leds and lock led working ([83d0996](https://github.com/olipayne/Arduino-Morse-Radio/commit/83d09966d7c0ec4402c6fd6abb39f0b6a08081e2))
* nicer web ui, json, ajax all the things ([fb3a2b9](https://github.com/olipayne/Arduino-Morse-Radio/commit/fb3a2b9c87a5f40fee652720f18fe09b19e66d78))
* oof ([1c1c481](https://github.com/olipayne/Arduino-Morse-Radio/commit/1c1c48110963d06fc78ff2717ea18171bc84335e))
* optimize power management and LED handling ([c873bef](https://github.com/olipayne/Arduino-Morse-Radio/commit/c873beff1eb753575d708b1c908ac359d30187fb))
* **ota:** add another wifi ([0f1589c](https://github.com/olipayne/Arduino-Morse-Radio/commit/0f1589cee611ef611800c310f677627460d87fdf))
* **ota:** improve OTA update UX with reversed LED order and clean start ([eafaa8b](https://github.com/olipayne/Arduino-Morse-Radio/commit/eafaa8b8cfc11e4d569c7af90eaf234f19822e64))
* **potentiometer:** implement advanced hysteresis for pot readings ([63ce58d](https://github.com/olipayne/Arduino-Morse-Radio/commit/63ce58df8aaa93cdef901f4770ddaf050f53b24b))
* power switch working, deep sleep when off ([c3fc46f](https://github.com/olipayne/Arduino-Morse-Radio/commit/c3fc46f110e2800f23f97cfa55b5618d38764dd1))
* **power:** implement momentary power switch functionality ([729b1b5](https://github.com/olipayne/Arduino-Morse-Radio/commit/729b1b56976dbf1bdefa7ff402d5fba7c0ead552))
* **power:** improve LED behavior ([14dafd7](https://github.com/olipayne/Arduino-Morse-Radio/commit/14dafd7fffe9e26f2a570c73e9216ca1f87c79c1))
* rebuild, missing some bits but meh ([5b4f735](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b4f73595340c32a001d3dd4f206e269e03704a0))
* remove deploy workflow, all in one place ([d571f82](https://github.com/olipayne/Arduino-Morse-Radio/commit/d571f82a71f60dcb6eb4694028f79026a799469b))
* show raw potentiometer value on web and improve OTA LED progress ([21df6d8](https://github.com/olipayne/Arduino-Morse-Radio/commit/21df6d84b83acaa93a1c7d658a9c6ffa0f065c00))
* simplify versioning to manual VERSION file approach ([11dc22f](https://github.com/olipayne/Arduino-Morse-Radio/commit/11dc22f1c88719df244aa951ce1c472001edf262))
* sleep stuff, WIP ([96e5327](https://github.com/olipayne/Arduino-Morse-Radio/commit/96e53274d75646367840b254ed3317aad9db97dd))
* smooth raw potentiometer readings with 5-sample moving average ([5b9b792](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b9b7926436928e14c3e27fd0761ae2230c57be2))
* task scheduler added, simplifies things a lot ([fec4b11](https://github.com/olipayne/Arduino-Morse-Radio/commit/fec4b115279865ee1fe0a10a76461aeba268d062))
* tests and adjust timing a bit ([2cfbe0a](https://github.com/olipayne/Arduino-Morse-Radio/commit/2cfbe0ab584b30867e88ae182d94475c30695608))
* tuning calibration ([d1eaa77](https://github.com/olipayne/Arduino-Morse-Radio/commit/d1eaa7707db71b6f95415ee21d621dd499077aad))
* updates now possible OTA with /update ([c54cfaf](https://github.com/olipayne/Arduino-Morse-Radio/commit/c54cfaf62fffe5f5236d91a019c55a1a38f19068))
* useless debugging removed and UMS3 v2 bump ([c7e1093](https://github.com/olipayne/Arduino-Morse-Radio/commit/c7e10939def0741a66b52028cf9b77414c88f468))
* **version:** implement automatic git-based versioning system ([65b4d3a](https://github.com/olipayne/Arduino-Morse-Radio/commit/65b4d3abf43bd0ff6140805c122c0ec2dc764bee))
* **web:** Split interface into focused message and tuning pages ([375d3a4](https://github.com/olipayne/Arduino-Morse-Radio/commit/375d3a42001161aa0ded049d6918d73c9ff9a568))
* workflow ([69be210](https://github.com/olipayne/Arduino-Morse-Radio/commit/69be2109f2e8e241a116aee698a4936a62032635))


### Bug Fixes

* **adc:** reduce crosstalk between potentiometers ([d656d89](https://github.com/olipayne/Arduino-Morse-Radio/commit/d656d89f9e45e5aa7009ad254d31342f63129318))
* always adjust volume properly ([b69d787](https://github.com/olipayne/Arduino-Morse-Radio/commit/b69d78743fe673f6856cf55518bd7894b537bfb6))
* attempt at versioning ([ed81aa5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ed81aa5e6ae321c0d7971ee7aa6556c837e2fbc1))
* audio good with refactor, finally ([ddb2088](https://github.com/olipayne/Arduino-Morse-Radio/commit/ddb2088841e086022d4ac71bf7eb1c6c198be740))
* **audio:** simplify volume control and fix zero volume detection ([7de41ec](https://github.com/olipayne/Arduino-Morse-Radio/commit/7de41ec1803901cfb5512a15df9df88eca170acb))
* back to single threaded lol ([89aed3f](https://github.com/olipayne/Arduino-Morse-Radio/commit/89aed3f40a0f286f1c04234fc1e72730e3b8fd64))
* better battery checks ([e097559](https://github.com/olipayne/Arduino-Morse-Radio/commit/e09755991a2347e5a5fba0790a1dde18a0f6c2c7))
* better wifi page ([87f142d](https://github.com/olipayne/Arduino-Morse-Radio/commit/87f142d0c64923db8abed0d0a228cf6ffdc788fa))
* build improvements ([d137111](https://github.com/olipayne/Arduino-Morse-Radio/commit/d137111ea39be4745309128ae7137620f73d7d62))
* built an OTA file duh ([db8b89f](https://github.com/olipayne/Arduino-Morse-Radio/commit/db8b89f1fbca8bf3434b556f0ad8723fe2b994f9))
* bump ArduinoJson to v7.x ([fccbd43](https://github.com/olipayne/Arduino-Morse-Radio/commit/fccbd4355ce37b6acb8cada23d5f6fe804c6c282))
* changelog first ([5682dd3](https://github.com/olipayne/Arduino-Morse-Radio/commit/5682dd3a8a85dfb6acf5ba5220ab8b6953e31405))
* check batto less often and print if on charge ([d236d90](https://github.com/olipayne/Arduino-Morse-Radio/commit/d236d90ee29029b1841d8cbdb388f8e2855896f1))
* **ci:** resolve esptool.py permission denied error in GitHub Actions ([d2152c7](https://github.com/olipayne/Arduino-Morse-Radio/commit/d2152c748b896ecdee1ae8fc572503c24ef0f1ba))
* **ci:** use pip-installed esptool instead of PlatformIO bundled version ([3a5c17a](https://github.com/olipayne/Arduino-Morse-Radio/commit/3a5c17a8afeecb4322f1f604b7fec8161932e349))
* **ci:** use python -m esptool for reliable execution ([a24cf8b](https://github.com/olipayne/Arduino-Morse-Radio/commit/a24cf8bce7dcfddb5921b44134189f06ded59934))
* clarify conventional commits requirement in release guide ([7b3243e](https://github.com/olipayne/Arduino-Morse-Radio/commit/7b3243ef671603e16419bf2f1d9adab4a934af0f))
* comment out deep sleep for now ([ba76bae](https://github.com/olipayne/Arduino-Morse-Radio/commit/ba76bae5f540550102761b22a291b628c8f4d6f4))
* dark mode support ([9d4bd09](https://github.com/olipayne/Arduino-Morse-Radio/commit/9d4bd09fc329750d0ad50d3fc32620d9c30dc008))
* debug every 5 secs, and \n\r properly for javascript to be happy ([1585dca](https://github.com/olipayne/Arduino-Morse-Radio/commit/1585dca3054ff438ab0ca8e7a2b9a8fb82c7fc2c))
* debug/release flags fixes and github builds working (i hope) ([824dddf](https://github.com/olipayne/Arduino-Morse-Radio/commit/824dddf1f0b718061e14d963f49323776f55dfe5))
* divide by zero and logging ([b6a0bda](https://github.com/olipayne/Arduino-Morse-Radio/commit/b6a0bda272bfe9fd81899c9cd343ce08b355e09b))
* ensure clean rebuild with updated version file ([0a3379f](https://github.com/olipayne/Arduino-Morse-Radio/commit/0a3379fb666805c314c916934ff4d60cd3992a8d))
* fixed version file names so we can always grab latest automagically ([7fe946a](https://github.com/olipayne/Arduino-Morse-Radio/commit/7fe946afc6fe953f2e9ac05ac6278935b4282a8e))
* force a release for timbo to test ([461cc9b](https://github.com/olipayne/Arduino-Morse-Radio/commit/461cc9b5204aa72c349e36f05d49d1c011100947))
* force usb path for quicker flashing ([f4d2d51](https://github.com/olipayne/Arduino-Morse-Radio/commit/f4d2d51e9a409d6decc25171f95ede13be3070b0))
* forgot to migrate ([9014d0a](https://github.com/olipayne/Arduino-Morse-Radio/commit/9014d0aa599449c36cbeb957b50106c0857d22b7))
* get better battery average before printing ([d76da18](https://github.com/olipayne/Arduino-Morse-Radio/commit/d76da182d5d3b00e252cc73c23bc468da70b90cb))
* improve power switch reliability and deep sleep wake-up ([939e821](https://github.com/olipayne/Arduino-Morse-Radio/commit/939e82170dbf46deeba808d6f421f735a6ec969e))
* improve release workflow to verify build first ([b6f5ed8](https://github.com/olipayne/Arduino-Morse-Radio/commit/b6f5ed87f2d8327715d6428bddaa1f1c9bd83057))
* improve speaker control and volume response ([2761913](https://github.com/olipayne/Arduino-Morse-Radio/commit/27619138ffaac1b7f61e8b72fc205704c53b14fd))
* improve static noise generator ([8e14082](https://github.com/olipayne/Arduino-Morse-Radio/commit/8e1408206f6fd7eb7a9a8d198e42b1032d725d47))
* kill the power to the amp when sleeping ([198aa37](https://github.com/olipayne/Arduino-Morse-Radio/commit/198aa3715e3587ed225e8a2cf5393b948878a91b))
* let's make that pixel orange ([49d58d4](https://github.com/olipayne/Arduino-Morse-Radio/commit/49d58d487670f4071fc06e23077c8f09c106dab7))
* light sleep better ([e67e5c8](https://github.com/olipayne/Arduino-Morse-Radio/commit/e67e5c81b42be566ca25e55a2caaf548d130b6b7))
* light sleep better ([4d64948](https://github.com/olipayne/Arduino-Morse-Radio/commit/4d64948790a681ce33c25ccf243e5e269dad73da))
* lock led working ([fac8232](https://github.com/olipayne/Arduino-Morse-Radio/commit/fac82320e3540fc51332677a07ded03f56e107b1))
* logging when printing morse code ([79f9d88](https://github.com/olipayne/Arduino-Morse-Radio/commit/79f9d88567df73c7b28ed126b97f45a495c1f1cd))
* mmmmm ([6de2540](https://github.com/olipayne/Arduino-Morse-Radio/commit/6de25402f01f9041bb6b94ecbcd00e97339f1457))
* morse code and voltmeter on decode speed working ([661b0a1](https://github.com/olipayne/Arduino-Morse-Radio/commit/661b0a13eb04c3ff1657e367a8cbed401ddd0b9e))
* morse code fix ([1e88db4](https://github.com/olipayne/Arduino-Morse-Radio/commit/1e88db43c92f70b3079d247372b3214bd9383008))
* morse code now actually plays correctly lol ([3c2b1ee](https://github.com/olipayne/Arduino-Morse-Radio/commit/3c2b1eef6aeece895b781308ece27c0441e07f27))
* morse volume improvements ([e374d41](https://github.com/olipayne/Arduino-Morse-Radio/commit/e374d41289ec2fac187640212f37a8b777ce084e))
* morse_leds binary ([0263f61](https://github.com/olipayne/Arduino-Morse-Radio/commit/0263f61774758f596dd3b50c970cfada32855584))
* no need for extra_scripts.py ([f3f6bf0](https://github.com/olipayne/Arduino-Morse-Radio/commit/f3f6bf045388ba640dbf7e30dd6632c776c3a258))
* ok and new release to see how it looks ([d24a26a](https://github.com/olipayne/Arduino-Morse-Radio/commit/d24a26abf6bd21ee90e239e5a372163199e7b362))
* ok it turns out that the filenames are fine to have with version numbers ([d5deb0c](https://github.com/olipayne/Arduino-Morse-Radio/commit/d5deb0c8777dd2567fc63ca21c49e2908a9bdb5c))
* one-off commit to have a debug bin ([29daeb2](https://github.com/olipayne/Arduino-Morse-Radio/commit/29daeb27cc49b65dd831c7f5754ff8f0cfe6f51c))
* oops ([ee85951](https://github.com/olipayne/Arduino-Morse-Radio/commit/ee85951b64d74c6eab887bae1d81a2edd825672c))
* ota cleanup ([7c577ed](https://github.com/olipayne/Arduino-Morse-Radio/commit/7c577ed604f120984b660d0dcb33485f6f4a6248))
* ota fix? ([38a07a7](https://github.com/olipayne/Arduino-Morse-Radio/commit/38a07a714aa1002c6b158994a522132752e2acea))
* ota partitions ([4a054f7](https://github.com/olipayne/Arduino-Morse-Radio/commit/4a054f769cfd1ae142718add82fae76af839d8ac))
* ota working i think ([44acd4e](https://github.com/olipayne/Arduino-Morse-Radio/commit/44acd4e34cad53a2a1af5556d633787c3ac16a0f))
* ota working i think ([16cea4c](https://github.com/olipayne/Arduino-Morse-Radio/commit/16cea4ce02d37a0728632cb0eecce877fc2f9674))
* parallelize ([6c1dcc6](https://github.com/olipayne/Arduino-Morse-Radio/commit/6c1dcc68c50044114d6364372d3cc6fab3f7a3e9))
* parallelize ([117a2a9](https://github.com/olipayne/Arduino-Morse-Radio/commit/117a2a9eae86c48be116966babdc44e1bc9919fe))
* parallelize ([78db945](https://github.com/olipayne/Arduino-Morse-Radio/commit/78db945a83ab798fa7fb9a20d7bb904e5bdad8fb))
* parallelize ([8e6c153](https://github.com/olipayne/Arduino-Morse-Radio/commit/8e6c153859cc91801924e609663aa0fa4094795b))
* parallelize ([6a4fcaf](https://github.com/olipayne/Arduino-Morse-Radio/commit/6a4fcafeb30b62f781d83a0c3d22248ec2ef80f2))
* parallelize ([baa33e1](https://github.com/olipayne/Arduino-Morse-Radio/commit/baa33e17857284635edbf03f6e90cb928dc61ba0))
* potentiometer threshold raised so less spammy ([5b4f59c](https://github.com/olipayne/Arduino-Morse-Radio/commit/5b4f59c34c18b26189c9e40ddc0fa1f8d9e0d390))
* power is connected to off, not on ([af5a600](https://github.com/olipayne/Arduino-Morse-Radio/commit/af5a600a65fad82da3131d4172c50b5acaa0099b))
* **power:** optimize deep sleep wake-up with USB+battery ([10234cb](https://github.com/olipayne/Arduino-Morse-Radio/commit/10234cb3644931879e945acb91b398da946c9cbf))
* prevent version script from overwriting Version.h during release builds ([ea763a1](https://github.com/olipayne/Arduino-Morse-Radio/commit/ea763a170811289e6f3c53267185be672c8e862e))
* production improvements ([0acb34b](https://github.com/olipayne/Arduino-Morse-Radio/commit/0acb34b929da1f1b510726663d163f2004618603))
* production improvements ([12af37d](https://github.com/olipayne/Arduino-Morse-Radio/commit/12af37d643d612f16bea92c92a33ecac2ea8946d))
* production improvements ([8bb2f19](https://github.com/olipayne/Arduino-Morse-Radio/commit/8bb2f190939c07c2b616ae600ad6e6b0467493d5))
* pulse ([8bd4398](https://github.com/olipayne/Arduino-Morse-Radio/commit/8bd4398b2516103cf23294b75ef5f0389c6d8a1b))
* pwm channels being overwritten ([0c3897d](https://github.com/olipayne/Arduino-Morse-Radio/commit/0c3897da75a9bf70d9d17908bbb19e24de1843a4))
* refactor ([00f9134](https://github.com/olipayne/Arduino-Morse-Radio/commit/00f9134eca012173b1dd8b7c9b16f4ffcdf36b70))
* refactor bits to chop it up a bit ([5645c82](https://github.com/olipayne/Arduino-Morse-Radio/commit/5645c8222534a555ab7e0a57cbc60e289f7bddcc))
* remove dead code and wrap all serial out in debug flag ([395fd75](https://github.com/olipayne/Arduino-Morse-Radio/commit/395fd7595b722b0adb9992b48ac90e495c6cf273))
* remove pwm channel interference(IDK) ([715d879](https://github.com/olipayne/Arduino-Morse-Radio/commit/715d879b0c7a0ae48b0121754ba996340feaef6c))
* reset idle time when reading the current tuning pot value ([38acfc4](https://github.com/olipayne/Arduino-Morse-Radio/commit/38acfc4ccf37342397594f51b4742117278732da))
* signal strength ([a4460bd](https://github.com/olipayne/Arduino-Morse-Radio/commit/a4460bd481ff1dca21b877866e6b2c6998294372))
* signal strength ([e5fdc1a](https://github.com/olipayne/Arduino-Morse-Radio/commit/e5fdc1a99cc282323b3798d004923c92d94f6bb8))
* small reference fixes ([c82c0f1](https://github.com/olipayne/Arduino-Morse-Radio/commit/c82c0f13fc3179c98e915f89ea60878f9da4e211))
* small stuff ([29dba1c](https://github.com/olipayne/Arduino-Morse-Radio/commit/29dba1c55e3c3e28fe5924e5a67214e5475765bd))
* small things, now it works ([0ff73ec](https://github.com/olipayne/Arduino-Morse-Radio/commit/0ff73ec9d60c43a2b08acb7816a5be48725f1330))
* spam less sleep debug data every time the second is matching ([8ebb150](https://github.com/olipayne/Arduino-Morse-Radio/commit/8ebb1507e212acc8d205ffa78142c0dd21c03a72))
* speed adjustments ([f50dfb9](https://github.com/olipayne/Arduino-Morse-Radio/commit/f50dfb96fb2c1f33541d903635be7eb4705e2952))
* static noise working again ([c607fb7](https://github.com/olipayne/Arduino-Morse-Radio/commit/c607fb77d5b9e0b0998bdf85718fdf68e0b9cd96))
* swap medium/short wave and medium/fast decode speed logic for toggle switches ([33fc957](https://github.com/olipayne/Arduino-Morse-Radio/commit/33fc9572774fa8de1a5698a3def872ccf4e97099))
* swap power and morse LEDs ([98d6073](https://github.com/olipayne/Arduino-Morse-Radio/commit/98d60733f6d7b62710b6a39a0a3ddc4e3374ab45))
* temp fix binary mode for the voltmeter ([583006a](https://github.com/olipayne/Arduino-Morse-Radio/commit/583006af48c956d0be75e0ba5afc44bd6241f315))
* this right? ([0622e64](https://github.com/olipayne/Arduino-Morse-Radio/commit/0622e64392dcce7a5e4f41c3d5fe3654b045d030))
* turn on blue ([8598c29](https://github.com/olipayne/Arduino-Morse-Radio/commit/8598c29cdf3369eff78c5a9151cc0bdc8f640f9c))
* usb to keep ota working ([a0b8bf8](https://github.com/olipayne/Arduino-Morse-Radio/commit/a0b8bf80721e67ce1dc3b6313dd5a09ba66d3ede))
* use safe release-please label names ([b7f5f03](https://github.com/olipayne/Arduino-Morse-Radio/commit/b7f5f0373008c7724d6f7d97e3431393889cd65e))
* use the neopixel to display battery in debug mode ([fa22b1c](https://github.com/olipayne/Arduino-Morse-Radio/commit/fa22b1c583008cf3a663f89bcaca38dd5c0736f6))
* versioned .bins ([578da6a](https://github.com/olipayne/Arduino-Morse-Radio/commit/578da6a2f4568b4e8704813ea39a0c14e3e1d833))
* versioned .bins ([feec1ea](https://github.com/olipayne/Arduino-Morse-Radio/commit/feec1ea1195abd49b06c332e52b1f6fd4f0fc2a9))
* voltmeter ([b28ccb4](https://github.com/olipayne/Arduino-Morse-Radio/commit/b28ccb4adf0c34d1dfff545b7c3174f020049442))
* voltmeter properly PWMing now ([6028ee0](https://github.com/olipayne/Arduino-Morse-Radio/commit/6028ee00e0670a9dd4cca99c70ab56c7dd635ad5))
* weird wifi issues solved ([291c4b7](https://github.com/olipayne/Arduino-Morse-Radio/commit/291c4b7ede0708c4caf9e878b52d5d0e91c8d6e7))
* wifi html ([a575c63](https://github.com/olipayne/Arduino-Morse-Radio/commit/a575c63635ada2ddc25793e94e43de6524a59b18))
* wifi working ([14884d0](https://github.com/olipayne/Arduino-Morse-Radio/commit/14884d0ba9e17044c943dbfaccf4e0ce2cba7527))
* worth releasing ([f15f2ce](https://github.com/olipayne/Arduino-Morse-Radio/commit/f15f2ce21881efae1c9af52cc745135cffb5ee42))
* worth releasing ([0af15a7](https://github.com/olipayne/Arduino-Morse-Radio/commit/0af15a7f92b22bd1bdedc0701963db4d51edd743))
* worth releasing ([cbb0a79](https://github.com/olipayne/Arduino-Morse-Radio/commit/cbb0a79f94fe4f5c13b1fff8ea17fccdd477f575))
* worth releasing ([cd03a8d](https://github.com/olipayne/Arduino-Morse-Radio/commit/cd03a8dc7021ee341e4d782ddd01ad7a9da0ab0b))


### Code Refactoring

* wow c++ in the 21st century ([9c1d183](https://github.com/olipayne/Arduino-Morse-Radio/commit/9c1d183b945035d8bd9cf255ba66e2cfb656c7f1))

## [1.8.0](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.7.10...v1.8.0) (2025-03-16)


### Features

* add automated firmware versioning from Git tags ([27f070c](https://github.com/olipayne/Arduino-Morse-Radio/commit/27f070c24e61eead429dafc8214a9beb63ba59eb))



## [1.7.10](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.7.9...v1.7.10) (2025-03-16)


### Bug Fixes

* attempt at versioning ([ed81aa5](https://github.com/olipayne/Arduino-Morse-Radio/commit/ed81aa5e6ae321c0d7971ee7aa6556c837e2fbc1))



## [1.7.9](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.7.8...v1.7.9) (2025-03-16)


### Bug Fixes

* improve static noise generator ([8e14082](https://github.com/olipayne/Arduino-Morse-Radio/commit/8e1408206f6fd7eb7a9a8d198e42b1032d725d47))



## [1.7.8](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.7.7...v1.7.8) (2025-03-16)


### Bug Fixes

* better wifi page ([87f142d](https://github.com/olipayne/Arduino-Morse-Radio/commit/87f142d0c64923db8abed0d0a228cf6ffdc788fa))



## [1.7.7](https://github.com/olipayne/Arduino-Morse-Radio/compare/v1.7.6...v1.7.7) (2025-03-16)


### Bug Fixes

* usb to keep ota working ([a0b8bf8](https://github.com/olipayne/Arduino-Morse-Radio/commit/a0b8bf80721e67ce1dc3b6313dd5a09ba66d3ede))
