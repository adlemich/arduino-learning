# CMakeList.txt: CMake-Projekt für "ArduinoIO_Demo_TestClient". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)

# Aktivieren Sie Hot Reload für MSVC-Compiler, sofern unterstützt.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ArduinoIO_Demo_TestClient")

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (ArduinoIO_Demo_TestClient "ArduinoIO_Demo_TestClient.cpp" "ArduinoIO_Demo_TestClient.h" "ArduinoIO_TCPClient.h" "ArduinoIO_TCPClient.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ArduinoIO_Demo_TestClient PROPERTY CXX_STANDARD 20)
endif()

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
