Write-Host "=============================="
Write-Host " BeforeAfter Restructure Tool"
Write-Host "=============================="

$ROOT = Get-Location

$BACKUP = "$ROOT\Backup\BeforeAfter_old"
New-Item -ItemType Directory -Force -Path $BACKUP | Out-Null

Write-Host "[1/6] Creating folder structure..."

$folders = @(
"engine\core",
"engine\world",
"engine\combat",
"engine\registry",
"language\ruby\gemfiles",
"language\dsl",
"language\runtime_bridge",
"tools\ide\ui",
"tools\tests",
"tools\debug",
"build\bin",
"build\temp",
"config\vscode",
"third_party"
)

foreach ($f in $folders) {
    New-Item -ItemType Directory -Force -Path $f | Out-Null
}

Write-Host "[2/6] Moving engine files..."

if (Test-Path "Core.h") { Move-Item "Core.h" "engine\core\" }
if (Test-Path "Core Old.h") { Move-Item "Core Old.h" $BACKUP }
if (Test-Path "Core copy.h") { Move-Item "Core copy.h" $BACKUP }

if (Test-Path "Combat.h") { Move-Item "Combat.h" "engine\combat\" }
if (Test-Path "combat-old.h") { Move-Item "combat-old.h" $BACKUP }
if (Test-Path "Combat copy.h") { Move-Item "Combat copy.h" $BACKUP }

if (Test-Path "world.c") { Move-Item "world.c" "engine\world\" }
if (Test-Path "world.h") { Move-Item "world.h" "engine\world\" }

Write-Host "[3/6] Moving registry system..."

Move-Item -ErrorAction SilentlyContinue "Src\cplusplus_src\RegistryContext.*" "engine\registry\"
Move-Item -ErrorAction SilentlyContinue "Src\cplusplus_src\AdditionMachine.hpp" "engine\registry\"

Move-Item -ErrorAction SilentlyContinue "Src\cplusplus_src\Registries\*" "engine\registry\"

Write-Host "[4/6] Moving language system..."

if (Test-Path "Chython") { Move-Item "Chython" "language\ruby\" }

Write-Host "[5/6] Moving tools..."

if (Test-Path "main.c") { Move-Item "main.c" "tools\ide\" }
if (Test-Path "main.cpp") { Move-Item "main.cpp" "tools\debug\" }

Move-Item -ErrorAction SilentlyContinue "*.exe" "build\bin\"

Write-Host "[6/6] Moving config..."

if (Test-Path ".ruby-version") { Move-Item ".ruby-version" "config\" }
if (Test-Path ".vscode") { Move-Item ".vscode" "config\vscode\" }

Write-Host "=============================="
Write-Host " RESTRUCTURE COMPLETE"
Write-Host " Backup at: $BACKUP"
Write-Host "=============================="