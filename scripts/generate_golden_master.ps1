# Golden Master baseline generator for UnitConverter CLI regression tests.
# Usage:
#   .\scripts\generate_golden_master.ps1
#   .\scripts\generate_golden_master.ps1 -BuildDir build_cov -Approve

param(
    [string]$BuildDir = "build",
    [switch]$Approve
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$ExpectedPath = Join-Path $Root "tests\golden_master_expected.txt"
$CliPath = Join-Path $Root "$BuildDir\unit_converter_cli.exe"
$Scenarios = @("meter:2.5", "feet:1.0", "yard:1.0", "meter:0.0")

function Format-SixDecimal {
    param([double]$Value)
    return "{0:F6}" -f $Value
}

function Parse-Scenario {
    param([string]$Scenario)

    $colon = $Scenario.IndexOf(':')
    if ($colon -lt 0) {
        throw "Invalid scenario: $Scenario"
    }

    return @{
        Unit       = $Scenario.Substring(0, $colon)
        ValueToken = $Scenario.Substring($colon + 1)
    }
}

function Capture-CliRawOutput {
    param([string]$Scenario)

    if (-not (Test-Path $CliPath)) {
        throw "CLI not found: $CliPath (run cmake --build first)"
    }

    $TempDir = Join-Path $env:TEMP "unit_converter_golden"
    New-Item -ItemType Directory -Force -Path $TempDir | Out-Null
    $InputFile = Join-Path $TempDir "input.txt"
    $OutputFile = Join-Path $TempDir "output.txt"

    Set-Content -Path $InputFile -Value $Scenario -NoNewline
    cmd /c "`"$CliPath`" < `"$InputFile`" > `"$OutputFile`""
    return (Get-Content -Path $OutputFile -Raw).Replace("`r", "")
}

function Convert-CliLine {
    param(
        [string]$Line,
        [string]$ValueToken,
        [string]$SourceUnit
    )

    if ($Line -match '=\s*(.+)\s+(\S+)$') {
        $targetValue = [double]$Matches[1]
        $targetUnit = $Matches[2]
        if ($targetUnit -eq $SourceUnit) {
            return $null
        }
        return "$ValueToken $SourceUnit = $(Format-SixDecimal $targetValue) $targetUnit"
    }

    return $null
}

function Build-ScenarioBlockFromCli {
    param([string]$Scenario)

    $parsed = Parse-Scenario -Scenario $Scenario
    $raw = Capture-CliRawOutput -Scenario $Scenario
    $lines = New-Object System.Collections.Generic.List[string]

    foreach ($line in ($raw -split "`n")) {
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }

        if ($line -like "*Insert value for converting*") {
            $promptEnd = $line.IndexOf("): ")
            if ($promptEnd -ge 0) {
                $line = $line.Substring($promptEnd + 3)
            } else {
                continue
            }
        }

        $converted = Convert-CliLine -Line $line.Trim() -ValueToken $parsed.ValueToken -SourceUnit $parsed.Unit
        if ($null -ne $converted) {
            $lines.Add($converted)
        }
    }

    if ($lines.Count -eq 0) {
        throw "CLI produced no conversion lines for scenario: $Scenario"
    }

    $block = New-Object System.Text.StringBuilder
    [void]$block.Append("[$Scenario]`n")
    foreach ($entry in $lines) {
        [void]$block.Append("$entry`n")
    }
    [void]$block.Append("---`n")
    return $block.ToString()
}

function Normalize-Newlines {
    param([string]$Text)
    return $Text.Replace("`r`n", "`n").Replace("`r", "`n")
}

function Write-GoldenBaseline {
    param([string]$Content)
    $normalized = Normalize-Newlines $Content
    [System.IO.File]::WriteAllText($ExpectedPath, $normalized)
}

function Build-GoldenDocumentFromCli {
    $document = New-Object System.Text.StringBuilder
    foreach ($scenario in $Scenarios) {
        [void]$document.Append((Build-ScenarioBlockFromCli -Scenario $scenario))
    }
    return $document.ToString()
}

Write-Host "Project root: $Root"
Write-Host "Expected path: $ExpectedPath"

if (-not (Test-Path $CliPath)) {
    Write-Host "Building CLI in $BuildDir ..."
    cmake -S $Root -B (Join-Path $Root $BuildDir)
    cmake --build (Join-Path $Root $BuildDir) --target unit_converter_cli
}

Write-Host "Capturing CLI stdout for $($Scenarios.Count) scenarios ..."
$Document = Build-GoldenDocumentFromCli

if ((-not (Test-Path $ExpectedPath)) -or $Approve) {
    Write-GoldenBaseline -Content $Document
    Write-Host "Golden master baseline written: $ExpectedPath"
    Write-Host "Run: git add tests/golden_master_expected.txt"
    exit 0
}

$Expected = Normalize-Newlines (Get-Content -Path $ExpectedPath -Raw)
$Document = Normalize-Newlines $Document
if ($Expected -ne $Document) {
    Write-Error "Golden master mismatch. Re-run with -Approve to refresh baseline."
    exit 1
}

Write-Host "Golden master matches expected baseline."
