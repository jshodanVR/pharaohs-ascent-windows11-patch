# Windows PowerShell 5.1, included with Windows 11. No administrator required.
[CmdletBinding()]
param([string]$SourcePath, [string]$DestinationPath, [switch]$NonInteractive)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$stage = $null
$tempRoot = $null
$reader = $null
$success = $false

function Hash-File([string]$Path) {
    $stream = [IO.File]::OpenRead($Path)
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return [BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-', '').ToLowerInvariant() }
    finally { $stream.Dispose(); $sha.Dispose() }
}
function Safe-Child([string]$Root, [string]$Relative) {
    if ([IO.Path]::IsPathRooted($Relative) -or $Relative -match '(^|[\\/])\.\.([\\/]|$)') { throw 'Unsafe relative file path.' }
    $base = [IO.Path]::GetFullPath($Root).TrimEnd('\') + '\'
    $result = [IO.Path]::GetFullPath((Join-Path $base $Relative))
    if (-not $result.StartsWith($base, [StringComparison]::OrdinalIgnoreCase)) { throw 'File path outside destination.' }
    return $result
}
function Remove-OurTemporaryDirectory([string]$Path, [string]$Parent, [string]$Prefix) {
    if (-not $Path -or -not (Test-Path -LiteralPath $Path)) { return }
    $full = [IO.Path]::GetFullPath($Path)
    if ([IO.Path]::GetDirectoryName($full) -ne [IO.Path]::GetFullPath($Parent).TrimEnd('\') -or -not [IO.Path]::GetFileName($full).StartsWith($Prefix)) {
        throw 'Refusing cleanup outside the installer temporary directory.'
    }
    Remove-Item -LiteralPath $full -Recurse -Force
}

try {
    if (-not $SourcePath) {
        if ($NonInteractive) { throw 'Specify -SourcePath and -DestinationPath.' }
        Add-Type -AssemblyName System.Windows.Forms
        $dialog = New-Object Windows.Forms.OpenFileDialog
        $dialog.Title = "Select Pharaoh's Ascent original download (ZIP or ISO)"
        $dialog.Filter = 'Original game download (*.zip;*.iso)|*.zip;*.iso'
        try { if ($dialog.ShowDialog() -ne 'OK') { throw 'Installation cancelled.' }; $SourcePath = $dialog.FileName } finally { $dialog.Dispose() }
    }
    $SourcePath = (Resolve-Path -LiteralPath $SourcePath).ProviderPath
    if (-not $DestinationPath) {
        if ($NonInteractive) { throw 'Specify -DestinationPath (a new directory).' }
        Add-Type -AssemblyName System.Windows.Forms
        $dialog = New-Object Windows.Forms.FolderBrowserDialog
        $dialog.Description = 'Select a parent folder. A NEW Pharaohs-Ascent-Windows11 folder will be created inside it.'
        $dialog.SelectedPath = [Environment]::GetFolderPath('MyDocuments')
        try { if ($dialog.ShowDialog() -ne 'OK') { throw 'Installation cancelled.' }; $DestinationPath = Join-Path $dialog.SelectedPath 'Pharaohs-Ascent-Windows11' } finally { $dialog.Dispose() }
    }
    $DestinationPath = [IO.Path]::GetFullPath($DestinationPath).TrimEnd('\')
    if (Test-Path -LiteralPath $DestinationPath) { throw 'Destination already exists. Choose a NEW folder; existing games and saves are never overwritten.' }
    $destinationParent = [IO.Path]::GetDirectoryName($DestinationPath)
    if (-not $destinationParent -or -not (Test-Path -LiteralPath $destinationParent -PathType Container)) { throw 'Destination parent folder does not exist.' }
    $manifest = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'patch.json') -Raw | ConvertFrom-Json
    $adapter = Join-Path $PSScriptRoot 'PharaohHD.dll'
    if ((Hash-File $adapter) -ne $manifest.adapter_sha256) { throw 'Patch DLL is damaged or has been modified. Extract the patch again.' }
    $tempParent = [IO.Path]::GetTempPath().TrimEnd('\')
    $tempRoot = Join-Path $tempParent ('PharaohPatch-' + [Guid]::NewGuid().ToString('N'))
    [IO.Directory]::CreateDirectory($tempRoot) | Out-Null
    $folderSource = Test-Path -LiteralPath $SourcePath -PathType Container
    $isoPath = $SourcePath
    if (-not $folderSource -and [IO.Path]::GetExtension($SourcePath) -ieq '.zip') {
        Write-Host 'Reading the original ZIP (no ISO mounting required)...'
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $zip = [IO.Compression.ZipFile]::OpenRead($SourcePath)
        try {
            $isos = @($zip.Entries | Where-Object { $_.FullName.EndsWith('.iso', [StringComparison]::OrdinalIgnoreCase) })
            if ($isos.Count -ne 1 -or $isos[0].Length -gt 1073741824 -or $isos[0].Length -lt 32768) { throw 'Expected exactly one CD ISO (under 1 GB) inside the original ZIP.' }
            $isoPath = Join-Path $tempRoot 'source.iso'
            [IO.Compression.ZipFileExtensions]::ExtractToFile($isos[0], $isoPath, $false)
        } finally { $zip.Dispose() }
    } elseif (-not $folderSource -and [IO.Path]::GetExtension($SourcePath) -ine '.iso') { throw 'Source must be the original ZIP, ISO, or extracted CD directory.' }
    if (-not $folderSource) {
        Add-Type -Path (Join-Path $PSScriptRoot 'src\IsoReader.cs')
        $reader = New-Object PharaohPatch.CdReader($isoPath)
    }
    $stage = Join-Path $destinationParent ('.PharaohPatch-staging-' + [Guid]::NewGuid().ToString('N'))
    [IO.Directory]::CreateDirectory($stage) | Out-Null
    Write-Host 'Checking and copying original game files...'
    foreach ($asset in $manifest.assets) {
        $target = Safe-Child $stage $asset.path
        [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
        if ($folderSource) {
            $original = Safe-Child $SourcePath $asset.path
            if (-not (Test-Path -LiteralPath $original -PathType Leaf)) { throw "Required file missing: $($asset.path)" }
            Copy-Item -LiteralPath $original -Destination $target
        } else { $reader.Extract($asset.path, $target, [long]$asset.size) }
        if ((Get-Item -LiteralPath $target).Length -ne [long]$asset.size -or (Hash-File $target) -ne $asset.sha256) {
            throw "Unsupported or damaged game file: $($asset.path). This patch requires the original English ISO version."
        }
    }
    $originalExe = Join-Path $stage 'DATA\PHARAOH.EXE'
    if ((Hash-File $originalExe) -ne $manifest.source_exe_sha256) { throw 'Unsupported executable version.' }
    Write-Host 'Applying the verified executable delta...'
    [byte[]]$originalBytes = [IO.File]::ReadAllBytes($originalExe)
    [byte[]]$patched = New-Object byte[] ([int]$manifest.output_exe_size)
    [Array]::Copy($originalBytes, $patched, $originalBytes.Length)
    foreach ($record in $manifest.records) {
        [byte[]]$bytes = [Convert]::FromBase64String($record.data)
        [int]$offset = $record.offset
        if ($offset -lt 0 -or $offset -gt $patched.Length - $bytes.Length) { throw 'Invalid patch record.' }
        [Array]::Copy($bytes, 0, $patched, $offset, $bytes.Length)
    }
    $patchedExe = Join-Path $stage 'PHARAOH-W11.EXE'
    [IO.File]::WriteAllBytes($patchedExe, $patched)
    if ((Hash-File $patchedExe) -ne $manifest.output_exe_sha256) { throw 'Patched executable failed SHA256 verification.' }
    Write-Host 'Expanding original graphics...'
    $expandedCount = 0
    foreach ($file in (Get-ChildItem -LiteralPath (Join-Path $stage 'DATA') -Filter '*.BMA' -File)) {
        $stream = [IO.File]::OpenRead($file.FullName)
        try { $header = New-Object byte[] 14; $read = $stream.Read($header, 0, 14) } finally { $stream.Dispose() }
        if ($read -ge 14 -and [Text.Encoding]::ASCII.GetString($header, 0, 4) -eq 'SZDD') {
            $expanded = $file.FullName + '.expanded'
            & "$env:SystemRoot\System32\expand.exe" $file.FullName $expanded | Out-Null
            if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $expanded)) { throw "Unable to expand $($file.Name)." }
            if ((Get-Item -LiteralPath $expanded).Length -ne [BitConverter]::ToUInt32($header, 10)) { throw "Invalid expanded size: $($file.Name)" }
            Move-Item -LiteralPath $expanded -Destination $file.FullName -Force
            $expandedCount++
        }
    }
    Copy-Item -LiteralPath $adapter -Destination (Join-Path $stage 'PharaohHD.dll')
    if (Test-Path -LiteralPath (Join-Path $stage 'FONTS\COMIC.TTF')) { Copy-Item -LiteralPath (Join-Path $stage 'FONTS\COMIC.TTF') -Destination (Join-Path $stage 'COMIC.TTF') }
    [IO.Directory]::CreateDirectory((Join-Path $stage 'saved')) | Out-Null
    @('[Display]', 'Width=1920', 'Height=1080', 'Fullscreen=0') | Set-Content -LiteralPath (Join-Path $stage 'PharaohHD.ini') -Encoding ASCII
    @('@echo off', 'cd /d "%~dp0"', 'start "" "PHARAOH-W11.EXE" /NV') | Set-Content -LiteralPath (Join-Path $stage 'PLAY.cmd') -Encoding ASCII
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'README-EN.txt') -Destination (Join-Path $stage 'README-PATCH.txt')
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'LEGGIMI-IT.txt') -Destination (Join-Path $stage 'LEGGIMI-PATCH.txt')
    $report = [ordered]@{ patch_version=$manifest.version; original_exe_sha256=$manifest.source_exe_sha256; patched_exe_sha256=(Hash-File $patchedExe); adapter_sha256=(Hash-File (Join-Path $stage 'PharaohHD.dll')); verified_original_assets=$manifest.assets.Count; expanded_graphics=$expandedCount }
    $report | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $stage 'PATCH-INSTALL-REPORT.json') -Encoding UTF8
    # Commit only after every check passes. A concurrently created destination also fails safely.
    [IO.Directory]::Move($stage, $DestinationPath)
    $stage = $null
    $success = $true
    Write-Host "Installed successfully: $DestinationPath" -ForegroundColor Green
    Write-Host 'Start PLAY.cmd. F11 toggles fullscreen. Original files were not changed.'
    if (-not $NonInteractive) {
        Add-Type -AssemblyName System.Windows.Forms
        [Windows.Forms.MessageBox]::Show("Installation complete.`r`n`r`n$DestinationPath`r`n`r`nStart PLAY.cmd to play.", "Pharaoh's Ascent Windows 11 Patch") | Out-Null
    }
} catch {
    Write-Host ("Installation failed: " + $_.Exception.Message) -ForegroundColor Red
    if (-not $NonInteractive) { Add-Type -AssemblyName System.Windows.Forms; [Windows.Forms.MessageBox]::Show($_.Exception.Message, 'Patch installation failed') | Out-Null }
} finally {
    if ($null -ne $reader) { $reader.Dispose() }
    if ($stage) { Remove-OurTemporaryDirectory $stage $destinationParent '.PharaohPatch-staging-' }
    if ($tempRoot) { Remove-OurTemporaryDirectory $tempRoot $tempParent 'PharaohPatch-' }
}
if (-not $success) { exit 1 }
exit 0
