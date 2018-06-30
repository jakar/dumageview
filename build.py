#!/usr/bin/env python3

import argparse
import contextlib
import functools
import os
import shutil
import subprocess

from pathlib import Path

BUILD_TYPE_DEFAULT = "RelWithDebInfo"
BUILD_DIR_DEFAULT = "_build"

SCRIPT_DIR = Path(__file__).resolve().parent

def runproc(*args, **kw):
    subprocess.run(filter(None, args), check=True, **kw)

@contextlib.contextmanager
def chdir(path):
    orig_path = Path.cwd()
    os.chdir(str(path))
    yield
    os.chdir(str(orig_path))

def clean_existing(path):
    if path.is_dir():
        shutil.rmtree(str(path))
    elif path.exists():
        path.unlink()

def build(args):
    build_path = SCRIPT_DIR / args.build_dir
    if args.clean:
        clean_existing(build_path)

    build_path.mkdir(exist_ok=True)

    with chdir(build_path):
        cmake_env = os.environ.copy()
        if args.clang:
            cmake_env["CC"] = "clang"
            cmake_env["CXX"] = "clang++"
        if args.cc:
            cmake_env["CC"] = args.cc
        if args.cxx:
            cmake_env["CXX"] = args.cxx

        cmake_vars = args.cmake_vars or ()

        cmake_args = [
            "cmake",
            f"-DCMAKE_BUILD_TYPE={args.build_type}",
            f"-DCMAKE_INSTALL_PREFIX={args.install_prefix}" if args.install_prefix else None,
            "-DDUMAGEVIEW_BUILD_PLUGINS=ON" if args.plugins else None,
            ]
        cmake_args.extend(f"-D{var}" for var in cmake_vars)
        cmake_args.append("..")

        runproc(*cmake_args, env=cmake_env)

        cpus = os.cpu_count()
        runproc("make",
                "VERBOSE=1" if args.verbose else None,
                f"-j{cpus}" if cpus else None)

        if args.install:
            runproc("sudo" if args.sudo else None, "make", "install")

def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="print build commands",
        )
    parser.add_argument(
        "-c", "--clean",
        action="store_true",
        help="remove existing build directory",
        )
    parser.add_argument(
        "-p", "--plugins",
        action="store_true",
        help="build plugins with default target",
        )
    parser.add_argument(
        "-i", "--install",
        action="store_true",
        help="install after build",
        )
    parser.add_argument(
        "-S", "--sudo",
        action="store_true",
        help="use sudo for installation",
        )
    parser.add_argument(
        "-I", "--install_prefix",
        metavar="PREFIX",
        help="set installation prefix",
        )
    parser.add_argument(
        "-b", "--build-dir",
        default=BUILD_DIR_DEFAULT,
        help="set directory where cmake is run",
        )
    parser.add_argument(
        "-t", "--build_type",
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        default=BUILD_TYPE_DEFAULT,
        help="set build configuration",
        )
    parser.add_argument(
        "--clang",
        action="store_true",
        help="use Clang",
        )
    parser.add_argument(
        "--cc",
        help="set C compiler",
        )
    parser.add_argument(
        "--cxx",
        help="set C++ compiler",
        )
    parser.add_argument(
        "-D", "--cmake-vars",
        metavar="CMAKE_VAR",
        nargs="*",
        help="set options for CMake",
        )
    args = parser.parse_args()
    build(args)

if __name__ == "__main__":
    main()
