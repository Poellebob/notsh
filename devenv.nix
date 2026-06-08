{ pkgs, lib, config, inputs, ... }:

{
  packages = with pkgs; [
    git
    meson
    ninja
    just
    clang
    gdb 
    cjson
  ];

  env = {
    BUILD_DIR = "build";
    CC="clang";
  };

  languages.c.enable = true;
}
