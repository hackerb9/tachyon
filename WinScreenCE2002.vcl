<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: WinScreenCE2002 - Win32 (WCE ARM) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP92.tmp" with contents
[
/nologo /W3 /GX- /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC=310" /D "UNICODE" /D "_UNICODE" /Fp"ARMDbg/WinScreenCE2002.pch" /YX /Fo"ARMDbg/" /Fd"ARMDbg/" /MC /c 
"C:\Projects\CSBwin\src\WinScreenCE2002.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP92.tmp" 
Creating temporary file "C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP93.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMDbg/WinScreenCE2002.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /nodefaultlib:oldnames.lib" /out:"ARMDbg/WinScreenCE2002.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
.\ARMDbg\WinScreenCE2002.obj
]
Creating command line "link.exe @C:\DOCUME~1\ADMINI~1\LOCALS~1\Temp\RSP93.tmp"
<h3>Output Window</h3>
Compiling...
WinScreenCE2002.cpp
C:\Projects\CSBwin\src\Transition.h(6) : fatal error C1083: Cannot open include file: 'CarbonCore/MacTypes.h': No such file or directory
Error executing clarm.exe.



<h3>Results</h3>
WinScreenCE2002.exe - 1 error(s), 0 warning(s)
</pre>
</body>
</html>
