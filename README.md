#  LLVM-PAPI Function-Level Performance Instrumentation Tool

This project is a compiler-based toolchain for **automatically instrumenting C/C++ programs** to collect **per-function performance metrics** using **PAPI (Performance API)**. Built on **LLVM Clang LibTooling**, it inserts hooks into source code that measure custom hardware performance counters (e.g., `PAPI_TOT_INS`, `PAPI_L1_DCM`) and **exports detailed CSV logs** for performance analysis.

---

##  Features

-  Per-function entry/exit instrumentation using Clang AST rewriting
-  Dynamic runtime integration with **PAPI** event counters
-  Export of function metrics including:
  - Function name
  - Start & end timestamps
  - Values of specified PAPI events
-  Fully automated pipeline using `run_pipeline.sh`
-  CLI-based customizability of source, events, and output path

---


