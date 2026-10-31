; Windows installer: the standalone app, the VST3 plugin in the common VST3 folder, shortcuts
; and an uninstaller. Built by the CI with: ISCC /DVersion=x.y.z /DArtefacts=<Release folder> BassAmp.iss

#ifndef Version
  #define Version "0.0.0"
#endif
#ifndef Artefacts
  #define Artefacts "..\build\BassAmp_artefacts\Release"
#endif

[Setup]
AppId={{6F1D3C52-8E0B-4C7A-9B44-2F3E7A1C5D90}
AppName=Bass Amp
AppVersion={#Version}
AppPublisher=Weylland
AppPublisherURL=https://github.com/Weylland/bass-amp
AppSupportURL=https://github.com/Weylland/bass-amp/issues
DefaultDirName={autopf}\Bass Amp
DisableProgramGroupPage=yes
LicenseFile=..\LICENSE
UninstallDisplayIcon={app}\Bass Amp.exe
OutputDir=..\dist
OutputBaseFilename=BassAmp-{#Version}-Windows-x64-Setup
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "pt"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Types]
Name: "full"; Description: "{code:FullInstall}"
Name: "custom"; Description: "{code:CustomInstall}"; Flags: iscustom

[Components]
Name: "app"; Description: "Bass Amp (standalone)"; Types: full custom
Name: "vst3"; Description: "VST3 plugin"; Types: full

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Components: app

[Files]
Source: "{#Artefacts}\Standalone\Bass Amp.exe"; DestDir: "{app}"; Components: app; Flags: ignoreversion
Source: "{#Artefacts}\VST3\Bass Amp.vst3\*"; DestDir: "{commoncf64}\VST3\Bass Amp.vst3"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\LICENSE"; DestDir: "{app}"; DestName: "LICENSE.txt"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\Bass Amp"; Filename: "{app}\Bass Amp.exe"; Components: app
Name: "{autodesktop}\Bass Amp"; Filename: "{app}\Bass Amp.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\Bass Amp.exe"; Description: "{cm:LaunchProgram,Bass Amp}"; Components: app; Flags: nowait postinstall skipifsilent

[Code]
function FullInstall (Param: String): String;
begin
  Result := SetupMessage (msgFullInstallation);
end;

function CustomInstall (Param: String): String;
begin
  Result := SetupMessage (msgCustomInstallation);
end;
