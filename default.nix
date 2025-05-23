{ pkgs
, coil
, lib ? pkgs.lib
, fixedsFile ? ./fixeds.json
, fixeds ? lib.importJSON fixedsFile
}:

rec {
  nixos-pkgs = coil.core.nixos-pkgs.extend (self: super: with self; {
    coil-dump = self.coil.compile-cpp (callPackage ./coil-dump.nix {
      coil-core = self.coil.core;
    });
  });
  coil-dump-nixos = nixos-pkgs.coil-dump;
  coil-dump-nixos-test = pkgs.runCommand "coil-dump-nixos-test" {} ''
    set -eu
    mkdir $out
    cd $out
    ln -s ${assets} assets
    for i in ${coil-dump-nixos}/bin/test_*
    do
      echo $i
      $i
    done
  '';

  coil-dump-ubuntu = pkgs.vmTools.runInLinuxImage ((pkgs.callPackage ./coil-dump.nix {
    inherit (coil.core.ubuntu-pkgs) coil-core;
  }).overrideAttrs (attrs: {
    cmakeFlags = (attrs.cmakeFlags or []) ++ [
      # force clang
      "-DCMAKE_CXX_COMPILER=clang++-${coil.core.ubuntu-pkgs.clangVersion}"
      "-DCMAKE_C_COMPILER=clang-${coil.core.ubuntu-pkgs.clangVersion}"
    ];
    CMAKE_PREFIX_PATH = "${coil.core.ubuntu-pkgs.coil-core}";
    inherit (coil.core.ubuntu-pkgs) diskImage;
    diskImageFormat = "qcow2";
    memSize = 2048;
  }));

  windows-pkgs = coil.core.windows-pkgs.extend (self: super: with self; {
    coil-dump = mkCmakePkg {
      name = "coil-dump";
      inherit (coil-dump-nixos) src;
      buildEnv = buildEnvWithModulesSupport;
      buildInputs = [
        nlohmann_json
        vulkan-headers
        vulkan-loader
        spirv-headers
        zstd
        sdl3
        zlib
        libpng
        sqlite
        freetype
        harfbuzz
        ogg
        opus
        coil-core
      ];
      postPatch = ''
        sed -iE 's/cxx_std_26/cxx_std_23/' CMakeLists.txt
        sed -iE 's/CMAKE_CXX_STANDARD 26/CMAKE_CXX_STANDARD 23/' CMakeLists.txt
      '';
    };
  });
  coil-dump-windows = windows-pkgs.coil-dump;
  coil-dump-windows-test = pkgs.runCommand "coil-dump-windows-test" {
    nativeBuildInputs = [
      coil.toolchain-windows.wine
    ];
  } ''
    set -eu
    ${coil.toolchain-windows.initWinePrefix}
    mkdir $out
    cd $out
    ln -s ${assets} assets
    for i in ${coil-dump-windows}/bin/test_*
    do
      echo $i
      wine $i
    done
  '';

  assets = let
    font = file: {
      loader = "fthb";
      buffer = {
        loader = "file";
        path = file;
      };
    };
  in pkgs.linkFarm "assets" [
    {
      name = "example_render_fonts.json";
      path = pkgs.writeText "example_render_fonts.json" (builtins.toJSON {
        fontArabic = font "${coil.stuff.fonts.vazirmatn}/share/fonts/Vazirmatn[wght].ttf";
        fontDevanagari = font "${coil.stuff.fonts.poppins}/share/fonts/Poppins-VariableFont_wght.otf";
        fontHan = font "${coil.stuff.fonts.source_han_sans}/share/fonts/SourceHanSans-VF.otf";
        fontHebrew = font "${coil.stuff.fonts.noto_sans_hebrew}/share/fonts/NotoSansHebrew[wght].ttf";
        fontLatinGreekCyrillic = font "${coil.stuff.fonts.roboto_flex}/share/fonts/RobotoFlex[GRAD,XOPQ,XTRA,YOPQ,YTAS,YTDE,YTFI,YTLC,YTUC,opsz,slnt,wdth,wght].ttf";
        fontThai = font "${coil.stuff.fonts.noto_sans_thai}/share/fonts/NotoSansThai[wght].ttf";
      });
    }
    {
      name = "test_unicode.json";
      path = pkgs.writeText "test_unicode.json" (builtins.toJSON (let
        quickbrown = pkgs.fetchurl {
          inherit (fixeds.fetchurl."https://www.cl.cam.ac.uk/~mgk25/ucs/examples/quickbrown.txt") url name sha256;
        };
      in {
        utf8 = {
          loader = "file";
          path = quickbrown;
        };
        utf16 = {
          loader = "file";
          path = pkgs.runCommand "utf16.txt" {
            nativeBuildInputs = [pkgs.glibc];
          } ''
            iconv -f utf8 -t utf16le < ${quickbrown} > $out
          '';
        };
        utf32 = {
          loader = "file";
          path = pkgs.runCommand "utf32.txt" {
            nativeBuildInputs = [pkgs.glibc];
          } ''
            iconv -f utf8 -t utf32le < ${quickbrown} > $out
          '';
        };
      }));
    }
    {
      name = "test_webm.json";
      path = pkgs.writeText "test_webm.json" (builtins.toJSON {
        webm = {
          loader = "file";
          path = pkgs.runCommand "av1.webm" {} ''
            ${pkgs.ffmpeg_6-full}/bin/ffmpeg -i ${pkgs.fetchurl {
              inherit (fixeds.fetchurl."https://upload.wikimedia.org/wikipedia/commons/transcoded/2/20/Juno%27s_Perijove-05_Jupiter_Flyby%2C_Reconstructed_in_125-Fold_Time-Lapse%2C_Revised.webm/Juno%27s_Perijove-05_Jupiter_Flyby%2C_Reconstructed_in_125-Fold_Time-Lapse%2C_Revised.webm.1080p.vp9.webm") name url sha256;
              meta.license = lib.licenses.cc-by-30;
            }} -c:v libsvtav1 $out
          '';
        };
        video = {
          loader = "av1";
          source = {
            loader = "webm_track";
            source = "webm";
            type = "Video";
          };
        };
      });
    }
  ];

  touch = {
    inherit
      coil-dump-nixos coil-dump-nixos-test
      coil-dump-ubuntu
      coil-dump-windows coil-dump-windows-test
      assets
    ;
    autoUpdateScript = coil.toolchain.autoUpdateFixedsScript fixedsFile;
  };
}
