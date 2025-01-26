# CellStudio Executable Version

## Overview
This project provides the users a way to run the simulation without giving full access to the engine.

---

## System Requirements
### Linux Environment Required
- This executable is compiled for Linux and requires either:
  - Native Linux system
  - Windows Subsystem for Linux (WSL)
  - Linux virtual machine

If using Windows, install and configure WSL:
1. Open PowerShell as administrator
2. Run: `wsl --install`
3. Restart your computer
4. Complete Linux distribution setup

### Required Libraries
Install using package manager (apt/yum):
- valgrind
- libmpi (OpenMPI or similar)

---

## Usage
To run the executable:

Use the provided script to run the program:

```sh
./run_CellStudio.sh
```

To use the `read_output.py` parsing script:

Ensure that Python 3 is installed on your system to use the parsing script.
Make a copy of `read_output.py` into the output folder you want to parse.
Run the following command in the terminal:

```sh
python3 read_output.py ./
```

---

## Folder Structure
The project folder contains the following important directories and files:

```
project-folder/
├── build/                  # Build folder containing the CellStudio executable
│   ├── CellStudio          # Main executable file
│   └── Input/              # Configuration files
├── libs/                   # Libraries required for the project
├── Scripts/                # Example scripts and tools
├── read_output.py          # Python script to parse output files
└── run_CellStudio.sh       # Shell script to run the executable
```

---

## Configuration
The blackbox version allows limited configuration through the XML file located in the `input/` folder. Modify this file to control how the simulation runs.
Ensure that the XML file is properly updated before running the executable to reflect the desired simulation setup.

---

## Additional Resources
The `Scripts/` folder contains example scripts used in the engine's development:
- Build scripts
- Analysis tools
- Test automation
These are provided for reference only.

---

## Troubleshooting
### Issue: Bash script is not executable.
**Solution**: Run `chmod +x run_CellStudio.sh` to make the script executable.

### Issue: "valgrind: ./build/CellStudio: Permission denied"
**Solution**: Run `chmod +x ./build/CellStudio ` to make the script executable.

### Issue: Python script fails to run.
**Solution**: Ensure Python 3 is installed by running `python3 --version`.

