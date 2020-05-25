!define VERSION 0.1.0
!define NAME crosspath-gui
!define ENTRY Software\${NAME}
!define REPO "https://github.com/busoc/crosspath-gui"
!define DEV BUSOCGC
!define COMPANY BUSOC

Name "${NAME} installer v${VERSION}"
OutFile "${NAME}-setup.exe"
InstallDir $PROGRAMFILES\${NAME}

BrandingText "${NAME} version ${VERSION}"

SetCompressor /SOLID lzma

ShowInstDetails show
ShowUnInstDetails show

SetDateSave off
SetOverwrite on

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Installer"

SetOutPath $INSTDIR
WriteUninstaller $INSTDIR\uninstall.exe

FILE /r ..\..\Deploy\crosspath\

createDirectory $SMPROGRAMS\${COMPANY} 
createShortCut $SMPROGRAMS\${COMPANY}\crosspath-gui.lnk $INSTDIR\crosspath\crosspath-gui.exe
DetailPrint "adding shortcut $SMPROGRAMS\${COMPANY}\crosspath-gui.lnk"

WriteRegStr HKCU ${ENTRY} "DisplayName" ${NAME}
DetailPrint "adding to registryDisplayName(${NAME}) in ${ENTRY}"
WriteRegStr HKCU ${ENTRY} "DisplayVersion" ${VERSION}
DetailPrint "adding to registryDisplayVersion(${VERSION}) in ${ENTRY}"
WriteRegStr HKCU ${ENTRY} "Maintainer" ${DEV}
DetailPrint "adding to registryMaintainer(${DEV}) in ${ENTRY}"
WriteRegStr HKCU ${ENTRY} "Repository" ${REPO}
DetailPrint "adding to registryRepository(${REPO}) in ${ENTRY}"
WriteRegStr HKCU ${ENTRY} "UninstallString" $INSTDIR\uninstall.exe
DetailPrint "adding to registryUnsinstallString() in ${ENTRY}"

SectionEnd

Section "Uninstall"

DELETE $INSTDIR\*exe
RMDIR /r $INSTDIR\crosspath\

DELETE $SMPROGRAMS\${COMPANY}\crosspath-gui.lnk
RMDIR /r $SMPROGRAMS\${COMPANY}\

DetailPrint "delete shortcut $SMPROGRAMS\${COMPANY}\crosspath-gui.lnk"

DeleteRegKey HKCU ${ENTRY}
DetailPrint "deleting from registry ${ENTRY}"

SectionEnd
