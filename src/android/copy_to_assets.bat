@echo off

set basePath=%cd%

@rem cd  "..\..\bin\"
@rem call cache_all.bat 

cd %basePath%
if exist .\assets rmdir .\assets /s /q
@rem xcopy "..\..\bin\*.*" ".\assets\*.*" /e /exclude:exclude.txt
if exist .\assets\sound\Desktop rmdir .\assets\sound\Desktop /s /q
if exist .\assets\cache rmdir .\assets\cache /s /q
@rem pause