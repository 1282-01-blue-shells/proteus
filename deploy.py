#!/usr/bin/env python3

import sys

def fail(msg: str):
    sys.stderr.write(f"error: {msg}\n")
    exit(1)

if sys.version_info[0] != 3:
    fail("this script requires a Python 3 interpreter")

from pathlib import Path
import platform
import shutil
import subprocess

def sd_card_dir() -> Path:
    label = "FEHSD"

    os = platform.system()
    if os == "Windows":
        wmic_output = [line for line in subprocess.getoutput(
            "WMIC VOLUME GET DriveLetter,Label /VALUE"
        ).splitlines() if len(line) > 0]
        try:
            label_index = wmic_output.index(f"Label={label}")
        except:
            fail("SD card is not inserted")
        # The `DriveLetter=...` line immediately precedes the `Label=...` one.
        drive_letter = wmic_output[label_index - 1].removeprefix("DriveLetter=")
        return Path(f"{drive_letter}\\")
    elif os == "Darwin":
        return Path(f"/Volumes/{label}")
    elif os == "Linux":
        return Path(f"/media/{label}")
    else:
        fail("unknown platform")

def main():
    if len(sys.argv) == 1:
        print("OVERVIEW: A tool to install an application to the Proteus SD card.")
        print()
        print("<app-name> is the name of application to be installed. This is equivalent to the")
        print("basename of the directory containing the application.")
        print("If the SD card is not inserted, the script will exit with an error message.")
        print("Otherwise, the .S19 file corresponding to the application is copied to the SD card.")
        print()
        print("USAGE: deploy.py <app-name>")
        return

    app = sys.argv[1]
    local_s19 = Path(f"Build/{app}.s19")
    if not local_s19.exists():
        fail(f"<{local_s19}> does not exist")
    remote_s19 = sd_card_dir() / "CODE.S19"

    print(f"Copying <{local_s19}> to <{remote_s19}>...", end="")
    try:
        shutil.copyfile(local_s19, remote_s19, follow_symlinks=False)
    except:
        print("\n")
        fail("failed to copy to the SD card")
    print(" done")

if __name__ == "__main__":
    main()
