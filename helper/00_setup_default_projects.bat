
@echo =============== Start ==============================

:: setup default projects
@echo ----------------------------------------------------
@echo ---------------- SETUP -----------------------------
@echo ----------------------------------------------------

@call ..\common\setup.bat -project variant_efr32bg21 -variant MyVariant -platform efr32xg21 -derivat EFR32BG21A010F1024IM32 -compiler gccArm -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_mkv56f1 -variant MyVariant -platform mkv5x -derivat MKV56F1M0VLL24 -compiler armclang -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_ra4m2 -variant MyVariant -platform ra4 -derivat R7FA4M2AD3CFP -compiler armclang -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_stm32g071 -variant MyVariant -platform STM32G0 -derivat STM32G071CB -compiler armclang -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_stm32g474 -variant MyVariant -platform STM32G4 -derivat STM32G474VE -compiler armclang -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_stm32h723 -variant MyVariant -platform stm32h7 -derivat STM32H723ZG -compiler armclang -scheduler rtos -fwu FWU1								
@call ..\common\setup.bat -project variant_stm32l471 -variant MyVariant -platform STM32L4 -derivat STM32L471VE -compiler armclang -scheduler rtos -fwu FWU1								

@exit /b


:: help content:
:: To crate a log file use this command
:: 00_set_up_default_projects.bat >setup_log.txt 2>&1
