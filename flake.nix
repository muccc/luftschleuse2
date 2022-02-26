{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }: rec {
    overlay = final: prev: {
      luftschleuse2-lockd = import ./software/lockd/default.nix { pkgs = final; };
    };

  } // (flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ self.overlay ];
      };
    in
    rec {
      defaultPackage = self.packages.${system}.luftschleuse2-lockd;

      packages.luftschleuse2-lockd = pkgs.luftschleuse2-lockd;
    }
  ));
}
