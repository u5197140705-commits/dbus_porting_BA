:: change to project root
@pushd "%~dp0"
@call common\tools\gmake\make.exe -f common\build\makefile %*
@popd
