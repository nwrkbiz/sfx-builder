Simple SFX (Self Extracting Archive) Builder
============================================

This project allows creating simple self extracting zip archives for windows and linux.
Place a [.sfx.manifest.json](./.sfx.manifest.json) file in the root of your zip archive to configure your installer.

After creating your zip archive you can make it self extracting by executing:

````
sfx.linux_x86_64_musl --zip archive.zip --outfile Installer.run
````

You can also create self extracting archives on for windows and vice versa:

````
// linux to windows
sfx.linux_x86_64_musl --zip archive.zip --sfx sfx.windows_64.exe --outfile Installer.exe

// windows to linux
sfx.windows_64.exe --zip archive.zip --sfx sfx.linux_x86_64_musl --outfile Installer.run
````


All options
-----------

````
Allowed options:
  --help                Print help.
  --zip arg             Path to zip file for packing.
  --sfx arg             Path to SFX file for packing. (defaults to this
                        executable)
  --outfile arg         Output file for packing. (defaults to ./Install.exe or
                        ./Install.run)
  --silent arg          Silent install
````
