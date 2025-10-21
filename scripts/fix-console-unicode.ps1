# PowerShell script to help fix Unicode display issues
# This script sets up your console for proper UTF-8 display

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Bodge Unicode Console Fix (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Set UTF-8 encoding
Write-Host "[1/4] Setting console to UTF-8..." -ForegroundColor Yellow
try {
    [Console]::OutputEncoding = [System.Text.Encoding]::UTF8
    $OutputEncoding = [System.Text.Encoding]::UTF8
    Write-Host "    ✓ UTF-8 encoding enabled successfully" -ForegroundColor Green
} catch {
    Write-Host "    ✗ Failed to set UTF-8 encoding" -ForegroundColor Red
}
Write-Host ""

# Test Unicode characters
Write-Host "[2/4] Testing Unicode character display:" -ForegroundColor Yellow
Write-Host "    Box drawing: ╔═══╗"
Write-Host "                 ║ X ║"
Write-Host "                 ╚═══╝"
Write-Host "    Symbols: ✓ ✗ ℹ ⚠"
Write-Host "    Emojis: 🔨 📦 ⚙️ 🎯"
Write-Host ""

# Show current settings
Write-Host "[3/4] Current console settings:" -ForegroundColor Yellow
Write-Host "    Output Encoding: $([Console]::OutputEncoding.EncodingName)"
Write-Host "    Code Page: $([Console]::OutputEncoding.CodePage)"
Write-Host "    PowerShell Version: $($PSVersionTable.PSVersion)"
Write-Host ""

# Check for profile
Write-Host "[4/4] Checking PowerShell profile:" -ForegroundColor Yellow
if (Test-Path $PROFILE) {
    Write-Host "    ✓ Profile exists: $PROFILE" -ForegroundColor Green
    
    $profileContent = Get-Content $PROFILE -Raw
    if ($profileContent -match "OutputEncoding.*UTF8") {
        Write-Host "    ✓ UTF-8 encoding already configured in profile" -ForegroundColor Green
    } else {
        Write-Host "    ℹ UTF-8 not configured in profile" -ForegroundColor Yellow
        Write-Host "    To make UTF-8 permanent, add this to your profile:" -ForegroundColor Cyan
        Write-Host "    [Console]::OutputEncoding = [System.Text.Encoding]::UTF8" -ForegroundColor White
    }
} else {
    Write-Host "    ℹ Profile not found: $PROFILE" -ForegroundColor Yellow
    Write-Host "    To create and configure:" -ForegroundColor Cyan
    Write-Host "    New-Item -Path $PROFILE -ItemType File -Force" -ForegroundColor White
    Write-Host "    Add-Content $PROFILE '[Console]::OutputEncoding = [System.Text.Encoding]::UTF8'" -ForegroundColor White
}
Write-Host ""

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "If you see the characters correctly above," -ForegroundColor White
Write-Host "your console is ready to use Bodge!" -ForegroundColor White
Write-Host ""

# Try to run bodge version if it exists
$bodgePath = Join-Path $PSScriptRoot "..\bodge.exe"
if (Test-Path $bodgePath) {
    Write-Host "Testing Bodge..." -ForegroundColor Yellow
    Write-Host "========================================" -ForegroundColor Cyan
    & $bodgePath version
} else {
    Write-Host "Note: bodge.exe not found" -ForegroundColor Yellow
    Write-Host "Please build Bodge first with: .\make.bat" -ForegroundColor White
}

Write-Host ""
Write-Host "Press any key to exit..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

