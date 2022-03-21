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

    $expectedSha = (Invoke-WebRequest -UseBasicParsing "$url.sha1").ToString()

    if( $expectedSha -eq $null ) {
        Write-Host "Error: Failed to download $url.sha1"
        Write-Host "Error: The CSE-VPN must be on in order to connect to nexus"
        Exit -1
    }
    
    for ($i = 0; $i -lt 5; $i++)
    {
        $response = Invoke-WebRequest -UseBasicParsing -OutFile $zipFile -PassThru $url
        If($response.StatusCode -ne 200) {
            Write-Host "${response.StatusCode}. Failed to download $url"
        } else {
            $fileSha = Get-FileHash -Path $zipFile -Algorithm SHA1
            if($fileSha.Hash -ne $expectedSha ) {
                Write-Host "Sha mismatch $fileSha.Hash $expectedSha"
            }
            else {
                Write-Host "Completed Download of: $url"
                break
            }
        }
        
        if( $i -eq 4 ) {
            Write-Host "Error: Retries Exhausted. Failed to download $url"
            Write-Host "Error: The CSE-VPN must be on in order to connect to nexus"
            Exit -1
        }
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
    Write-Host "UCID updated to version ${Env:UCID_BUILD_NUMBER} successfully"
}
else {
    Write-Host "UCID update to date"
    Exit 0
}