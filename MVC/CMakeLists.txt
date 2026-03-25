cmake_minimum_required(VERSION 3.16)
project(Echec_Yalta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔧 FIX CRITIQUE : Force le linker MinGW 14 à inclure msvcrt pour les DLLs
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lmsvcrt")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lmsvcrt")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lmsvcrt")

# Désactive l'audio (évite les erreurs FLAC)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(Echec_Yalta
    main.cpp
    View/src/MakeBoard.cpp
    View/src/DrawBoard.cpp
    View/src/Menu.cpp
    Controller/src/BoardController.cpp
    Controller/src/MainController.cpp
    Model/src/Jeu.cpp
    Model/src/Plateau.cpp
    Model/src/Joueur.cpp
    Model/src/Piece.cpp
)

target_include_directories(Echec_Yalta PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/View/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Controller/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/include
)

target_link_libraries(Echec_Yalta PRIVATE sfml-graphics sfml-window sfml-system sfml-main)

add_custom_command(TARGET Echec_Yalta POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:Echec_Yalta>/resources
)