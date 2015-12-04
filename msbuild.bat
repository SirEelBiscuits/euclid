REM @echo off

setlocal
set key="HKLM\Software\Microsoft\MSBuild\ToolsVersions\14.0"
set value=MSBuildToolsPath
for /f "tokens=2,* skip=2" %%a in ('reg query %key% /v %value%') do (
	set type=%%a
	set data=%%b
	set value2=%%c
)
echo "%data%"

"%data%msbuild" /m:4 /nologo /clp:ErrorsOnly /t:%1 /p:Configuration=%2 /p:Platform=x86 euclid.sln

exit %ERRORLEVEL%
