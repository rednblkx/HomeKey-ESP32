# Esp-dsp Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased] 

### Added
- ci: add pre-commit hooks

### Changed

### Removed

## [1.4.11] 2024-02-21

### Added
- Fixed point functions mul/add/sub for esp32 and esp32s3

## [1.4.10] 2023-11-30

### Added
- Demo application for ESP32-S3-BOX-Lite Board

## [1.4.9] 2023-11-09

### Added
- New methods for Mat class to support sub-matrix operations

## [1.4.8] 2023-10-16

### Added
- Audio amplifier application for the esp32-LyraT board

## [1.4.7] 2023-07-13

### Changed
- Demo applications added to the component manager as examples 

## [1.4.6] 2023-07-11

### Added
- FIR f32 decimation filter optimized for esp32s3

### Changed
- In dsps_fird_init_f32() function the parameter start_pos removed
- In dsps_fird_f32() function the coefficients order changed to backward order

## [1.4.5] 2023-07-11

### Added
- Application for Azure IoT board
- Add Esp32-LyraT board application 

## [1.4.4] 2023-07-03

### Changed
- Update documentation build

## [1.4.1] 2023-06-30

### Fixed
- Add Bi-Quad for esp32s3 into common CMakeLists.txt 


## [1.4.0] 2023-03-29

### Added
- Complex signal generator dsps_cplx_gen()
- FIR f32 filter optimized for esp32s3
- Memcpy and Memset optimized for esp32s3

### Fixed
- Fix in tests to pass
- Minimum coeffcient length for fird_s16
- Include malloc.h into dsps_fft4r_fc32_ansi.c
- Fix for calculation length for dsps_corr_f32_axxx

### Changed

### Removed

## [1.3.0] 2023-03-10

### Added

- Fixed point FIR filter with decimation
- Update tag to 1.2.1 for component manager
- Extend dsp_power_of_two() to 32-bit value 

### Fixed

- add various links to idf_component.yml, exclude unnecessary files
- cmake: update component CMakeLists.txt file syntax to IDF v4.x+
- docs: move instructions for contributors into CONTRIBUTING.md
- docs: update README.md to use IDF component manager for installation
- drop IDF v4.0 and v4.1 support, add a CI build with IDF release/v5.0
- examples: remove GNU Make instructions from README files
- examples: allow examples to be installed from the component manager
- Fix for fft_cplx2reC_f32 function
- Wno-format-fix related errors
- Wrong path for extra component directory

### Changed

### Removed

## [1.2.0] 2022-09-22

