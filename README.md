# rtos_dev_tx
Repo for Developing and Testing the RTOS (ThreadX only) with Framework

### Command to get complete project
git clone https://production.github.bshg.com/RealTimeOS/rtos_dev_tx.git --recurse-submodules

### For Eclipse  
1.In Project Explorer ==>Importe the project

2.Select ==> Existing projects in workspace

### Option selected on setup uitility
Project name    = variant_cm4_stm32l4

Default Variant = myVariant

Platform        = STM32L4

Derivative      = STM32L471VE

Compiler        = keilArm

FirmwareUpdate  = FWU1

Os/Schedulers   = rtos

### To compile the project in cmd
make project=variant_cm4_stm32l4 all -j
