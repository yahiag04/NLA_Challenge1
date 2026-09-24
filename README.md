# Challenge 1 — Image filtering and denoising

Il progetto usa l'immagine del corso `quay.io/pjbaioni/amsc_mk:2025`. Il container include l'ambiente scientifico; il codice dell'esercizio va scritto in `challenge.cpp` dal team.

L'immagine `deer.jpg`, `stb_image.h` e `stb_image_write.h` devono trovarsi nella cartella del progetto. Il container deve montare questa cartella come `/shared-folder`, così sorgenti, immagini e risultati sono condivisi con il computer host.

## macOS (Docker)

Installa e avvia Docker Desktop. Dal Terminale del Mac, scarica l'immagine del corso:

```bash
docker pull quay.io/pjbaioni/amsc_mk:2025
```

Crea il container `amsc`, collegando la directory del progetto a `/shared-folder` nel container. Sostituisci il percorso qui sotto se la cartella del progetto è in un'altra posizione:

```bash
docker run --platform linux/amd64 -it --name amsc \
  -v /Users/yahiaghallale/Documents/NLA_Challenge1:/shared-folder \
  quay.io/pjbaioni/amsc_mk:2025 /bin/bash
```

Se il Mac ha un processore Intel, puoi rimuovere `--platform linux/amd64`. Per gli accessi successivi, dal Terminale del Mac esegui:

```bash
docker start amsc
docker exec -it amsc /bin/bash
```

Non usare di nuovo `docker run` se il container `amsc` esiste già: per entrare si usano `docker start` e `docker exec`.

## Windows (WSL2 + Ubuntu + Apptainer)

Le istruzioni del corso raccomandano WSL con Ubuntu 24.04. Installa WSL da PowerShell (come amministratore), poi apri Ubuntu:

```powershell
wsl --install -d Ubuntu-24.04
```

Nel terminale Ubuntu installa Apptainer seguendo i comandi Linux sotto. I file Windows sono accessibili da `/mnt/c`; per esempio `C:\Users\Nome\Documents\NLA_Challenge1` diventa `/mnt/c/Users/Nome/Documents/NLA_Challenge1`. È preferibile lavorare in una directory del filesystem Linux, ad esempio `~/NLA_Challenge1`, per prestazioni migliori.

## Linux (Apptainer)

Installa Apptainer. Su Ubuntu:

```bash
sudo apt update
sudo apt install -y software-properties-common
sudo add-apt-repository -y ppa:apptainer/ppa
sudo apt update
sudo apt install -y apptainer
```

Scarica l'immagine del corso una volta:

```bash
apptainer pull docker://quay.io/pjbaioni/amsc_mk:2025
```

Questo crea `amsc_mk_2025.sif` nella directory corrente. Per avviare una shell con la directory corrente accessibile nel container:

```bash
apptainer shell --bind "$PWD:/shared-folder" amsc_mk_2025.sif
```

Se il file SIF è in un'altra directory, indica il suo percorso completo.

## Preparazione e compilazione (Windows/WSL o Linux)

Una volta dentro il container, carica l'ambiente e vai nella cartella condivisa:

```bash
source /u/sw/etc/bash.bashrc
module load gcc-glibc
cd /shared-folder
```

LIS è fornito nell'immagine del corso. Carica il modulo se disponibile:

```bash
module avail lis
module load lis
```

Quando `challenge.cpp` sarà implementato, compila con Eigen e LIS. Se il modulo LIS imposta le variabili standard `mkLisInc` e `mkLisLib`:

```bash
g++ -O2 -std=c++17 -I"$mkEigenInc" -I"$mkLisInc" challenge.cpp \
  -L"$mkLisLib" -Wl,-rpath,"$mkLisLib" -llis -o challenge
```

Se le variabili non sono definite, nel container del corso LIS 2.0.30 è installato qui:

```bash
g++ -O2 -std=c++17 -I"$mkEigenInc" \
  -I/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/include challenge.cpp \
  -L/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/lib \
  -Wl,-rpath,/u/sw/toolchains/gcc-glibc/11.2.0/pkgs/lis/2.0.30/lib \
  -llis -o challenge
```

Avvia il programma passando l'immagine:

```bash
./challenge deer.jpg
```

I file generati nella directory `/shared-folder` saranno disponibili anche nella cartella del progetto sul computer host.

## Riferimenti

- `Challenge1.pdf`: consegna e filtri richiesti.
- Materiale `Lab0/Lab0a_SetUp.md`: setup Linux/Windows e container del corso.
- Materiale `Lab1/Lab1_IntroEigen.md`: Eigen e gestione immagini con stb.
# NLA_Challenge1
