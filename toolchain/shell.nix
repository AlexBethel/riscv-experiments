{ pkgs ? import <nixpkgs> {} }:
with pkgs.pkgsCross.riscv32;

mkShell {
  nativeBuildInputs = [
    # ...
  ];
}
