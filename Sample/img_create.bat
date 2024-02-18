@echo off
echo.
echo Creating package "PPSX40000.pkg"
echo.
"C:?????\prospero-pub-cmd.exe" img_create "PPSX40000.gp5" "PPSX40000.pkg"
echo.
echo Info for package "PPSX40000.pkg"
echo.
"C:?????\prospero-pub-cmd.exe" img_info "PPSX40000.pkg"
echo.
pause

