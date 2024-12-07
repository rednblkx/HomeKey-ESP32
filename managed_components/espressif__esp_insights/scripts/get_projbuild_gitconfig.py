# This file is expected to be present in ${COMPONENT_DIR}
# accessed from components/esp_insights/CMakeLists.txt
# Used in:
# 1. Project ESP Insights build package tar file

#from __future__ import unicode_literals
import os
import sys
import json
import subprocess
from builtins import range, str
from io import open

# Input project directory from CMakeLists.txt
PROJ_DIR=sys.argv[1]
# Input project name
PROJ_NAME=sys.argv[2]
# Input project version
PROJ_VER=sys.argv[3]
# Input custom config filename from CMakeLists.txt
FILENAME=sys.argv[4]
# Input IDF_PATH from CMakeLists.txt
IDF_PATH=sys.argv[5]
# Toolchain Prefix
TOOLCHAIN_PREFIX=sys.argv[6]

NEWLINE = "\n"

CONFIG = {}

# Set Config

# Set current directory i.e Set ${COMPONENT_DIR} as current directory
CURR_DIR = os.getcwd()

def _change_dir(dirname):
    # Change directory
    os.chdir(dirname)


def _set_submodule_cfg(submodules, repo_name):
    # Set config for submodules
    CFG_TITLE = "submodules"
    NAME_STR = "name"
    VERSION_STR = "version"
    CONFIG[repo_name][CFG_TITLE] = []

    if submodules:
        # Get the submodule name and version
        submodules_list = submodules.strip().split(NEWLINE)
        for i in range(0, len(submodules_list), 2):
            name = submodules_list[i].split('\'')[1]
            version = submodules_list[i+1]
            submodule_json = { NAME_STR: name, VERSION_STR: version }
            CONFIG[repo_name][CFG_TITLE].append(submodule_json)


def run_cmd(command, get_basename=False):
    try:
        resp = subprocess.check_output(command, shell=True).strip().decode('utf-8')
        if get_basename:
            resp = os.path.basename(resp)
        return resp
    except subprocess.CalledProcessError:
        raise Exception("ERROR: Please check command : {}".format(command))

def set_cfg(config_name):
    # Set config for ESP-IDF Repo
    if config_name  == "esp-idf":
        # Get repo name (for IDF repo)
        REPO_CMD='git rev-parse --show-toplevel'
        repo_name = run_cmd(REPO_CMD, get_basename=True)
        CONFIG[repo_name] = {}

        # Get commit HEAD
        GITHEAD_STR = "HEAD"
        HEAD='git describe --always --tags --dirty'
        head_ver = run_cmd(HEAD)
        CONFIG[repo_name][GITHEAD_STR] = head_ver

        # Get submodule latest refs
        SUBMODULE = 'git submodule foreach git describe --always --tags --dirty'
        submodules = run_cmd(SUBMODULE)
        _set_submodule_cfg(submodules, repo_name)
    elif config_name == "toolchain":
        # Get toolchain version
        TOOLCHAIN_STR = "toolchain"
        TOOLCHAIN = TOOLCHAIN_PREFIX + 'gcc --version'
        toolchain = run_cmd(TOOLCHAIN)
        CONFIG[TOOLCHAIN_STR] = toolchain.strip().split(NEWLINE)[0]

# Set project details - name and version
def set_project_details():
    # Set project name and version
    CONFIG['project'] = {}
    CONFIG['project']['name'] = PROJ_NAME
    CONFIG['project']['version'] = PROJ_VER

try:
    with open(FILENAME, "w+", encoding="utf-8") as output_file:
        # ESP-IDF REPO CONFIG
        # Change to ESP-IDF Directory
        _change_dir(IDF_PATH)
        set_cfg("esp-idf")

        # Change back to ${COMPONENT_DIR}
        _change_dir(CURR_DIR)

        # Set project name and version
        set_project_details()

        # GET TOOLCHAIN VERSION
        set_cfg("toolchain")

        output_file.write(str(json.dumps(CONFIG, indent=4, sort_keys=True)))

except Exception as e:
    # Remove config file created if error occurs
    os.system("rm " + FILENAME)
    sys.exit(e)