# Hier steht der Name des Projectes
PROJECT(TD_Tutorial)

# Wir setzten eine minimale Version von CMake von 2.8.11 voraus
# Warum?
# Ich bin mir gerade nicht sicher. Wahrscheinlich brauchen wir die neuen
# Features nicht. Egal. Wird schon passen.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)


# Wir brauchen ein paar Pakete.
# SDL2 hat bisher keine FindSDL2.cmake oder ähnliches.
# Dafür aber eine pkg-config Datei. Also können wir die Infos für SDL2
# einfach dem pkg-config überlassen. Dazu brauchen wir aber auch das
# passende Modul von pkg-config für CMake.
# Und da wir pkg-config unbeding brauchen, bekommt es ein required-flag.
FIND_PACKAGE(PkgConfig REQUIRED)


# Nutzen wir also pkg-config um die Pakete zu finden.
# Sollten diese nicht gefunden werden, wird beim Laufen lassen ein Fehler
# angezeigt, falls das entsprechende Pakete mit REQUIRED geflaggt wurde.
# SDL_image ist ein eigenes Paket, eine eigene Bibliothek. Darum müssen wir sie
# auch extra hinzufügen.
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
PKG_CHECK_MODULES(SDL2_Image REQUIRED SDL2_image)


# Sind wir hier, wurde alles gefunden. Also lasst es uns nutzen.
# Ganz wichtig: Includes
# Der Compiler muss die Dateien, die HEADER, die benötigt werden, irgendwo
# finden. Durch find_package und dem pkg-Äquivalent gibt es ein paar
# Variablen, in denen die passenden Pfade stehen. Hier müssen wir sie nur 
# noch weiterreichen.
INCLUDE_DIRECTORIES( ${SDL2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES( ${SDL2_Image_INCLUDE_DIRS})


# Hurra Hurra C++11 ist da!
# Um das zu nutzen, muss ich aber auch ein paar Flags setzten.
# Die hier sind nur für GCC und CLang.
# Da sind auch gleich noch ein paar mehr Optionen mit dabei. Ein paar Warnungen
# extra.
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -g3")


# Es werden ein paar CPP Dateien anfallen. Diese kommen hier in die Liste der
# SOURCE_FILES. Die werden nacher dem Kompiler gegeben und der macht etwas
# damit. Neue CPP Dateien müssen hier von Hand hinzugefügt werden!
SET( SOURCE_FILES
	main.cpp
)


# Welche CPP Dateien gehören zum Projekt?
# Genau!
# Die, die in SOURCE_FILES stehen.
ADD_EXECUTABLE(TD_Tutorial ${SOURCE_FILES})


# Und zu guter letzt müssen noch die nötigen Bibliotheken zum Projekt gelinkt
# werden.
TARGET_LINK_LIBRARIES(TD_Tutorial ${SDL2_LIBRARIES} ${SDL2_Image_LIBRARIES})

# Wir haben Bilder.
# Die Bilder liegen in einem extra Ordner.
# Wenn wir einen Build machen, wäre es doch schön, wenn die Bilder auch an die
# passende Stelle kopiert werden. Dann können wir im Build-Ordner unser
# Spielchen testen.
# Der Unterordner hat seine eigene CMakeLists.txt
# Also führen wir unseren Unterordner hier mit auf.
ADD_SUBDIRECTORY(images)
