
@echo =============== Start ==============================

:: lint default projects:
@echo ----------------------------------------------------
@echo ---------------- LINT ------------------------------
@echo ----------------------------------------------------

::@call make lint project=variant_efr32bg21 -j -B
::@call make lint project=variant_mkv56f1 -j -B
::@call make lint project=variant_ra4m2 -j -B
::@call make lint project=variant_stm32g071 -j -B
::@call make lint project=variant_stm32g474 -j -B
::@call make lint project=variant_stm32h723 -j -B
::@call make lint project=variant_stm32l471 -j -B

@exit /b

