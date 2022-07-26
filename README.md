# rtos_dev_tx
Repo for Developing and Testing the RTOS (ThreadX only) with Framework

### Command to get complete project
git clone https://production.github.bshg.com/RealTimeOS/rtos_dev_tx.git --recurse-submodules

### For Eclipse  
1.In Project Explorer ==>Importe the project

2.Select ==> Existing projects in workspace

### Option selected on setup uitility

variant_cm0p_stm32g0 |  -
--------------- | --------------------
Project name    | variant_cm0p_stm32g0
Default Variant | myVariant
Platform        | STM32G0
Derivative      | STM32G071CB
Compiler        | keilArm
FirmwareUpdate  | FWU1
Os/Schedulers   | rtos

variant_cm4_stm32l4 |  -
--------------- | --------------------
Project name    | variant_cm4_stm32l4
Default Variant | myVariant
Platform        | STM32L4
Derivative      | STM32L471VE
Compiler        | keilArm
FirmwareUpdate  | FWU1
Os/Schedulers   | rtos

variant_cm7_mkv56f1 |  -
--------------- | --------------------
Project name    | variant_cm7_mkv56f1
Default Variant | myVariant
Platform        | mkv5x
Derivative      | MKV56F1M0VLL24
Compiler        | armclang
FirmwareUpdate  | FWU1
Os/Schedulers   | rtos

variant_cm33_ra4 |  -
--------------- | --------------------
Project name    | variant_cm33_ra4
Default Variant | myVariant
Platform        | ra4
Derivative      | R7FA4M2AD3CFP
Compiler        | armclang
FirmwareUpdate  | FWU1
Os/Schedulers   | rtos


### To compile the project(s) in cmd
make project=variant_cm0p_stm32g0 all -j  
make project=variant_cm4_stm32l4 all -j  
make project=variant_cm7_mkv56f1 all -j  
make project=variant_cm33_ra4 all -j  
