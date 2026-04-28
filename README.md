# Reproducibility Code and Data for: Parameter Tuning of Genetic Algorithm for the Capacitated Vehicle Routing Problem Using Multi-Phase Design of Experiments

## Description
This repository contains the source code, raw data, and statistical analysis scripts required to reproduce the findings presented in the article "Parameter Tuning of Genetic Algorithm for the Capacitated Vehicle Routing Problem Using Multi-Phase Design of Experiments". It includes the C++ implementation of the Genetic Algorithm (GA) framework—specifically focusing on various constraint-adaptive initialization methods—and the Python scripts used for the multi-phase Design of Experiments (DoE) statistical validation.

## Code Information
The repository is divided into two main components:

### 1. C++ Source Code (GA Simulation)
This section contains the core Genetic Algorithm framework. The population initialization strategies discussed in the paper are modularized into the following files:
* `CARI.cpp`: Constraint-Aware Random Initialization.
* `CANN.cpp`: Constraint-Adaptive Nearest Neighbor.
* `CAKNN2.cpp` to `CAKNN5.cpp`: Constraint-Adaptive k-Nearest Neighbor variants (with neighborhood size  = 2, 3, 4, 5).
* `HNRI.cpp`: Hybrid Neighbor Random Initialization.

### 2. Python Scripts (Statistical Analysis)
* `phase_1_script.py`: Processes Phase 1 data to find the optimal uniform population size.
* `phase_2_script.py`: Processes Phase 2 data to perform normality tests (Shapiro-Wilk), ANOVA, Kruskal-Wallis tests, and generates the Main Effects Table and Figure 2.
* `phase_3_script.py`: Processes Phase 3 data to calculate the Relative Percentage Deviation (RPD), Efficiency Gain, performs the Wilcoxon signed-rank test, and generates Table 8 and Figure 3.

## Dataset & Execution Workflow (C++)

To maximize execution speed, the GA implementation relies on hardcoded data inputs. Before compiling, you must manually configure the benchmark data and parameters directly inside the respective C++ source files (e.g., `CAKNN2.cpp`, `CARI.cpp`).

### Step 1: Input Data Configuration (Manual Setup)
To facilitate easy testing of other benchmark instances without requiring you to parse raw `.vrp` files, we have provided a helper file named `Benchmark_Data_Reference.txt`.

If you wish to evaluate a different dataset (ranging from 106 to 1001 nodes), please follow these steps:
1. Open the provided `Benchmark_Data_Reference.txt` file.
2. Locate the specific benchmark instance you want to test (numbered 1 to 10).
3. Copy the entire block of code for that instance (which includes `coordinates`, `demand`, `vehicleCapacity`, `numVehicle`, and `benchmarkID`).
4. Open the target C++ source file (e.g., `CAKNN2.cpp`) and locate the `//------initialization` section at the top.
5. Replace the existing variables with the block of code you just copied.

### Step 2: Evolutionary Parameter Setup
Configure the parameters based on the experimental phase:
* `populationSize` (e.g., 300)
* `generations` (e.g., 500)
* `crossoverRate` (e.g., 0.8)
* `mutationRate` (e.g., 0.2)
* `breederRate` (e.g., 0.3)
* `run`: Number of independent continuous runs (default is `20`).

### Step 3: Compile and Run
Compile the C++ code using Visual Studio 2022 (or any C++14/17 compatible compiler). The algorithm will automatically execute sequentially for the number of times specified in the `run` variable. 

Output: The C++ program produces detailed CSV logs (e.g., `FoG(HNRI)=X-n1001-k43...csv`) containing generation-by-generation route structures and fitness traces.

## Executing the Statistical Analysis (Python)

To reproduce the statistical tables and figures presented in the paper, the detailed C++ outputs must be aggregated into summary CSV files. Pre-aggregated datasets are provided in this repository:
* `dat.csv`: Aggregated simulation data for Phase 1.
* `dat2.csv`: Aggregated simulation data for Phase 2.
* `dat3-doe.csv`: Aggregated simulation data for Phase 3.

**Instructions:**
1. Ensure Python 3.8+ is installed along with dependencies: `pip install pandas numpy scipy statsmodels matplotlib seaborn`.
2. Update the `input_path` and `output_directory` variables at the bottom of each Python script to match your local folder structure.
3. Run Phase 1: `python phase_1_script.py` (Outputs Summary CSVs).
4. Run Phase 2: `python phase_2_script.py` (Outputs Validation CSVs, Main Effects Table, and Figure 2).
5. Run Phase 3: `python phase_3_script.py` (Outputs Table 8 and Figure 3).

## Methodology
The analytical methodology follows a three-phase experimental framework to systematically tune the GA:
1. Phase 1 (Population Sizing): Evaluates population sizes (50, 100, 200, 300) across selected benchmark instances to establish a robust baseline.
2. Phase 2 (Design of Experiments): Employs a fractional factorial design to assess the main effects of crossover, mutation, and breeder rates. Data processing involves extracting the minimum fitness value from each run (as lower fitness values represent better/higher-quality solutions in this CVRP formulation) for evaluation using Factorial ANOVA or Kruskal-Wallis tests.
3. Phase 3 (Efficiency Analysis): Compares Untuned, Tuned Fair (equal computational effort), and Tuned Best configurations. The performance gap is calculated as the Relative Percentage Deviation (RPD) from the Best Known Solution (BKS), validated via the Wilcoxon signed-rank test.

## Requirements
* **Simulation:** Visual Studio 2022 or standard C++ Compiler.
* **Analysis:** Python 3.8+.

## Citations
If you use this code or dataset in your research, please cite our paper.

## License & Contribution Guidelines
This project is provided for academic review and research reproducibility under the MIT License.
