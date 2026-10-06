@echo off
set "IDF_TOOLS_PATH=C:\Espressif"
call "C:\Espressif\frameworks\esp-idf-v5.1.1\export.bat"
cd C:\Users\karthikkrazy\Documents\antigravity\sharp-brahmagupta\sensecap_indicator_esp32\examples\indicator_openai
call idf.py fullclean
call idf.py build
call idf.py -p COM21 flash monitor
