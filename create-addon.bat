del bin\jakilid-firefox-addon.xpi
rd /s /q bin\ffaddon\
xcopy "./Jakilid@Jakilid.com" bin\ffaddon\ /E /Y
xcopy ".\ui\gui\UI" bin\ffaddon\chrome\ui\ /E /Y
cd bin\ffaddon\
7za a -tzip -mx=1 ..\jakilid-firefox-addon.xpi *
cd ..
rd /s /q ffaddon\