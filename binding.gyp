{
  "targets": [
    {
      "target_name": "BLEConnectionNodeAddon", #Compiled node addon name.
      "cflags!": [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-exceptions"
      ],
      "sources": [ #Source files required to build.
        "cpp_source/main.cpp",
        "cpp_source/BLEWrapper.cpp",
        "cpp_source/BLE/BTHConnection.cpp",
      ],
      "include_dirs": [ #If you use any library in cpp project, write here header files path.
        "<!@(node -p \"require('node-addon-api').include\")",
      ],
      "libraries": [
        '-llegacy_stdio_definitions.lib', '-luser32.lib', '-lWs2_32.lib'
      ],
      "configurations": {#Compilation settings, default RuntimeTypeInfo is closed.
        "Release": {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": "2",
              "RuntimeTypeInfo": "true",
              "AdditionalOptions": [
                "/std:c++17"
              ]
            }
          }
        }
      },
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines":[
       "WIN",
       '_HAS_EXCEPTIONS=1' #To handle exceptions occured in cpp project. 
       ]
    }
  ]
}