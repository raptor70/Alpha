@echo off

set basePath=%cd%

@rem cd  "..\..\bin\"
@rem call cache_all.bat 

cd %basePath%

call %ANDROID_HOME%/tools/jobb.bat -d %basePath%/assets_dump/ -dump %basePath%/main.3.com.kevinchoteau.splode.obb -pn com.kevinchoteau.splode -pv 3

pause

