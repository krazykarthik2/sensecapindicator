$bin_path = "build\indicator_openai.bin"
if (Test-Path $bin_path) {
    Remove-Item $bin_path -Force
    Write-Host "==> Deleted previous firmware binary ($bin_path) <=="
} else {
    Write-Host "==> No previous firmware binary found <=="
}

$env:IDF_TOOLS_PATH="C:\Espressif"
. C:\Espressif\frameworks\esp-idf-v5.1.1\export.ps1

idf.py build flash -p COM21
