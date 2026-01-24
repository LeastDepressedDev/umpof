@echo off
pip install redis --break-system-packages
for /F "delims=" %%i in ('python -c "import site; print(site.getsitepackages()[0])"') DO SET pth=%%i
echo %pth%
robocopy pympofl %pth%/Lib/pympofl