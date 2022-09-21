{ pkgs ? import (builtins.getFlake (toString ../../.)).inputs.nixpkgs {} }:

pkgs.poetry2nix.mkPoetryApplication {
  projectDir = ./.;
  python = pkgs.python310;
  overrides = pkgs.poetry2nix.overrides.withDefaults (self: super: {
    idna = super.idna.overrideAttrs (old: {
      nativeBuildInputs = old.nativeBuildInputs ++ [ self.flit-core ];
    });
    uvicorn = super.uvicorn.overrideAttrs (old: {
      nativeBuildInputs = old.nativeBuildInputs ++ [ self.hatchling ];
    });
    fastapi = super.fastapi.overrideAttrs (old: {
      nativeBuildInputs = old.nativeBuildInputs ++ [ self.hatchling ];
    });
  });
}
