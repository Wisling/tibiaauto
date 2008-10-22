e:
cd \projects\tibiaauto-pub
cvs update
cd \projects\tibiaauto-kernel
cvs update
d:
cd d:\Program Files\Microsoft Visual Studio\Common\msdev98\Bin\
msdev e:\projects\tibiaauto-kernel\tibiaauto.dsw /MAKE "tibiaauto - Win32 Release" /REBUILD
e:
cd e:\projects\tibiaauto-pub
rm -f TibiaAutoSetup_BETA.exe
"d:\Program Files\NSIS\makensis.exe" tibiaauto-beta.nsi
"d:\Program Files\WinSCP\WinSCP.com" /script=tibiaauto-release-beta.com
pause
