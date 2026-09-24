# Challenge 1 — Image Filtering and Denoising

## Overview

This project is part of a Numerical Linear Algebra assignment on image filtering and denoising. It uses a grayscale deer image to explore convolution filters for smoothing, sharpening, and edge detection. The assignment also represents these operations as sparse matrix-vector products and uses the Eigen and LIS libraries to solve linear systems iteratively.

The current `challenge.cpp` implementation covers the first two tasks: loading the image into an Eigen matrix, adding random noise, and exporting the noisy image as a PNG. The remaining tasks are intended to be completed collaboratively by the team.

This project uses the course image `quay.io/pjbaioni/amsc_mk:2025`. The container provides the scientific computing environment; the team will write the assignment code in `challenge.cpp`.

Keep the input image (`deer.jpg`) in the project directory. The stb headers are organized in the `stb/` subdirectory. Mount this directory in the container as `/shared-folder` so that source files, input images, and generated results are shared with the host computer.

## macOS (Docker)

Install and start Docker Desktop. In the Mac Terminal, download the course image:

```bash
docker pull quay.io/pjbaioni/amsc_mk:2025
```

Create the `amsc` container and mount the project directory at `/shared-folder`. Replace the path below if your project is stored elsewhere:

```bash
docker run --platform linux/amd64 -it --name amsc \
  -v /Users/yahiaghallale/Documents/NLA_Challenge1:/shared-folder \
  quay.io/pjbaioni/amsc_mk:2025 /bin/bash
```

On an Intel Mac, you can omit `--platform linux/amd64`. To enter the container again later, run these commands in the Mac Terminal:

```bash
docker start amsc
docker exec -it amsc /bin/bash
```

Do not run `docker run` again once the `amsc` container has been created. Use `docker start` and `docker exec` to reopen it.

## Windows (WSL2 + Ubuntu + Apptainer)

The course recommends using WSL with Ubuntu 24.04. Install WSL from PowerShell (as Administrator), then open Ubuntu:

```powershell
wsl --install -d Ubuntu-24.04
```

In the Ubuntu terminal, install Apptainer using the Linux instructions below. Windows files are available under `/mnt/c`; for example, `C:\Users\Name\Documents\NLA_Challenge1` maps to `/mnt/c/Users/Name/Documents/NLA_Challenge1`. For better performance, it is preferable to work in the Linux filesystem, for example `~/NLA_Challenge1`.

## Linux (Apptainer)

Install Apptainer. On Ubuntu:

```bash
sudo apt update
sudo apt install -y software-properties-common
sudo add-apt-repository -y ppa:apptainer/ppa
sudo apt update
sudo apt install -y apptainer
```

Download the course image once:

```bash
apptainer pull docker://quay.io/pjbaioni/amsc_mk:2025
```

This creates `amsc_mk_2025.sif` in the current directory. Start a shell and bind the current directory into the container as `/shared-folder`:

```bash
apptainer shell --bind "$PWD:/shared-folder" amsc_mk_2025.sif
```

If the SIF file is in another directory, provide its full path.

## Environment and compilation (Windows/WSL or Linux)

Once inside the container, load the environment and enter the shared project directory:

```bash
source /u/sw/etc/bash.bashrc
module load gcc-glibc
cd /shared-folder
```

LIS is included in the course image. Load its module if available:

```bash
module avail lis
module load lis
```

When `challenge.cpp` is implemented, compile it with Eigen and LIS. If the LIS module defines the standard `mkLisInc` and `mkLisLib` variables, use:

```bash
g++ -O2 -std=c++17 -I"$mkEigenInc" -I"$mkLisInc" challenge.cpp \
  -L"$mkLisLib" -Wl,-rpath,"$mkLisLib" -llis -o challenge
```

If those variables are not defined, LIS 2.0.30 is installed at this path in the course container:

```bash
g++ -O2 -std=c++17 -I"$mkEigenInc" \
  -I/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/include challenge.cpp \
  -L/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/lib \
  -Wl,-rpath,/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/lib \
  -llis -o challenge
```

Run the program with the input image:

```bash
./challenge deer.jpeg
```

Files generated in `/shared-folder` will also be available in the project directory on the host computer.

## References

- `Challenge1.pdf`: assignment requirements and filters.
- `Lab0/Lab0a_SetUp.md`: Linux and Windows setup and course container instructions.
- `Lab1/Lab1_IntroEigen.md`: Eigen and image handling with stb.
