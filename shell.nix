{ pkgs ? import <nixpkgs> {} }:

(pkgs.buildFHSEnv {
  name = "platformio-fhs";
  targetPkgs = pkgs: (with pkgs; [
    platformio
    git
    python3
    zlib
    glibc
    libusb1
    udev
  ]);
  runScript = "bash";
}).env
