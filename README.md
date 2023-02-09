# FillMyPDF

![FillMyPDF help](https://github.com/strinque/FillMyPDF/blob/master/docs/help.png)

**FillMyPDF** is a simple **Windows** program written in c++11 which fills PDF fields with custom data.  
This project is composed of a library which contains all the logic of reading/writing to PDF file.  
A command-line executable: `FillMyPDF.exe` will also be provided and uses this library.  

The *PoDoFo* library will be used to to parse a PDF file and modify its contents into memory.
*PoDoFo* is a free portable c++ library which includes very simple classes to edit PDF files.

Implemented in c++17 and use `vcpkg`/`cmake` for the build-system.  
It uses the `winpp` header-only library from: https://github.com/strinque/winpp.

## Features

- [x] use `nlohmann/json` header-only library for `json` parsing
- [x] use c++ `PoDoFo` library to manipulate **PDF** files
- [x] implement a standalone c++ class: `FillMyPDF` to provide filling functionnalities
- [x] implement a simple c++ command-line program that uses this library and demonstrate usage

## Usage

![FillMyPDF filling-process.png](https://github.com/strinque/FillMyPDF/blob/master/docs/filling-process.png)

The following command-line fills a **PDF** file: `forms.pdf` with **json** data: `data.json` and generates a **PDF** file: `output.pdf`:

``` console
FillMyPDF.exe --pdf-input forms.pdf \
              --pdf-output output.pdf \
              --json-data data.json
```

Format of `json` data file (with utf8 values for test): 

``` json
{
  "text": [
    {
      "name": "Given Name Text Box",
      "value": "Inès Düpond"
    },
    {
      "name": "Family Name Text Box",
      "value": "Jaouîsê"
    },
    {
      "name": "Address 1 Text Box",
      "value": "148 park avênue"
    },
    {
      "name": "Postcode Text Box",
      "value": "56000"
    },
    {
      "name": "City Text Box",
      "value": "Best City"
    }
  ],
  "checkbox": [
    {
      "name": "Driving License Check Box",
      "value": true
    }
  ]
}
```

## Requirements

This project uses **vcpkg**, a free C/C++ package manager for acquiring and managing libraries to build all the required libraries.  
It also needs the installation of the **winpp**, a private *header-only library*, inside **vcpkg**.

### Install vcpkg

The install procedure can be found in: https://vcpkg.io/en/getting-started.html.  
The following procedure installs `vcpkg` and integrates it in **Visual Studio**.

Open PowerShell: 

``` console
cd c:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
```

Create a `x64-windows-static-md` triplet file used to build the program in *shared-mode* for **Windows CRT** libraries but *static-mode* for third-party libraries:

``` console
$VCPKG_DIR = Get-Content "$Env:LocalAppData/vcpkg/vcpkg.path.txt" -Raw 

Set-Content "$VCPKG_DIR/triplets/community/x64-windows-static-md.cmake" 'set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_BUILD_TYPE release)'
```

### Install winpp ports-files

Copy the *vcpkg ports files* from **winpp** *header-only library* repository to the **vcpkg** directory.

``` console
$VCPKG_DIR = Get-Content "$Env:LocalAppData/vcpkg/vcpkg.path.txt" -Raw 

mkdir $VCPKG_DIR/ports/winpp
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/strinque/winpp/master/vcpkg/ports/winpp/portfile.cmake" -OutFile "$VCPKG_DIR/ports/winpp/portfile.cmake"
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/strinque/winpp/master/vcpkg/ports/winpp/vcpkg.json" -OutFile "$VCPKG_DIR/ports/winpp/vcpkg.json"
```

## Build

### Build using cmake

To build the program with `vcpkg` and `cmake`, follow these steps:

``` console
$VCPKG_DIR = Get-Content "$Env:LocalAppData/vcpkg/vcpkg.path.txt" -Raw 

git clone https://github.com/strinque/FillMyPDF
cd FillMyPDF
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE="MinSizeRel" `
      -DVCPKG_TARGET_TRIPLET="x64-windows-static-md" `
      -DCMAKE_TOOLCHAIN_FILE="$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" `
      ../
cmake --build . --config MinSizeRel
```

The program executable should be compiled in: `FillMyPDF\build\src\MinSizeRel\FillMyPDF.exe`.

### Build with Visual Studio

**Microsoft Visual Studio** can automatically install required **vcpkg** libraries and build the program thanks to the pre-configured files: 

- `CMakeSettings.json`: debug and release settings
- `vcpkg.json`: libraries dependencies

The following steps needs to be executed in order to build/debug the program:

``` console
File => Open => Folder...
  Choose FillMyPDF root directory
Solution Explorer => Switch between solutions and available views => CMake Targets View
Select x64-release or x64-debug
Select the src\FillMyPDF.exe (not bin\FillMyPDF.exe)
```

To add command-line arguments for debugging the program:

```
Solution Explorer => Project => (executable) => Debug and Launch Settings => src\program.exe
```

``` json
  "args": [
    "--pdf-input ${projectDir}\\pdf\\forms.pdf",
    "--pdf-output ${projectDir}\\pdf\\output.pdf",
    "--json ${projectDir}\\pdf\\data.json"
  ]
```