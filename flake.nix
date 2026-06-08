{
  description = "notsh - a minimal command-line note-taking utility";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = {
          notsh = pkgs.callPackage ./default.nix { };
          default = self.packages.${system}.notsh;
        };

        apps = {
          notsh = { type = "app"; program = "${self.packages.${system}.notsh}/bin/note"; };
          default = self.apps.${system}.notsh;
        };
      });

  overlays.default = final: prev: {
    notsh = final.callPackage ./default.nix { };
  };
}
}
