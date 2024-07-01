{
  description = "sqlite-orm-lite is a C++ ORM library for SQLite3 database.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/24.05";

    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux"
    "i686-linux"
    "aarch64-linux"
    "x86_64-darwin"
    "aarch64-darwin"
  ]
    (system:
      let
        pkgs = import nixpkgs {
          inherit system;

          overlays = [ ];

          # Uncomment this if you need unfree software (e.g. cuda) for
          # your project.
          #
          # config.allowUnfree = true;
        };
      in
      {
        devShells.default = pkgs.mkShell rec {
          # Update the name to something that suites your project.
          name = "sqlite-orm-lite";

          packages = with pkgs; [
            # Development Tools
            llvmPackages_18.clang
            cmake
            cmakeCurses

            # Development time dependencies
            gtest

            # Build time and Run time dependencies
            sqlite.dev
          ];

          # Setting up the environment variables you need during
          # development.
          shellHook =
            let
              icon = "f121";
            in
            ''
              export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
              function format_dir() {
                find "$1" -type f \( -name "*.cc" -o -name "*.h" \) -exec echo "Formatting {}" \; -exec clang-format -i {} +
              }
            '';
        };

        packages.default = pkgs.callPackage ./default.nix { };
      });
}
