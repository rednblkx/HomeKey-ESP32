# JSON Generator

[![Component Registry](https://components.espressif.com/components/espressif/json_generator/badge.svg)](https://components.espressif.com/components/espressif/json_generator)

A simple JSON (JavasScript Object Notation) generator with flushing capability.
Details of JSON can be found at [http://www.json.org/](http://www.json.org/).
The JSON strings generated can be validated using any standard JSON validator. Eg. [https://jsonlint.com/](https://jsonlint.com/)

# Files
- `src/json_generator.c`: Actual source file for the JSON generator with implementation of all APIS
- `include/json_generator.h`: Header file documenting and exposing all available APIs

# Usage

Include the C and H files in your project's build system and that should be enough.
`json_generator` requires only standard library functions for compilation
