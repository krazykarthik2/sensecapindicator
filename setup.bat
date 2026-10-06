@echo off
echo ===================================================
echo   SenseCAP Indicator (To-Do App) - Setup Script
echo ===================================================

:: 1. Load ESP-IDF Environment
if "%IDF_PATH%"=="" (
    echo ESP-IDF environment not found. Attempting to load from C:\Espressif...
    if exist "C:\Espressif\frameworks\esp-idf-v5.1.1\export.bat" (
        set "IDF_TOOLS_PATH=C:\Espressif"
        call "C:\Espressif\frameworks\esp-idf-v5.1.1\export.bat"
    ) else (
        echo ERROR: Could not find ESP-IDF v5.1.1 at C:\Espressif.
        echo Please ensure ESP-IDF is installed and run its export.bat manually.
        pause
        exit /b 1
    )
) else (
    echo ESP-IDF environment is already loaded.
)

:: 2. Build the project
echo.
echo Navigating to the main project directory...
cd examples\indicator_openai

echo.
echo Building the firmware...
call idf.py build
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Build failed! Please check the logs above.
    pause
    exit /b %errorlevel%
)

:: 3. Flash prompt
echo.
echo Build successful! 
set /p FLASH="Do you want to flash the device now? (Y/N): "
if /I "%FLASH%"=="Y" goto do_flash
goto end

:do_flash
echo.
set /p PORT="Enter your device COM port (e.g., COM25): "
echo Flashing device on %PORT%...
call idf.py -p %PORT% flash monitor

:end
echo.
echo ===================================================
echo Setup complete! You can also flash manually anytime
echo by running 'idf.py flash' inside examples\indicator_openai
echo ===================================================
pause
