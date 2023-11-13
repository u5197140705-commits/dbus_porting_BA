# rtos_ref_project
Repo for Developing and Testing the RTOS with Framework.

---

### Command to get complete project
git clone https://github-bshg.boschdevcloud.com/RealTimeOS/rtos_ref_project.git --recurse-submodules

---

### Option selected on setup uitility

|                 | stm32g071         | efr32bg21              | ra4m2         | stm32g474         | stm32l471         | stm32h723         | mkv56f1         |
| :-------------- | :---------------- | :--------------------- | :------------ | :---------------- | :---------------- | :---------------- | :-------------- |
| Project name    | variant_stm32g071 | variant_efr32bg21      | variant_ra4m2 | variant_stm32g474 | variant_stm32l471 | variant_stm32h723 | variant_mkv56f1 |
| Default Variant | myVariant         | myVariant              | myVariant     | MyVariant         | myVariant         | myVariant         | myVariant       |
| Platform        | STM32G0           | efr32xg21              | ra4           | STM32G4           | STM32L4           | stm32h7           | mkv5x           |
| Derivative      | STM32G071CB       | EFR32BG21A010F1024IM32 | R7FA4M2AD3CFP | STM32G474VE       | STM32L471VE       | STM32H723ZG       | MKV56F1M0VLL24  |
| Compiler        | armclang          | gccArm                 | armclang      | armclang          | armclang          | armclang          | armclang        |
| FirmwareUpdate  | FWU1              | FWU1                   | FWU1          | FWU1              | FWU1              | FWU1              | FWU1            |
| Os/Schedulers   | rtos              | rtos                   | rtos          | rtos              | rtos              | rtos              | rtos            |
| Cortex          | **M0+**           | **M33**                | **M33**       | **M4**            | **M4**            | **M7**            | **M7**          |

---

### To compile the project(s) in cmd
make project=variant_stm32g071 all -j  
make project=variant_efr32bg21 all -j  
make project=variant_ra4m2 all -j  
make project=variant_stm32g474 all -j  
make project=variant_stm32l471 all -j  
make project=variant_stm32h723 all -j  
make project=variant_mkv56f1 all -j  

---

### Eclipse support: 
A preconfigured Eclipse project is part of this repository.  
To add the project into Eclipse please follow the steps below:
1. Inside Project Explorer ==> Import  
2. Select ==> Existing Project into Workspace
3. Select the root directory from rtos_ref_project 
4. Finish 

---

### Debugging support:

Preconfigured **Keil uVison** and **Segger Ozone** projects are part ot this repository.  
The locations are:
* **./uv** for Keil uVision and 
* **./ozone** for Segger Ozone 

---

### Simple automatisation support:

There are some batch files inside **./helper** folder for compiling, linking, linting or axivion-checkeing for all standard projects at once.

---
