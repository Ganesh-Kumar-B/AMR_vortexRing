# AMR Vortex Ring Collision using OpenFOAM v2412

## Overview

This repository contains an OpenFOAM implementation for simulating vortex ring collisions with adaptive mesh refinement (AMR) based on the magnitude of vorticity.

The project includes:

* A custom utility (`createVortexRing`) for initializing the vortex-ring velocity field.
* A modified solver (`pimpleFoamVortRefine`) that computes vorticity and vorticity magnitude during runtime for mesh refinement.
* A complete OpenFOAM case setup.
* Parallel execution support using MPI.
* Dynamic mesh refinement based on `magVorticity`.

---

## Repository Structure

```text
AMR_vortexRing/
├── case/
│   ├── 0.orig/
│   ├── constant/
│   ├── system/
│   ├── vortex_ring.unv
│   ├── Allrun
│   └── Allclean
│
├── solvers/
│   └── pimpleFoamVortRefine/
│
├── utilities/
│   └── createVortexRing/
│
└── README.md
```

---

## Requirements

This project has been tested with:

* OpenFOAM v2412 (OpenCFD release)
* MPI
* ParaView

Ensure OpenFOAM is sourced before building or running:

```bash
source /usr/lib/openfoam/openfoam2412/etc/bashrc
```

Verify:

```bash
echo $WM_PROJECT_VERSION
```

Expected output:

```text
2412
```

---

## Building the Applications

### Build the vortex-ring initialization utility

```bash
cd utilities/createVortexRing
wmake
```

### Build the AMR solver

```bash
cd ../../solvers/pimpleFoamVortRefine
wmake
```

After successful compilation, the following executables should be available:

```text
createVortexRing
pimpleFoamVortRefine
```

---

## Case Preparation

Move to the case directory:

```bash
cd case
```

### Step 1: Import the mesh

```bash
ideasUnvToFoam vortex_ring.unv
```

This converts the mesh generated externally into OpenFOAM format.

---

### Step 2: Create the initial time directory

```bash
cp -r 0.orig 0
```

This creates the OpenFOAM initial condition directory.

---

### Step 3: Initialize the vortex rings

```bash
createVortexRing
```

This utility generates:

* Velocity field (`U`)
* Vorticity field (`vorticity`)
* Magnitude of vorticity (`magVorticity`)

at the initial time.

---

### Step 4: Decompose the case

```bash
decomposePar -force
```

This prepares the case for parallel execution.

---

### Step 5: Run the simulation

```bash
mpirun --oversubscribe -np 6 pimpleFoamVortRefine -parallel
```

where:

* `-np 6` uses 6 MPI processes.
* `--oversubscribe` allows MPI to use more processes than available physical cores if necessary.

---

## Adaptive Mesh Refinement

The solver uses OpenFOAM's `dynamicRefineFvMesh`.

Refinement is performed based on:

```text
magVorticity
```

The vorticity field is recomputed during runtime and used to dynamically refine and unrefine the mesh around vortex structures.

The AMR settings can be modified in:

```text
constant/dynamicMeshDict
```

---

## Visualisation

The simulation can be visualized while running.

Open ParaView:

```bash
paraview
```

Open the decomposed case directly:

```text
processor0/
```

or

```text
case.foam
```

if available.

No reconstruction is required during runtime.

Useful fields:

* U
* p
* vorticity
* magVorticity
* cellLevel

Coloring by `cellLevel` allows visualization of the adaptive mesh refinement levels.

---

## Reconstructing Results

After the simulation is complete:

```bash
reconstructPar
```

or

```bash
reconstructPar -latestTime
```

to reconstruct the decomposed fields.

---

## Cleaning the Case

To remove generated data:

```bash
./Allclean
```

or manually remove:

```bash
processor*
[0-9]*
```

while preserving:

```text
0.orig
constant
system
```

---

## Notes

* This repository is intended for OpenFOAM v2412.
* The solver computes vorticity and vorticity magnitude internally.
* Dynamic mesh refinement is driven by the vorticity magnitude field.
* The case is configured for parallel execution.
* ParaView can directly visualize the decomposed AMR mesh without reconstruction.

---

## Author

Ganesh Kumar B

M.S. (Engineering Mechanics)
Jawaharlal Nehru Centre for Advanced Scientific Research (JNCASR)

Research interests:

* Vortex dynamics
* Lattice Boltzmann methods
* Computational fluid dynamics
* High-performance scientific computing

