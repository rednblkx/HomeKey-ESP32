# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

# This file is used in CI generate binary files for different kinds of apps

import argparse
import sys
import logging
import os
from pathlib import Path
from typing import List

idf_build_apps_logger = logging.getLogger('idf_build_apps')
from idf_build_apps import App, build_apps, find_apps, setup_logging

# from idf_ci_utils import IDF_PATH, get_pytest_app_paths, get_pytest_cases, get_ttfw_app_paths

PROJECT_ROOT = Path(__file__).parent.parent.parent.absolute()
DEF_APP_PATH = PROJECT_ROOT / 'examples' / 'zigbee'
APPS_BUILD_PER_JOB = 30
BUILD_APPS_IDF = [

]
IGNORE_WARNINGS = [
    r"warning: 'init_spiffs' defined but not used",
    r"warning: 'esp_zb_gateway_board_try_update' defined but not used",
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / 'zigbee' / '.build-test-rules.yml'),
]

def _is_build_app_idf(app: App) -> bool:
    for build_app in BUILD_APPS_IDF:
        if app.name == build_app["name"] and app.target == build_app["target"]:
            return True
    return False    

def get_cmake_apps(
    paths: List[str],
    target: str,
    config_rules_str: List[str],
) -> List[App]:
    apps = find_apps(
        paths,
        recursive=True,
        target=target,
        build_dir='build_@t_@w',
        config_rules_str=config_rules_str,
        check_warnings=True,
        preserve=True,
        manifest_files=MAINFEST_FILES,
    )
    return apps

def update_component_yml_files():
    idf_path = os.environ.get('IDF_PATH')

    os.chdir(os.path.join(PROJECT_ROOT, 'examples', 'zigbee', 'esp_zigbee_gateway'))
    os.remove('main/idf_component.yml')
    os.system(f'cp {os.path.join(idf_path, "../", "esp-zboss-lib", "esp-zigbee-sdk", "tools", "managed_component_yml", "gw_idf_component.yml")} main/idf_component.yml')

    os.chdir(os.path.join(PROJECT_ROOT, 'examples', 'zigbee', 'light_sample', 'HA_on_off_switch'))
    os.remove('main/idf_component.yml')

    os.chdir(os.path.join(PROJECT_ROOT, 'examples', 'zigbee', 'light_sample', 'HA_on_off_light'))
    os.remove('main/idf_component.yml')
    os.system(f'cp {os.path.join(idf_path, "../", "esp-zboss-lib", "esp-zigbee-sdk", "tools", "managed_component_yml", "light_idf_component.yml")} main/idf_component.yml')

def main(args: argparse.Namespace) -> None:
    current_dir = os.getcwd()
    update_component_yml_files()
    os.chdir(current_dir)
    apps = get_cmake_apps(args.paths, args.target, args.config)

    # no_pytest and only_pytest can not be both True
    if args.idf_examples:
        apps_for_build = [app for app in apps if not _is_build_app_idf(app)]
    else:
        apps_for_build = apps[:]

    logging.info('Found %d apps after filtering', len(apps_for_build))
    logging.info(
        'Suggest setting the parallel count to %d for this build job',
        len(apps_for_build) // APPS_BUILD_PER_JOB + 1,
    )

    ret_code = build_apps(
        apps_for_build,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        collect_size_info=args.collect_size_info,
        # build_verbose=0,
        keep_going=True,
        ignore_warning_strs=IGNORE_WARNINGS,
        copy_sdkconfig=True,
    )

    sys.exit(ret_code)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the apps for different test types. Will auto remove those non-test apps binaries',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('paths', nargs='*', help='Paths to the apps to build.')
    parser.add_argument(
        '-t',
        '--target',
        default='all',
        help='Build apps for given target. could pass "all" to get apps for all targets',
    )
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be '
        'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, '
        'relative to the project directory, to be used. Optional NAME can be specified, '
        'which can be used as a name of this configuration. FILEPATTERN is the name of '
        'the sdkconfig file, relative to the project directory, with at most one wildcard. '
        'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        '--parallel-count', default=1, type=int, help='Number of parallel build jobs.'
    )
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--idf_examples',
        action="store_true",
        help='Only build pytest apps, definded in PYTEST_APPS_IDF',
    )
    parser.add_argument(
        '--collect-size-info',
        type=argparse.FileType('w'),
        help='If specified, the test case name and size info json will be written to this file',
    )

    arguments = parser.parse_args()
    if not arguments.paths:
        arguments.paths = [DEF_APP_PATH]
    setup_logging(verbose=1)  # Info
    main(arguments)