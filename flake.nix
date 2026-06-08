{
  description = "notsh - a minimal command-line note-taking utility";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs }: let
    supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f system nixpkgs.legacyPackages.${system});
  in {
    packages = forAllSystems (system: pkgs: {
      notsh = pkgs.callPackage ./default.nix { };
      default = self.packages.${system}.notsh;
    });

    apps = forAllSystems (system: pkgs: {
      notsh = { type = "app"; program = "${self.packages.${system}.notsh}/bin/note"; };
      default = self.apps.${system}.notsh;
    });

    overlays.default = final: prev: {
      notsh = final.callPackage ./default.nix { };
    };
  };
}
