:: get project name from current folder name
@for %%I in (.) do @set ProjectName=%%~nxI
:: change to framework root from project folder and save current path
@pushd "%~dp0\..\.."
:: call makefile
@call common\tools\gmake\make.exe -f common\build\makefile project=%ProjectName% %*
:: return to original path
@popd