import os
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)
