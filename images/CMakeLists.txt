# In dem Ordner, in dem sich diese Datei befindet, liegen sonst eignetlich nur
# Bilder herum. Diese wollen wir an ihren Zielord kopieren.
# Dazu brauchen wir erst einmal eine Liste mit den Bildern.
SET( IMAGE_FILES
	enemy.png
	turret.png
)

# Für jedes Bild in unserer Liste:
# kopieren wir dieses Bild an die passende Stelle im Build Ordner.
foreach( img ${IMAGE_FILES})
	CONFIGURE_FILE(${img} ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
endforeach()
