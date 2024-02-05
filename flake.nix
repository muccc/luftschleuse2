{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    poetry2nix = {
      url = "github:nix-community/poetry2nix";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    poetry2nix,
    ...
  }:
    {
      overlays.default = final: prev: {
        luftschleuse2-lockd = import ./software/lockd/default.nix {pkgs = final;};
      };
    }
    // (flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [self.overlays.default poetry2nix.overlay];
        };
      in {
        packages = {
          default = pkgs.luftschleuse2-lockd;
          luftschleuse2-lockd = pkgs.luftschleuse2-lockd;
        };
        formatter = pkgs.alejandra;
      }
    ));
}
