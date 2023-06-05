
@echo =============== Start ==============================

:: compile default projects
@echo ----------------------------------------------------
@echo ---------------- COMPILE ---------------------------
@echo ----------------------------------------------------

@call ..\make all project=variant_efr32bg21 -j
@call ..\make all project=variant_mkv56f1 -j
@call ..\make all project=variant_ra4m2 -j
@call ..\make all project=variant_stm32g071 -j
@call ..\make all project=variant_stm32g474 -j
@call ..\make all project=variant_stm32h723 -j
@call ..\make all project=variant_stm32l471 -j

@exit /b


:: help content:
:: To crate a log file use this command
:: 01_compile_default_projects.bat >compile_log.txt 2>&1

