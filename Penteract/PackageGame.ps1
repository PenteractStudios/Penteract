${EngineName} = "Tesseract"
${GameName} = "Penteract"
${GameProjectName} = "Penteract"
${Version} = "0.1.0"

${PackageDir} = "Package"
${EngineSourceDir} = "Engine"
${LibraryDir} = "Library"
${FontsDir} = "Fonts"
${ReleaseDir} = "Penteract/Build/ReleaseEditor"
${LicensesDir} = "Licenses"

${OutDir} = "Package"

Remove-Item -Path "${PackageDir}" -ErrorAction "Ignore" -Recurse
New-Item -Path "${PackageDir}" -ItemType "Directory" -Force
New-Item -Path "${PackageDir}/${LicensesDir}" -ItemType "Directory" -Force
New-Item -Path "${PackageDir}/${LicensesDir}/${EngineName}" -ItemType "Directory" -Force
Copy-Item -Path "../${LicensesDir}" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "${FontsDir}", "${LibraryDir}" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "../LICENSE" -Destination "${PackageDir}/${LicensesDir}/${EngineName}" -Force -Recurse
Copy-Item -Path "*.dll" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "${ReleaseDir}/${GameProjectName}.dll" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "${EngineSourceDir}/${EngineName}.exe" -Destination "${PackageDir}" -Force -Recurse
Copy-Item -Path "config.json" -Destination "${PackageDir}" -Force -Recurse