# notsh

Minimal CLI note-taking tool.

## Usage

```
note              Create a new note (opens $EDITOR)
note open <name>  Open an existing note
note list         List all notes
```

## Install

### As a flake input

Add to your `flake.nix` inputs and overlay:

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    notsh = {
      url = "git+https://codeberg.org/Poellebob/notsh.git";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, notsh, ... }: {
    # makes pkgs.notsh available everywhere
    nixpkgs.overlays = [ notsh.overlays.default ];
  };
}
```

Then use it like any other package:

```nix
{ pkgs, ... }: {
  environment.systemPackages = [ pkgs.notsh ];
}
```

Or add only the overlay in your `configuration.nix`:

```nix
{ config, pkgs, lib, inputs, ... }: 

{
  nixpkgs.overlays = [
    nixpkgs.overlays = [ inputs.notsh.overlays.default ];
  ];
}
```

Making `pkgs.notsh` available in `configuration.nix`.

## Build

```
nix build .#notsh       # nix flake
nix run .               # run directly
```

### Without nix

**Requirements:** `meson`, `ninja`, `clang`, `just`

```
just build
./build/note
```
