Param([string]$workingDir)

$update = $true
$versionFile = "$workingDir\ucid_version.txt"

New-Item $workingDir -ItemType Directory -ErrorAction Ignore

If(Test-Path -Path $versionFile -PathType Leaf) {
    $version = Get-Content $versionFile
    If($version -eq $Env:UCID_BUILD_NUMBER) {
        $update = $false
    }
}

If( $update ) {
    $zipFile = "$workingDir\ucid.zip"
    $url = "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/cmid/Windows/cisco-win-cmid-signed/$Env:UCID_BUILD_NUMBER/cisco-win-cmid-signed-$Env:UCID_BUILD_NUMBER.zip"

    $response = Invoke-WebRequest -UseBasicParsing -OutFile $zipFile -PassThru $url
    If($response.StatusCode -ne 200) {
        Write-Host "Error: The CSE-VPN must be on in order to connect to nexus"
        Write-Host "Error: ${response.StatusCode}. Failed to download $url"
        Exit -1
    }
    
    $expectedSha = (Invoke-WebRequest -UseBasicParsing "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/cmid/Windows/cisco-win-cmid-signed/$Env:UCID_BUILD_NUMBER/cisco-win-cmid-signed-$Env:UCID_BUILD_NUMBER.zip.sha1").ToString()
    $fileSha = Get-FileHash -Path $zipFile -Algorithm SHA1
    if($fileSha.Hash -ne $expectedSha ) {
        Write-Host "Sha mismatch $fileSha.Hash $expectedSha"
        Exit -1
    }
    
    if (Test-Path -Path "$workingDir\x64") {
        Remove-Item "$workingDir\x64" -Recurse
    }
    
    if (Test-Path -Path "$workingDir\x86") {
        Remove-Item "$workingDir\x86" -Recurse
    }
    
    Expand-Archive -Path $zipFile -Destination $workingDir
    $Env:UCID_BUILD_NUMBER | Out-File -FilePath $versionFile -Force
    Remove-Item -Path $zipFile
}
else {
    Write-Host "UCID update to date"
    Exit 0
}